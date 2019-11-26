#include "display.h"

#include "Relay.h"

#include "Dica.h"

#include <string.h>

#include <stdio.h>

#include <math.h>

#include <stdlib.h>

#include "RTC_Driver.h"

#include "keyboard.h"

#include "task.h"

#include "IWDG_Driver.h"

#include "lib_def.h"

#include "UartCmd.h"

#include "Sapp.h"


#include "hal_defs.h"

#include "Timer_driver.h"

#include "valve.h"

#include "adc.h"

#include "CanCmd.h"

#include "ad840x.h"

#include "qmi.h"

#include "Beep.h"

#include "tocData.h"

#define FLOW1_PIN STM32F103_GPB(6)   
#define FLOW2_PIN STM32F103_GPB(7)   
#define FLOW3_PIN STM32F103_GPB(8)   
#define FLOW4_PIN STM32F103_GPB(9) 


DISPLAY_STRU Display;

static int  sSeconds;

void Disp_EcoReport(void)
{
    uint8_t ucLoop;
    uint8_t ucPayLoad = 0;
    uint8_t ucRptFlag = FALSE;

    APP_PACKET_CLIENT_RPT_IND_STRU *pRpt = (APP_PACKET_CLIENT_RPT_IND_STRU *)Config_buff;
    APP_PACKET_RPT_ECO_STRU        *pFm  = (APP_PACKET_RPT_ECO_STRU *)pRpt->aucData;

    if (Display.ausHoldRegs[HOLD_REGS_NAME_RPT0] & APP_EXE_ECO_REPORT_MASK)
    {
        pRpt->hdr.ucMsgType = APP_PACKET_CLIENT_REPORT;
        pRpt->hdr.ucDevType = APP_DEV_TYPE_EXE_BOARD;
        pRpt->hdr.ucTransId = APP_DEV_TYPE_EXE_BOARD;
        pRpt->ucRptType     = APP_PACKET_RPT_ECO;
        ucPayLoad++;
        for (ucLoop = 0; ucLoop < APP_EXE_MAX_ECO_NUMBER; ucLoop++)
        {
            if (Display.ausHoldRegs[HOLD_REGS_NAME_RPT0] & (1 << ucLoop))
            {
                uint8_t ucDataPtr = APP_EXE_INPUT_REG_QMI_OFFSET + ucLoop * 3;
 
                un_data_type ud = {0};
             
                pFm->ucId   = ucLoop;
 
                ud.auc[3] = (Display.ausInputRegs[ucDataPtr + 0] >> 8) & 0xff;
                ud.auc[2] = (Display.ausInputRegs[ucDataPtr + 0] >> 0) & 0xff;
                ud.auc[1] = (Display.ausInputRegs[ucDataPtr + 1] >> 8) & 0xff;
                ud.auc[0] = (Display.ausInputRegs[ucDataPtr + 1] >> 0) & 0xff;
                pFm->ev.fWaterQ = ud.f;
                pFm->ev.usTemp = Display.ausInputRegs[ucDataPtr + 2];
                pFm++;
 
                ucPayLoad += sizeof(APP_PACKET_RPT_ECO_STRU);
                ucRptFlag = TRUE;

                Display.ausInputRegsShadow[ucDataPtr + 0] = Display.ausInputRegs[ucDataPtr + 0];
                Display.ausInputRegsShadow[ucDataPtr + 1] = Display.ausInputRegs[ucDataPtr + 1];
                Display.ausInputRegsShadow[ucDataPtr + 2] = Display.ausInputRegs[ucDataPtr + 2];
            }
        }
        pRpt->hdr.ucLen = ucPayLoad;

        // broadcast
        if (ucRptFlag) CanSndSappCmd2(CAN_LOCAL_EXE_INDEX,APP_DEV_TYPE_MAIN_CTRL ,CanAddress,SAPP_CMD_DATA,(uint8_t *)pRpt,ucPayLoad + APP_PROTOL_HEADER_LEN); 
    }

}

void Disp_PmReport(void)
{
    uint8_t ucLoop;
    uint8_t ucPayLoad = 0;
    uint8_t ucRptFlag = FALSE;

    APP_PACKET_CLIENT_RPT_IND_STRU *pRpt = (APP_PACKET_CLIENT_RPT_IND_STRU *)Config_buff;
    APP_PACKET_RPT_PM_STRU         *pPm  = (APP_PACKET_RPT_PM_STRU *)pRpt->aucData;

    if (Display.ausHoldRegs[HOLD_REGS_NAME_RPT0]  & APP_EXE_PM_REPORT_MASK)
    {
        pRpt->hdr.ucMsgType = APP_PACKET_CLIENT_REPORT;
        pRpt->hdr.ucDevType = APP_DEV_TYPE_EXE_BOARD;
        pRpt->hdr.ucTransId = APP_DEV_TYPE_EXE_BOARD;
        pRpt->ucRptType     = APP_PACKET_RPT_PM;
        ucPayLoad++;
        for (ucLoop = 0; ucLoop < APP_EXE_PRESSURE_METER; ucLoop++)
        {
            if (Display.ausHoldRegs[HOLD_REGS_NAME_RPT0] & (1 << (ucLoop + APP_EXE_MAX_ECO_NUMBER)))
            {
                uint8_t ucDataPtr = APP_EXE_INPUT_REG_LOOP_OFFSET + ucLoop;
 
             
                pPm->ucId   = ucLoop;
 
                pPm->ulValue = Display.ausInputRegs[ucDataPtr ];

                Display.ausInputRegsShadow[ucDataPtr] = Display.ausInputRegs[ucDataPtr];
                
                ucPayLoad += sizeof(APP_PACKET_RPT_PM_STRU);
                pPm++;
                ucRptFlag = TRUE;
            }
        }
        pRpt->hdr.ucLen = ucPayLoad;

        if (ucRptFlag) CanSndSappCmd2(CAN_LOCAL_EXE_INDEX,APP_DEV_TYPE_MAIN_CTRL ,CanAddress,SAPP_CMD_DATA,(uint8_t *)pRpt,ucPayLoad + APP_PROTOL_HEADER_LEN); 
    }

}


void Disp_RectReport(void)
{
    uint8_t ucLoop;
    uint8_t ucPayLoad = 0;
    uint8_t ucRptFlag = FALSE;

    APP_PACKET_CLIENT_RPT_IND_STRU *pRpt = (APP_PACKET_CLIENT_RPT_IND_STRU *)Config_buff;
    APP_PACKET_RPT_RECT_STRU         *pPm  = (APP_PACKET_RPT_RECT_STRU *)pRpt->aucData;

    if (Display.ausHoldRegs[HOLD_REGS_NAME_RPT1]  & APP_EXE_RECT_REPORT_MASK)
    {
        pRpt->hdr.ucMsgType = APP_PACKET_CLIENT_REPORT;
        pRpt->hdr.ucDevType = APP_DEV_TYPE_EXE_BOARD;
        pRpt->hdr.ucTransId = APP_DEV_TYPE_EXE_BOARD;
        pRpt->ucRptType     = APP_PACKET_RPT_RECT;
        ucPayLoad++;
        for (ucLoop = 0; ucLoop < APP_EXE_RECT_NUM; ucLoop++)
        {
            if (Display.ausHoldRegs[HOLD_REGS_NAME_RPT1] & (1 << (ucLoop + APP_EXE_RECT_REPORT_POS)))
            {
                uint8_t ucDataPtr = APP_EXE_INPUT_REG_RECTIFIER_OFFSET + ucLoop;
 
             
                pPm->ucId   = ucLoop;
 
                pPm->ulValue = Display.ausInputRegs[ucDataPtr ];
                Display.ausInputRegsShadow[ucDataPtr] = Display.ausInputRegs[ucDataPtr];
                ucPayLoad += sizeof(APP_PACKET_RPT_RECT_STRU);
                pPm++;
                ucRptFlag = TRUE;
            }
        }
        pRpt->hdr.ucLen = ucPayLoad;

        if (ucRptFlag) CanSndSappCmd2(CAN_LOCAL_EXE_INDEX,APP_DEV_TYPE_MAIN_CTRL ,CanAddress,SAPP_CMD_DATA,(uint8_t *)pRpt,ucPayLoad + APP_PROTOL_HEADER_LEN); 
    }

}


void Disp_GPumpReport(void)
{
    uint8_t ucLoop;
    uint8_t ucPayLoad = 0;
    uint8_t ucRptFlag = FALSE;

    APP_PACKET_CLIENT_RPT_IND_STRU *pRpt = (APP_PACKET_CLIENT_RPT_IND_STRU *)Config_buff;
    APP_PACKET_RPT_GPUMP_STRU         *pPm  = (APP_PACKET_RPT_GPUMP_STRU *)pRpt->aucData;

    if (Display.ausHoldRegs[HOLD_REGS_NAME_RPT1]  & APP_EXE_GPUMP_REPORT_MASK)
    {
        pRpt->hdr.ucMsgType = APP_PACKET_CLIENT_REPORT;
        pRpt->hdr.ucDevType = APP_DEV_TYPE_EXE_BOARD;
        pRpt->hdr.ucTransId = APP_DEV_TYPE_EXE_BOARD;
        pRpt->ucRptType     = APP_PACKET_RPT_GPUMP;
        ucPayLoad++;
        for (ucLoop = 0; ucLoop < APP_EXE_G_PUMP_NUM; ucLoop++)
        {
            if (Display.ausHoldRegs[HOLD_REGS_NAME_RPT1] & (1 << (ucLoop + APP_EXE_GPUMP_REPORT_POS)))
            {
                uint8_t ucDataPtr = APP_EXE_INPUT_REG_PUMP_OFFSET + ucLoop;
 
             
                pPm->ucId   = ucLoop;
 
                pPm->ulValue = Display.ausInputRegs[ucDataPtr ];
                Display.ausInputRegsShadow[ucDataPtr] = Display.ausInputRegs[ucDataPtr];
                ucPayLoad += sizeof(APP_PACKET_RPT_GPUMP_STRU);
                pPm++;
                ucRptFlag = TRUE;
            }
        }
        pRpt->hdr.ucLen = ucPayLoad;

        if (ucRptFlag) CanSndSappCmd2(CAN_LOCAL_EXE_INDEX,APP_DEV_TYPE_MAIN_CTRL ,CanAddress,SAPP_CMD_DATA,(uint8_t *)pRpt,ucPayLoad + APP_PROTOL_HEADER_LEN); 
    }

}

void Disp_RPumpReport(void)
{
    uint8_t ucLoop;
    uint8_t ucPayLoad = 0;
    uint8_t ucRptFlag = FALSE;

    APP_PACKET_CLIENT_RPT_IND_STRU *pRpt = (APP_PACKET_CLIENT_RPT_IND_STRU *)Config_buff;
    APP_PACKET_RPT_RPUMP_STRU      *pPm  = (APP_PACKET_RPT_RPUMP_STRU *)pRpt->aucData;

    if (Display.ausHoldRegs[HOLD_REGS_NAME_RPT1]  & APP_EXE_RPUMP_REPORT_MASK)
    {
        pRpt->hdr.ucMsgType = APP_PACKET_CLIENT_REPORT;
        pRpt->hdr.ucDevType = APP_DEV_TYPE_EXE_BOARD;
        pRpt->hdr.ucTransId = APP_DEV_TYPE_EXE_BOARD;
        pRpt->ucRptType     = APP_PACKET_RPT_RPUMP;
        ucPayLoad++;
        for (ucLoop = 0; ucLoop < APP_EXE_R_PUMP_NUM; ucLoop++)
        {
            if (Display.ausHoldRegs[HOLD_REGS_NAME_RPT1] & (1 << (ucLoop + APP_EXE_RPUMP_REPORT_POS)))
            {
                uint8_t ucDataPtr = APP_EXE_INPUT_REG_REGPUMPI_OFFSET + ucLoop;
             
                uint8_t ucDatavPtr = APP_EXE_INPUT_REG_REGPUMPV_OFFSET + ucLoop;
                
                pPm->ucId   = ucLoop;
 
                pPm->ulValue = Display.ausInputRegs[ucDataPtr ];
                Display.ausInputRegsShadow[ucDataPtr] = Display.ausInputRegs[ucDataPtr];

                pPm->ulValue |= (Display.ausInputRegs[ucDatavPtr ] << 16);
                Display.ausInputRegsShadow[ucDatavPtr] = Display.ausInputRegs[ucDatavPtr];
                
                ucPayLoad += sizeof(APP_PACKET_RPT_RPUMP_STRU);
                pPm++;

                ucRptFlag = TRUE;
            }
        }
        pRpt->hdr.ucLen = ucPayLoad;

        if (ucRptFlag ) CanSndSappCmd2(CAN_LOCAL_EXE_INDEX,APP_DEV_TYPE_MAIN_CTRL ,CanAddress ,SAPP_CMD_DATA,(uint8_t *)pRpt,ucPayLoad + APP_PROTOL_HEADER_LEN); 
    }

}


void Disp_EDIReport(void)
{
    uint8_t ucLoop;
    uint8_t ucPayLoad = 0;
    uint8_t ucRptFlag = FALSE;

    APP_PACKET_CLIENT_RPT_IND_STRU *pRpt = (APP_PACKET_CLIENT_RPT_IND_STRU *)Config_buff;
    APP_PACKET_RPT_EDI_STRU         *pPm  = (APP_PACKET_RPT_EDI_STRU *)pRpt->aucData;

    if (Display.ausHoldRegs[HOLD_REGS_NAME_RPT1]  & APP_EXE_EDI_REPORT_MASK)
    {
        pRpt->hdr.ucMsgType = APP_PACKET_CLIENT_REPORT;
        pRpt->hdr.ucDevType = APP_DEV_TYPE_EXE_BOARD;
        pRpt->hdr.ucTransId = APP_DEV_TYPE_EXE_BOARD;
        pRpt->ucRptType     = APP_PACKET_RPT_EDI;
        ucPayLoad++;
        for (ucLoop = 0; ucLoop < APP_EXE_EDI_NUM; ucLoop++)
        {
            if (Display.ausHoldRegs[HOLD_REGS_NAME_RPT1] & (1 << (ucLoop + APP_EXE_EDI_REPORT_POS)))
            {
                uint8_t ucDataPtr = APP_EXE_INPUT_REG_EDI_OFFSET + ucLoop;
             
                pPm->ucId   = ucLoop;
                pPm->ulValue = Display.ausInputRegs[ucDataPtr ];
                Display.ausInputRegsShadow[ucDataPtr] = Display.ausInputRegs[ucDataPtr];
                ucPayLoad += sizeof(APP_PACKET_RPT_EDI_STRU);
                pPm++;
                ucRptFlag = TRUE;
            }
        }
        pRpt->hdr.ucLen = ucPayLoad;

        if (ucRptFlag) CanSndSappCmd2(CAN_LOCAL_EXE_INDEX,APP_DEV_TYPE_MAIN_CTRL ,CanAddress ,SAPP_CMD_DATA,(uint8_t *)pRpt,ucPayLoad + APP_PROTOL_HEADER_LEN); 
    }

}

void Disp_DinReport(uint8 ucState)
{
    uint8_t ucPayLoad = 0;

    APP_PACKET_CLIENT_RPT_IND_STRU  *pRpt  = (APP_PACKET_CLIENT_RPT_IND_STRU *)Config_buff;
    APP_PACKET_RPT_DIN_STRU         *pDin  = (APP_PACKET_RPT_DIN_STRU *)pRpt->aucData;

    {
        pRpt->hdr.ucMsgType = APP_PACKET_CLIENT_REPORT;
        pRpt->hdr.ucDevType = APP_DEV_TYPE_EXE_BOARD;
        pRpt->hdr.ucTransId = APP_DEV_TYPE_EXE_BOARD;
        pRpt->ucRptType     = APP_PACKET_DIN_STATUS;
        ucPayLoad++;

        pDin->ucState = ucState;
        ucPayLoad += sizeof(APP_PACKET_RPT_DIN_STRU);
        
        pRpt->hdr.ucLen = ucPayLoad;

        Display.ucState = ucState;

        Display.bit1PendingDinRpt = TRUE;

        CanSndSappCmd2(CAN_LOCAL_EXE_INDEX,APP_DEV_TYPE_MAIN_CTRL ,CanAddress ,SAPP_CMD_DATA,(uint8_t *)pRpt,ucPayLoad + APP_PROTOL_HEADER_LEN); 
    }

}

void Disp_DinStateChange(uint8 ucState)
{
    Display.ausInputRegs[APP_EXE_INPUT_REG_DIN_OFFSET] = ucState;

    if (Display.bit1PendingDinRpt)
    {
        Display.ucState = ucState;
    }
    else
    {
        Disp_DinReport(ucState);
    }
        
}

void Disp_DinReportAck(uint8 ucState)
{
    if (Display.ucState == ucState)
    {
        Display.bit1PendingDinRpt = FALSE;
    }
    else if (Display.bit1PendingDinRpt)
    {
        Disp_DinReport(Display.ucState);
    }

    
    //CanSndSappDbg(0,(uint8_t *)&ucState,1);
}

void Disp_TOCReport(void)
{
    uint8_t ucPayLoad = 0;

    APP_PACKET_CLIENT_RPT_IND_STRU *pRpt = (APP_PACKET_CLIENT_RPT_IND_STRU *)Config_buff;
    APP_PACKET_RPT_TOC_STRU        *pFm  = (APP_PACKET_RPT_TOC_STRU *)pRpt->aucData;

    pRpt->hdr.ucMsgType = APP_PACKET_CLIENT_REPORT;
    pRpt->hdr.ucDevType = APP_DEV_TYPE_EXE_BOARD;
    pRpt->hdr.ucTransId = APP_DEV_TYPE_EXE_BOARD;
    pRpt->ucRptType     = APP_PACKET_RPT_TOC;
    ucPayLoad++;
    pFm->fB = Display.fB;
    pFm->fP = Display.fP;

    ucPayLoad += sizeof(APP_PACKET_RPT_TOC_STRU);

    pRpt->hdr.ucLen = ucPayLoad;

    CanSndSappCmd2(CAN_LOCAL_EXE_INDEX,APP_DEV_TYPE_MAIN_CTRL ,CanAddress ,SAPP_CMD_DATA,(uint8_t *)pRpt,ucPayLoad + APP_PROTOL_HEADER_LEN); 

}
void Disp_FmReport(void)
{
    uint8_t ucLoop;
    uint8_t ucPayLoad = 0;

    APP_PACKET_CLIENT_RPT_IND_STRU *pRpt = (APP_PACKET_CLIENT_RPT_IND_STRU *)Config_buff;
    APP_PACKET_RPT_FM_STRU         *pFm  = (APP_PACKET_RPT_FM_STRU *)pRpt->aucData;

    if (Display.ausHoldRegs_fm[0])
    {
        pRpt->hdr.ucMsgType = APP_PACKET_CLIENT_REPORT;
        pRpt->hdr.ucDevType = APP_DEV_TYPE_FLOW_METER;
        pRpt->ucRptType     = APP_PACKET_RPT_FM;
        ucPayLoad++;
        for (ucLoop = 0; ucLoop < APP_FM_FLOW_METER_NUM; ucLoop++)
        {
            if (Display.ausHoldRegs_fm[0] & (1 << ucLoop))
            {
               pFm->ucId = ucLoop;
               pFm->ulValue = Display.aulInputRegs_fm[ucLoop]; // Display.aulInputRegs[ucLoop];
               pFm++;

               ucPayLoad += sizeof(APP_PACKET_RPT_FM_STRU);
            }
        }
        pRpt->hdr.ucLen = ucPayLoad;

        CanSndSappCmd2(CAN_LOCAL_FM_INDEX,APP_DEV_TYPE_MAIN_CTRL ,CanAddress + 1,SAPP_CMD_DATA,(uint8_t *)pRpt,ucPayLoad + APP_PROTOL_HEADER_LEN); 
    }

}


void Disp_SecondTask(void)
{
    sSeconds++;

    Disp_PmReport();

	//2019.01.07
	Disp_FmReport(); 
	switch((sSeconds % 2))
    {
    case 0:
        Disp_EcoReport();
        break;
    case 1:
        Disp_RectReport();
   
        Disp_GPumpReport();
   
        Disp_RPumpReport();
   
        Disp_EDIReport();        
        break;
    }
	//end
/*  
	//2019.1.7
    switch((sSeconds % 3))
    {
    case 0:
        Disp_FmReport(); //2019.01.07
        break;
    case 1:
        Disp_EcoReport();
        break;
    case 2:
        Disp_RectReport();
   
        Disp_GPumpReport();
   
        Disp_RPumpReport();
   
        Disp_EDIReport();        
        break;
    }
*/
    if (Display.bit1PendingDinRpt)
    {
        Disp_DinReport(Display.ucState);
    }
}

void Disp_ModbusReadInputRegister(uint8_t ucTrxIndex,MODBUS_PACKET_COMM_STRU *pModReq,MODBUS_PACKET_COMM_STRU *pModRsp)
{
    int iOffset = 0;

    int iIdx = 1;
    
    uint16_t usRegNum;
    
    uint16_t usStartAdr = (pModReq->aucData[iOffset] << 8) | (pModReq->aucData[iOffset + 1] );

    iOffset += 2;

    usRegNum = (pModReq->aucData[iOffset] << 8) | (pModReq->aucData[iOffset + 1] );

    iOffset += 2;

    switch(ucTrxIndex)
    {
    case CAN_LOCAL_EXE_INDEX:
        for (iOffset = usStartAdr; iOffset < usStartAdr + usRegNum; iOffset++)
        {
             pModRsp->aucData[iIdx + 0] = (Display.ausInputRegs[iOffset] >> 8) & 0XFF;
             pModRsp->aucData[iIdx + 1] = (Display.ausInputRegs[iOffset] >> 0) & 0XFF;
    
             iIdx += 2;
    
             if (iOffset >=  APP_EXE_MAX_INPUT_REGISTERS)
             {
                 goto end;
             }
        }        
        break;
    case CAN_LOCAL_FM_INDEX:
        usRegNum = ((usRegNum + 1)/2); // even aligned
    
        for (iOffset = 0; iOffset < usRegNum; iOffset++)
        {
             switch(iOffset*2 + usStartAdr)
             {
             case 0:
                pModRsp->aucData[iIdx + 0] = (Display.aulInputRegs_fm[0] >> 24) & 0XFF;
                pModRsp->aucData[iIdx + 1] = (Display.aulInputRegs_fm[0] >> 16) & 0XFF;
                pModRsp->aucData[iIdx + 2] = (Display.aulInputRegs_fm[0] >> 8) & 0XFF;
                pModRsp->aucData[iIdx + 3] = (Display.aulInputRegs_fm[0] >> 0) & 0XFF;
                iIdx += 4;
                break;
             case 2:
                 pModRsp->aucData[iIdx + 0] = (Display.aulInputRegs_fm[1] >> 24) & 0XFF;
                 pModRsp->aucData[iIdx + 1] = (Display.aulInputRegs_fm[1] >> 16) & 0XFF;
                 pModRsp->aucData[iIdx + 2] = (Display.aulInputRegs_fm[1] >> 8) & 0XFF;
                 pModRsp->aucData[iIdx + 3] = (Display.aulInputRegs_fm[1] >> 0) & 0XFF;
                 iIdx += 4;
                break;         
             case 4:
                 pModRsp->aucData[iIdx + 0] = (Display.aulInputRegs_fm[2] >> 24) & 0XFF;
                 pModRsp->aucData[iIdx + 1] = (Display.aulInputRegs_fm[2] >> 16) & 0XFF;
                 pModRsp->aucData[iIdx + 2] = (Display.aulInputRegs_fm[2] >> 8) & 0XFF;
                 pModRsp->aucData[iIdx + 3] = (Display.aulInputRegs_fm[2] >> 0) & 0XFF;
                 iIdx += 4;
                break;         
             case 6:
                 pModRsp->aucData[iIdx + 0] = (Display.aulInputRegs_fm[3] >> 24) & 0XFF;
                 pModRsp->aucData[iIdx + 1] = (Display.aulInputRegs_fm[3] >> 16) & 0XFF;
                 pModRsp->aucData[iIdx + 2] = (Display.aulInputRegs_fm[3] >> 8) & 0XFF;
                 pModRsp->aucData[iIdx + 3] = (Display.aulInputRegs_fm[3] >> 0) & 0XFF;
                 iIdx += 4;
                break;
             }
        }        
        break;
    }

end:
    
    pModRsp->aucData[0] = iIdx;

}

void Disp_ModbusReadHoldRegister(uint8_t ucTrxIndex,MODBUS_PACKET_COMM_STRU *pModReq,MODBUS_PACKET_COMM_STRU *pModRsp)
{
    int iOffset = 0;

    int iIdx = 1;
    
    uint16_t usRegNum;
    
    uint16_t usStartAdr = (pModReq->aucData[iOffset] << 8) | (pModReq->aucData[iOffset + 1] );

    iOffset += 2;

    usRegNum = (pModReq->aucData[iOffset] << 8) | (pModReq->aucData[iOffset + 1] );

    iOffset += 2;

    for (iOffset = usStartAdr; iOffset < usStartAdr + usRegNum; iOffset++)
    {
         switch(ucTrxIndex)
         {
         case CAN_LOCAL_EXE_INDEX:
            pModRsp->aucData[iIdx + 0] = (Display.ausHoldRegs[iOffset] >> 8) & 0XFF;
            pModRsp->aucData[iIdx + 1] = (Display.ausHoldRegs[iOffset] >> 0) & 0XFF;
       
            iIdx += 2;
       
            if (iOffset >=  APP_EXE_MAX_HOLD_REGISTERS)
            {
                goto end;
            }
            break;
        default:
            if (iOffset < APP_FM_MAX_HOLD_REGISTERS)
            {
                pModRsp->aucData[iIdx + 0] = (Display.ausHoldRegs_fm[iOffset] >> 8) & 0XFF;
                pModRsp->aucData[iIdx + 1] = (Display.ausHoldRegs_fm[iOffset] >> 0) & 0XFF;
                
                iIdx += 2;
            
            }      
            else
            {
                goto end;
            }
            break;
        }
    }

end:    
    
    pModRsp->aucData[0] = iIdx;

}


void Disp_ModbusReadInputStatus(uint8_t ucTrxIndex,MODBUS_PACKET_COMM_STRU *pModReq,MODBUS_PACKET_COMM_STRU *pModRsp)
{
    int iOffset = 0;

    uint16_t usRegNum;
    
    uint16_t usStartAdr = (pModReq->aucData[iOffset] << 8) | (pModReq->aucData[iOffset + 1] );

    iOffset += 2;

    usRegNum = (pModReq->aucData[iOffset] << 8) | (pModReq->aucData[iOffset + 1] );

    iOffset += 2;

    for (iOffset = usStartAdr; iOffset < usStartAdr + usRegNum; iOffset++)
    {
         uint8_t ucByte = (iOffset  - usStartAdr) / 8;
         uint8_t ucBits = (iOffset  - usStartAdr) % 8;

         if (iOffset >= INPUT_NUMBER)
         {
             break;
         }

         if (0 == ucBits)
         {
            pModRsp->aucData[1 + ucByte]  = GetInputLogicStatus(iOffset);
         }
         else
         {
            pModRsp->aucData[1 + ucByte] |= (GetInputLogicStatus(iOffset) << ucBits);
         }
    }
    
    pModRsp->aucData[0] = (iOffset -  usStartAdr + 7) / 8;

}

void Disp_ModbusForceSingleCoil(uint8_t ucTrxIndex,MODBUS_PACKET_COMM_STRU *pModReq,MODBUS_PACKET_COMM_STRU *pModRsp)
{
    int iOffset = 0;

    uint16_t usRegValue;
    
    uint16_t usStartAdr = (pModReq->aucData[iOffset] << 8) | (pModReq->aucData[iOffset + 1] );

    iOffset += 2;

    usRegValue = (pModReq->aucData[iOffset] << 8) | (pModReq->aucData[iOffset + 1] );

    iOffset += 2;

    if (ucTrxIndex == CAN_LOCAL_EXE_INDEX)
    {
        if (usStartAdr < VALVE_TOTAL_NUMBER)
        {
             ValveCtrl(usStartAdr,!!usRegValue);
        }
    }

    pModRsp->aucData[0] = pModReq->aucData[0];    
    pModRsp->aucData[1] = pModReq->aucData[1];    
    pModRsp->aucData[2] = pModReq->aucData[2];    
    pModRsp->aucData[3] = pModReq->aucData[3];    
    
}

void Disp_ModbusForceMultipleCoils(uint8_t ucTrxIndex,MODBUS_PACKET_COMM_STRU *pModReq,MODBUS_PACKET_COMM_STRU *pModRsp)
{
    int iOffset = 0;

    int iLoop;

    uint16_t usRegNum;
    
    uint16_t usStartAdr = (pModReq->aucData[iOffset] << 8) | (pModReq->aucData[iOffset + 1] );

    iOffset += 2;

    usRegNum = (pModReq->aucData[iOffset] << 8) | (pModReq->aucData[iOffset + 1] );

    iOffset += 2;

    iOffset += 1; // skip counts

    if (ucTrxIndex == CAN_LOCAL_EXE_INDEX)
    {
        for (iLoop = usStartAdr; iLoop < usStartAdr + usRegNum; iLoop++)
        {
             uint8_t ucByte = (iLoop - usStartAdr) / 8;
             uint8_t ucBits = (iLoop - usStartAdr)% 8;
    
             if (iLoop >= VALVE_TOTAL_NUMBER)
             {
                 break;
             }
    
             if (pModReq->aucData[iOffset + ucByte] & (1 << ucBits))
             {
                 ValveCtrl(iLoop,1);
             }
             else
             {
                 ValveCtrl(iLoop,0);
             }
        }
    }
    
    pModRsp->aucData[0] = pModReq->aucData[0];    
    pModRsp->aucData[1] = pModReq->aucData[1];    
    pModRsp->aucData[2] = pModReq->aucData[2];    
    pModRsp->aucData[3] = pModReq->aucData[3];    

}

void Disp_ModbusReadCoilStatus(uint8_t ucTrxIndex,MODBUS_PACKET_COMM_STRU *pModReq,MODBUS_PACKET_COMM_STRU *pModRsp)
{
    int iOffset = 0;

    uint16_t usRegNum;
    
    uint16_t usStartAdr = (pModReq->aucData[iOffset] << 8) | (pModReq->aucData[iOffset + 1] );

    iOffset += 2;

    usRegNum = (pModReq->aucData[iOffset] << 8) | (pModReq->aucData[iOffset + 1] );

    iOffset += 2;

    if (ucTrxIndex == CAN_LOCAL_EXE_INDEX)
    {
        for (iOffset = usStartAdr; iOffset < usStartAdr + usRegNum; iOffset++)
        {
             uint8_t ucByte = (iOffset - usStartAdr) / 8;
             uint8_t ucBits = (iOffset - usStartAdr) % 8;
    
             if (iOffset >= VALVE_TOTAL_NUMBER)
             {
                 break;
             }
    
             if (0 == ucBits)
             {
                pModRsp->aucData[1 + ucByte]  = ValveGetStatus(iOffset);
             }
             else
             {
                pModRsp->aucData[1 + ucByte] |= (ValveGetStatus(iOffset) << ucBits);
             }
        }

    }
    pModRsp->aucData[0] = (iOffset -  usStartAdr + 7) / 8;

}


void Disp_ModbusMaskWrite0XRegister(uint8_t ucTrxIndex,MODBUS_PACKET_COMM_STRU *pModReq,MODBUS_PACKET_COMM_STRU *pModRsp)
{
    int iOffset = 0;

    uint16_t usAndMask;

    uint16_t usOrMask;
    
    uint16_t usStartAdr = (pModReq->aucData[iOffset] << 8) | (pModReq->aucData[iOffset + 1] );

    iOffset += 2;

    usAndMask = (pModReq->aucData[iOffset] << 8) | (pModReq->aucData[iOffset + 1] );

    iOffset += 2;

    usOrMask = (pModReq->aucData[iOffset] << 8) | (pModReq->aucData[iOffset + 1] );

    iOffset += 2;

    if (ucTrxIndex == CAN_LOCAL_EXE_INDEX)
    {
        if (usStartAdr == 0) // only one register
        {
             usStartAdr = ValveGetAllStatus();
    
             for (iOffset = 0; iOffset < VALVE_TOTAL_NUMBER; iOffset++)
             {
                 if (usAndMask & (1 << iOffset))
                 {
                     if (usOrMask & (1 << iOffset))
                     {
                         ValveCtrl(iOffset,1);
                     }
                     else
                     {
                         ValveCtrl(iOffset,0);
                     }
                 }
             }
    
        }
    }
    pModRsp->aucData[0] = pModReq->aucData[0];    
    pModRsp->aucData[1] = pModReq->aucData[1];    
    pModRsp->aucData[2] = pModReq->aucData[2];    
    pModRsp->aucData[3] = pModReq->aucData[3];    
    pModRsp->aucData[4] = pModReq->aucData[4];    
    pModRsp->aucData[5] = pModReq->aucData[5];    

}

void Disp_ModbusPresetSingleRegister(uint8_t ucTrxIndex,MODBUS_PACKET_COMM_STRU *pModReq,MODBUS_PACKET_COMM_STRU *pModRsp)
{
    int iOffset = 0;

    uint16_t usValue ;
    
    uint16_t usStartAdr = (pModReq->aucData[iOffset] << 8) | (pModReq->aucData[iOffset + 1] );

    iOffset += 2;

    usValue = (pModReq->aucData[iOffset] << 8) | (pModReq->aucData[iOffset + 1] );

    iOffset += 2;

    pModRsp->aucData[0] = pModReq->aucData[0];
    pModRsp->aucData[1] = pModReq->aucData[1];
    pModRsp->aucData[2] = pModReq->aucData[2];
    pModRsp->aucData[3] = pModReq->aucData[3];

    switch(ucTrxIndex)
    {
    case CAN_LOCAL_EXE_INDEX:

        if (usStartAdr < APP_EXE_MAX_HOLD_REGISTERS)
        {
            Display.ausHoldRegs[usStartAdr] = usValue;
        
            switch(usStartAdr)
            {
            case HOLD_REGS_NAME_PUMP1:
            case HOLD_REGS_NAME_PUMP2:
                {
                    uint8_t ucChl   = APP_EXE_C1_NO + usStartAdr - HOLD_REGS_NAME_PUMP1;
                    uint8_t ucState = (usValue >> 8) & 0XFF;
                    ValveCtrl(ucChl,FALSE);
                    AD840x_Write(usStartAdr - HOLD_REGS_NAME_PUMP1,(usValue & 0xff));
                    ValveCtrl(ucChl,ucState);
                }
                break;
            default:
                break;
            }
        }
        break;
    case CAN_LOCAL_FM_INDEX:
        
        if (usStartAdr < APP_FM_MAX_HOLD_REGISTERS)
        {
            Display.ausHoldRegs_fm[usStartAdr] = usValue;
        }
        break;
    
    }

}

void Disp_ModbusPresetMultipleRegister(uint8_t ucTrxIndex,MODBUS_PACKET_COMM_STRU *pModReq,MODBUS_PACKET_COMM_STRU *pModRsp)
{
    int iOffset = 0;

    uint16_t usNums ;

    uint16_t usLoop;
    
    uint16_t usStartAdr = (pModReq->aucData[iOffset] << 8) | (pModReq->aucData[iOffset + 1] );

    uint16_t usValue;

    iOffset += 2;

    usNums = (pModReq->aucData[iOffset] << 8) | (pModReq->aucData[iOffset + 1] );

    iOffset += 2;

    iOffset++; // skip count 

    for (usLoop = 0; usLoop < usNums; usLoop++)
    {
        usValue = (pModReq->aucData[iOffset] << 8) | (pModReq->aucData[iOffset + 1] );

        switch(ucTrxIndex)
        {
        case CAN_LOCAL_EXE_INDEX:
            if (usStartAdr + usLoop < APP_EXE_MAX_HOLD_REGISTERS)
            {
                Display.ausHoldRegs[usStartAdr + usLoop] = usValue;

                switch(usStartAdr + usLoop)
                {
                case HOLD_REGS_NAME_PUMP1:
                case HOLD_REGS_NAME_PUMP2:
                    {
                        uint8_t ucChl   = APP_EXE_C1_NO + usStartAdr + usLoop - HOLD_REGS_NAME_PUMP1;
                        uint8_t ucState = (usValue >> 8) & 0XFF;
                        ValveCtrl(ucChl,FALSE);
                        AD840x_Write(usStartAdr + usLoop - HOLD_REGS_NAME_PUMP1,(usValue & 0XFF));
                        ValveCtrl(ucChl,ucState);
                    }                
                    break;
                default:
                    break;
                }            
            }
            break;
        case CAN_LOCAL_FM_INDEX:
            if (usStartAdr + usLoop < APP_FM_MAX_HOLD_REGISTERS)
            {
                Display.ausHoldRegs_fm[usStartAdr + usLoop] = usValue;
            }
            
            break;
        }
  
        iOffset += 2;
    }

    pModRsp->aucData[0] = pModReq->aucData[0];
    pModRsp->aucData[1] = pModReq->aucData[1];
    pModRsp->aucData[2] = pModReq->aucData[2];
    pModRsp->aucData[3] = pModReq->aucData[3];

}


void Disp_ModbusMaskWrite4XRegister(uint8_t ucTrxIndex,MODBUS_PACKET_COMM_STRU *pModReq,MODBUS_PACKET_COMM_STRU *pModRsp)
{
    int iOffset = 0;

    uint16_t usAndMask;

    uint16_t usOrMask;
    
    uint16_t usStartAdr = (pModReq->aucData[iOffset] << 8) | (pModReq->aucData[iOffset + 1] );

    iOffset += 2;

    usAndMask = (pModReq->aucData[iOffset] << 8) | (pModReq->aucData[iOffset + 1] );

    iOffset += 2;

    usOrMask = (pModReq->aucData[iOffset] << 8) | (pModReq->aucData[iOffset + 1] );

    iOffset += 2;

    switch(ucTrxIndex)
    {
    case CAN_LOCAL_EXE_INDEX:
        if (usStartAdr < APP_EXE_MAX_HOLD_REGISTERS) // only one register
        {
             for (iOffset = 0; iOffset < 16; iOffset++)
             {
                 if (usAndMask & (1 << iOffset))
                 {
                     if (usOrMask & (1 << iOffset))
                     {
                         Display.ausHoldRegs[usStartAdr] |= (1 << iOffset);
                     }
                     else
                     {
                         Display.ausHoldRegs[usStartAdr] &= ~(1 << iOffset);
                     }
                 }
             }
    
             switch(usStartAdr)
             {
             case HOLD_REGS_NAME_PUMP1:
             case HOLD_REGS_NAME_PUMP2:
                 {
                     uint8_t ucChl   = APP_EXE_C1_NO + usStartAdr - HOLD_REGS_NAME_PUMP1;
                     uint8_t ucState = (Display.ausHoldRegs[usStartAdr] >> 8)& 0xff;
                     ValveCtrl(ucChl,FALSE);
                     AD840x_Write(usStartAdr - HOLD_REGS_NAME_PUMP1,(Display.ausHoldRegs[usStartAdr] & 0xFF));
                     ValveCtrl(ucChl,ucState);
                 }                
                 break;
             default:
                 break;
             }            
             
    
        }        
        break;
    case CAN_LOCAL_FM_INDEX:
        if (usStartAdr < APP_FM_MAX_HOLD_REGISTERS) // only one register
        {
            for (iOffset = 0; iOffset < 16; iOffset++)
            {
                if (usAndMask & (1 << iOffset))
                {
                    if (usOrMask & (1 << iOffset))
                    {
                        Display.ausHoldRegs_fm[usStartAdr] |= (1 << iOffset);
                    }
                    else
                    {
                        Display.ausHoldRegs_fm[usStartAdr] &= ~(1 << iOffset);
                    }
                }
            }
    
        }
                
        break;
    }

    
    pModRsp->aucData[0] = pModReq->aucData[0];    
    pModRsp->aucData[1] = pModReq->aucData[1];    
    pModRsp->aucData[2] = pModReq->aucData[2];    
    pModRsp->aucData[3] = pModReq->aucData[3];    
    pModRsp->aucData[4] = pModReq->aucData[4];    
    pModRsp->aucData[5] = pModReq->aucData[5];    

}


void Disp_CanItfNotify(uint8_t ucTrxIndex,int code)
{
    switch(code)
    {
    case DISP_CANITF_CODE_LOST_HB:
        {
            int iLoop;
            
            Display.ausHoldRegs[0] = 0;
            Display.ausHoldRegs[1] = 0;
            for (iLoop = 0; iLoop < VALVE_TOTAL_NUMBER; iLoop++)
            {
                ValveCtrl(iLoop,FALSE);
            }
        }
        break;
    default:
        break;
    }
}


void Disp_CalcTocBPhase(void)
{
    float fTotal = 0;
    int   iLoop;
    int   iIdx;
    
    if (Display.ucTocNum < APP_TOC_SAMPLES_PER_SECOND*APP_TOC_OXIDIZATION_SECOND)
    {
        for (iLoop = 0; iLoop < Display.ucTocNum; iLoop++)
        {
            fTotal += Display.aTocData[iLoop].fToc;
        }
        fTotal /= Display.ucTocNum;
    }
    else
    {
        for (iLoop = 0; iLoop < APP_TOC_SAMPLES_PER_SECOND*APP_TOC_OXIDIZATION_SECOND; iLoop++)
        {
            iIdx    = (Display.ucTocIdx + DISP_TOC_SAMPLES - 1 - iLoop) % DISP_TOC_SAMPLES;
            fTotal += Display.aTocData[iIdx].fToc;
        }

        fTotal /= APP_TOC_SAMPLES_PER_SECOND*APP_TOC_OXIDIZATION_SECOND;
    }

    Display.fBRaw = fTotal;

    iIdx    = (Display.ucTocIdx + DISP_TOC_SAMPLES - 1 - iLoop) % DISP_TOC_SAMPLES;

    Display.fB = (1+(25-(Display.aTocData[iIdx].usTemp / 10))*0.03)*fTotal;
}

//2019.7.2 modify
void Disp_AddTocMeasurement(float value,uint16_t usTemp)
{
	static TocValues tocValues;
	static int isInit;
	static int samplesNum;


    if(APP_PACKET_EXE_TOC_STAGE_OXDIZATION == Display.ucTocStage)
    {
        isInit = 1;
		samplesNum = 0;
	    printf("TO: %f \r\n", value);
	}

    if (APP_PACKET_EXE_TOC_STAGE_FLUSH2 == Display.ucTocStage)
    {
		printf("TF: %f \r\n", value);		
		
		if(isInit && (samplesNum < D_SAMPLESNUM))
		{
			addTocValue(&tocValues, value);
			samplesNum++;
		}
		
		if(samplesNum == D_SAMPLESNUM)
		{
			double tempValue = tocValues.value;
            printf("TOC Value: %f  \n", tempValue);	
            
            appQmi_I4set(FALSE);
            Display.fP = tempValue;
            Display.fB = 0;
            Disp_TOCReport();
            
			memset(&tocValues, 0, sizeof(TocValues)); //計算完成后清零
			isInit = 0;
			samplesNum = 0;

        }
				      
    }
}

void Disp_CanItfToc(APP_PACKET_EXE_TOC_REQ_STRU *pToc)
{
    Display.ucTocStage = pToc->ucStage;

    switch(Display.ucTocStage)
    {
    case APP_PACKET_EXE_TOC_STAGE_OXDIZATION:
        appQmi_I4set(TRUE);
        break;
    case APP_PACKET_EXE_TOC_STAGE_FLUSH2:
        /* Calc Oxidization period */
        //Disp_CalcTocBPhase();
        break;
    default:
        break;
    }
    Display.ucTocNum = 0;
    Display.ucTocIdx = 0;

}

void Disp_Reset(void)
{
    Display.ucState           = 0;
    Display.bit1PendingDinRpt = FALSE;
}

int FlowMeter4_sh(int event,int chl,void *para)
{

    Display.aulInputRegs_fm[chl - DI_SENSOR7]++;

    return 0;
}


void Disp_zbResetInd(void)
{
    Display.ucTrxMap &= ~(1 << APP_TRX_ZIGBEE);
}

uint8_t Disp_GetTrxIndex(void)
{
    uint8_t ucTrxIndex = APP_TRX_CAN;
   
    if (Display.ucTrxMap & (1 << APP_TRX_CAN))
    {
        ucTrxIndex = APP_TRX_CAN;
    }
    else if (Display.ucTrxMap & (1 << APP_TRX_ZIGBEE))
    {
        ucTrxIndex = APP_TRX_ZIGBEE;
    }

    return ucTrxIndex;
}
  

void Disp_Init(void)
{
    memset(&Display,0,sizeof(DISPLAY_STRU));
    /* flow 4 use io */
    stm32_gpio_cfgpin(FLOW1_PIN,MAKE_PIN_CFG(GPIO_PuPd_NOPULL,GPIO_OType_PP,GPIO_Speed_50MHz,GPIO_Mode_IN)); 
    stm32_gpio_cfg_irq(FLOW1_PIN,EXTI_Trigger_Rising);
    InstallSensorHandler(DICA_SENSOR_EVENT_RISING,stm32_gpio_get_ext_line(FLOW1_PIN),FALSE,DICA_TYPE_PERIOD,FlowMeter4_sh,NULL);

    stm32_gpio_cfgpin(FLOW2_PIN,MAKE_PIN_CFG(GPIO_PuPd_NOPULL,GPIO_OType_PP,GPIO_Speed_50MHz,GPIO_Mode_IN)); 
    stm32_gpio_cfg_irq(FLOW2_PIN,EXTI_Trigger_Rising);
    InstallSensorHandler(DICA_SENSOR_EVENT_RISING,stm32_gpio_get_ext_line(FLOW2_PIN),FALSE,DICA_TYPE_PERIOD,FlowMeter4_sh,NULL);
    
    stm32_gpio_cfgpin(FLOW3_PIN,MAKE_PIN_CFG(GPIO_PuPd_NOPULL,GPIO_OType_PP,GPIO_Speed_50MHz,GPIO_Mode_IN)); 
    stm32_gpio_cfg_irq(FLOW3_PIN,EXTI_Trigger_Rising);
    InstallSensorHandler(DICA_SENSOR_EVENT_RISING,stm32_gpio_get_ext_line(FLOW3_PIN),FALSE,DICA_TYPE_PERIOD,FlowMeter4_sh,NULL);
    
    stm32_gpio_cfgpin(FLOW4_PIN,MAKE_PIN_CFG(GPIO_PuPd_NOPULL,GPIO_OType_PP,GPIO_Speed_50MHz,GPIO_Mode_IN)); 
    stm32_gpio_cfg_irq(FLOW4_PIN,EXTI_Trigger_Rising);
    InstallSensorHandler(DICA_SENSOR_EVENT_RISING,stm32_gpio_get_ext_line(FLOW4_PIN),FALSE,DICA_TYPE_PERIOD,FlowMeter4_sh,NULL);

}

