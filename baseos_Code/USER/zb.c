#include    <ucos_ii.h>

#include    <cpu.h>
#include    <lib_def.h>
#include    <lib_mem.h>
#include    <lib_str.h>

#include    <string.h>

#include "stm32f4xx.h"

#include "CanCmd.h"

#include "memory.h"
#include "msg.h"
#include "timer.h"

#include "stm32_eval.h"


#include "app_cfg.h"

#include "Config.h"

#include "Beep.h"

#include "Errorcode.h"

#include "common.h"

#include "sapp_ex.h"

#include "cminterface.h"

#include "osal_snv.h"

#include "RTC_Driver.h"

#include "Display.h"

#include "Task.h"

#include "Relay.h"

#include "CanCmd.h"

#include "zbcmitf.h"

#include "serial.h"

#include "zb.h"


#define SBL_BUF_SIZE                 196
#define SBL_MAX_SIZE                (SBL_BUF_SIZE - RPC_FRAME_HDR_SZ - RPC_UART_FRAME_OVHD)

#define MAX_SERAIL_MESSAGE_LEN       SBL_BUF_SIZE

typedef struct
{
    unsigned char ucStage;
    unsigned char ucIndex;
    unsigned char ucMsgLen;
    unsigned char ucChecksum;
    unsigned char ucDummy; // place hold
    unsigned char Data[MAX_SERAIL_MESSAGE_LEN];
}SERIAL_BUS_PARSER;


typedef struct
{
    uint8_t ucInitReset;
    uint8_t ucState;

    uint8_t ucHsTimer;
    uint8_t ucHsTxCnt;
}ZIGBEE_STUR;

static SERIAL_BUS_PARSER  SerBusParser;

static ZIGBEE_STUR sZigbee;

static uint8_t sZigbeeBuffer[256];

void zb_Serial_InitParser(void)
{
    SerBusParser.ucStage = rpcSteSOF;
    SerBusParser.ucIndex = 0;
}


int zb_ParseMessage(uint8_t *pucData,int len)
{
    unsigned char ucData;

    int bgn = 0;
    
    while(bgn < len)
    {
        ucData = pucData[bgn++];
        
        switch(SerBusParser.ucStage)
        {
        case rpcSteSOF:
            if (RPC_UART_SOF == ucData)
            {
                SerBusParser.ucStage = rpcSteLen;
            }
            break;
        case rpcSteLen:
           if (ucData > SBL_MAX_SIZE)
           {
             SerBusParser.ucStage = rpcSteSOF;
             break;
           }
           else
           {
             SerBusParser.ucStage = rpcSteData;
             
             SerBusParser.ucChecksum = 0;
             SerBusParser.ucIndex = 0;
             SerBusParser.ucMsgLen = ucData + (RPC_FRAME_HDR_SZ );  // Combine the parsing of Len, Cmd0 & Cmd1 with the data for BUS TYPE NONE ,OTHER busitf.
             // no break;
           }            

        case rpcSteData:
            SerBusParser.ucChecksum ^= ucData;
            SerBusParser.Data[SerBusParser.ucIndex] = ucData;
            
            if (++SerBusParser.ucIndex == SerBusParser.ucMsgLen)
            {
              SerBusParser.ucStage = rpcSteFcs;
            }
            break;
        case rpcSteFcs:
          SerBusParser.ucStage = rpcSteSOF;
        
          if ((SerBusParser.ucChecksum == ucData)) //  && ((SerKeyParser.Data[RPC_POS_CMD0] & RPC_SUBSYSTEM_MASK) == RPC_SYS_BOOT
          {
            return 1; 
          }
          break;
        default:
          SerBusParser.ucStage = rpcSteSOF;
          SerBusParser.ucIndex = 0;
          break;
        }
    }
    return 0;
}



/**************************************************************************************************
 * @fn          zb_SerialResp
 *
 * @brief       Make the SB response.
 *
 * input parameters
 *
 * None.
 *
 * output parameters
 *
 * None.
 *
 * @return      TRUE if the downloaded code has been enabled; FALSE otherwise.
 */
void zb_SerialResp(uint8_t* sbTxBuf)
{
  uint8 fcs = 0, len = sbTxBuf[RPC_POS_LEN] + RPC_FRAME_HDR_SZ;
  uint8 idx;
    
  for ( idx = RPC_POS_LEN; idx < len; idx++)
  {
    fcs ^= sbTxBuf[idx];
  }
  sbTxBuf[len] = fcs;

  sbTxBuf[-1] = RPC_UART_SOF; // 

  Serial_FillSndBuf(SERIAL_PORT1,sbTxBuf-1, len + RPC_UART_FRAME_OVHD);

}

void zbReset(void)
{
    uint8_t *sbTxBuf = sZigbeeBuffer + 1;
    sZigbee.ucInitReset = 0;
    

    sbTxBuf[RPC_POS_LEN]  = 0; // no data
    sbTxBuf[RPC_POS_CMD0] = RPC_SYS_APP;
    sbTxBuf[RPC_POS_CMD1] = SAPP_CMD_RESET;
    
    zb_SerialResp(sbTxBuf);

    Disp_zbResetInd();
}

void zbPing(void)
{
    unsigned char buf[8];
    uint8_t *sbTxBuf = sZigbeeBuffer + 1;

    ZB_CMDITF_CMD_STRU *pCmd = NULL;       
    {
        pCmd = (ZB_CMDITF_CMD_STRU *)&buf[3];
        buf[0] = afAddr16Bit;
        buf[1] = 0 & 0XFF;
        buf[2] = 0 & 0XFF;
    }

    pCmd->ucLen = 3;
    pCmd->ucCmd = ZB_CMITF_CMD_PING;
    pCmd->ucObj = 0;
    pCmd->aucData[0] = 0;

    sbTxBuf[RPC_POS_LEN]  = pCmd->ucLen + 3; //  3bytes for zigbee shortaddr
    sbTxBuf[RPC_POS_CMD0] = RPC_SYS_APP;
    sbTxBuf[RPC_POS_CMD1] = SAPP_CMD_AIR_DATA;

    memcpy(&sbTxBuf[RPC_POS_DAT0], buf,sbTxBuf[RPC_POS_LEN]);
    
    zb_SerialResp(sbTxBuf);

    sZigbee.ucHsTxCnt++;
}



void zb_PrepareReset()
{
    sZigbee.ucState = 0;

    sZigbee.ucHsTxCnt = 0;

    sZigbee.ucInitReset = 1;

}


void zbCanItf(uint8_t *pucData)
{
    int len = pucData[RPC_POS_LEN] + RPC_FRAME_HDR_SZ;
    uint8_t *sbTxBuf = sZigbeeBuffer + 1;

    memcpy(&sbTxBuf[RPC_POS_LEN],pucData,len);

    sbTxBuf[RPC_POS_CMD0] = RPC_SYS_BOOT;

    zb_SerialResp(sbTxBuf);

}

void zb_config_cb(uint8_t ucPort)
{
    memset(&Serial[ucPort],0,offsetof(SERIAL_STRU,ucDriverType));

    Serial[ucPort].ucDriverType = MSG_DRIVER;
    Serial[ucPort].ucPortType   = RS232;
    Serial[ucPort].ucPortCtrl   = 0; // DONT CARE FOR RS232

    switch(ucPort)
    {
    case SERIAL_PORT3:
        Serial[ucPort].UsartDef = UART4;
        Serial[ucPort].iIrq     = UART4_IRQn;
        Serial[ucPort].iComIdx  = SERIAL_PORT3;
        break;
    }

    Serial_RetriveConfig(ucPort);

    Serial[ucPort].SerialConfig.BaundRate  = 115200;
    Serial[ucPort].SerialConfig.ucDataBits = BAUD_DATA_8BITS;
    Serial[ucPort].SerialConfig.ucStopBits = BAUD_STOP_1BITS;
    Serial[ucPort].SerialConfig.ucParity   = BAUD_PARITY_NO;

}

void zb_SapiProc(uint8_t *pucData,int len)
{
    switch(pucData[RPC_POS_CMD1])
    {
    case SAPI_CMD_DEV_STATE:
        {
            uint8_t ucState = sZigbee.ucState;
            sZigbee.ucState = pucData[RPC_POS_DAT0];

            if ((ucState != sZigbee.ucState)
                && (DEV_END_DEVICE == sZigbee.ucState))
            {
                CanCcbSndZigbeeIndMsg();
            }
        }
        break;
    }
}

uint8_t zb_AirDataProc(uint8_t *sbRxBuf)
{
    ZB_CMDITF_CMD_STRU *pCmd = (ZB_CMDITF_CMD_STRU *)&sbRxBuf[RPC_POS_DAT0];   

    switch(pCmd->ucCmd)
    {
    case ZB_CMITF_CMD_PING:
        sZigbee.ucHsTxCnt = 0;
        break;
    }
	return 0;
}

void zb_SappProc(uint8_t *pucData,int len)
{
    uint8_t ucRet;

    uint8_t sbRxBuf[256];

    switch(pucData[RPC_POS_CMD1])
    {
    case SAPP_CMD_DATA:
        {
            /* skip protol */
            sbRxBuf[RPC_POS_LEN]  = pucData[RPC_POS_LEN] - 1;
            sbRxBuf[RPC_POS_CMD0] = pucData[RPC_POS_CMD0];
            sbRxBuf[RPC_POS_CMD1] = pucData[RPC_POS_DAT0];

            memcpy(&sbRxBuf[RPC_POS_DAT0],pucData + RPC_POS_DAT0 + 1,len - RPC_FRAME_HDR_SZ - 1);
            
            ucRet = CanCcbAfProc(CAN_ZIGBEE_INDEX,sbRxBuf);
        }
        break;
    case SAPP_CMD_AIR_DATA:
        {
            memcpy(sbRxBuf,pucData,len);
            ucRet = zb_AirDataProc(sbRxBuf);
        }
        break;
    }
		
    (void )ucRet;
}

void zbSecondTask(void)
{
    if (DEV_END_DEVICE == sZigbee.ucState)
    {
        CanCcbSystemStartReportRegister(CAN_ZIGBEE_INDEX);

        if (0 == (sZigbee.ucHsTimer % 8) )
        {
           if (sZigbee.ucHsTxCnt >= 3)
           {
              zb_PrepareReset();
           }
           else
           {
              zbPing();
           }
        }
    }

    if (sZigbee.ucInitReset)
    {
        zbReset();
    }
}


void zb_ItfProcess(Message *pMsg)
{
    if (zb_ParseMessage((uint8_t *)pMsg->data,pMsg->msgHead.nMsgLen))
    {
       /* process frame */
       switch(SerBusParser.Data[RPC_POS_CMD0])
       {
       case RPC_SYS_BOOT:
          CanSndSappRawCmd2(0,CanAddress,RPC_SYS_RCN,SerBusParser.Data[RPC_POS_CMD1],&SerBusParser.Data[RPC_POS_DAT0],SerBusParser.Data[RPC_POS_LEN]);
          break;
       case RPC_SYS_APP:
          zb_SappProc(SerBusParser.Data,SerBusParser.ucMsgLen);
          break;
       case RPC_SYS_SAPI:
           zb_SapiProc(SerBusParser.Data,SerBusParser.ucMsgLen);
          break;
       }
    }

}


void zbInit(void)
{
   uint8_t ucPortIdx = SERIAL_PORT1;

   Serial[ucPortIdx].sccb = zb_config_cb;

   Serial[ucPortIdx].mcb  = zb_ItfProcess;

   SerialInitPort(ucPortIdx);   
   
   //sys_timeout(1000,SYS_TIMER_PERIOD,1000,Modbus_Scheduler_cb,NULL,&ModbusSheduler.to4Schedule); 
   zb_Serial_InitParser();

   zb_PrepareReset();
}

