#include    <ucos_ii.h>

#include    <cpu.h>

#include    <string.h>

#include "stm32f4xx.h"


#include "Config.h"

#include "UartCmd.h"

#include "serial.h"

#include "Relay.h"

#include "DICA.h"

#include "Beep.h"

 
#include "sapp.h"
#if (IWDG_SUPPORT > 0)
#include "IWDG_Driver.h"
#endif



const char DataStr[]=__DATE__; 
const char TimeStr[]=__TIME__; 

//const char version[] = {"STM32F103_PWEA_V1.0"};
const char version[] = {"0.1.1.190702_93801"};

uint16_t gusDeviceId;

const char dtype[]   = {"SHLF0702"};
UINT8 Config_buff[1024];

SERIALNO_CONFIG_STRU gSerialNoCfg;

uint8_t aDevType[NV_DTYPE_SIZE];

ADMIN_STRU gAdmin;


const char *Config_GetVersion(void)
{
    return &version[0];
}

uint8 Config_SetItem(uint16 id, uint16 len, void *buf)
{
    uint8 ucStatus = osal_snv_write(id,len,buf);
    return ucStatus;
}

uint8 Config_GetItem(uint16 id, uint16 len, void *buf)
{
    uint8 ucStatus = osal_snv_read(id,len,buf);
    
    return ucStatus;
}

void Config_Sapp_Cmd(uint8_t cmd,uint8_t *data, uint8_t len)
{
    {
        sbTxBuf[RPC_POS_LEN]  = len; // len for data area (NOT INCLUDE CMD0&CMD1&LEN itself)
        sbTxBuf[RPC_POS_CMD0] = RPC_SYS_APP;
        sbTxBuf[RPC_POS_CMD1] = cmd;
        
        memcpy(&sbTxBuf[RPC_POS_DAT0],data,len);

        SHZNAPP_SerialResp(sappItfPort);
    }
}

#define CONTENT_POS 2
void Config_SaveSerialNo(UINT8 *pData)
{
    memcpy(gSerialNoCfg.aucSerialNo,pData,DEVICE_SERIAL_NO_SIZE);

    Config_SetItem(APP_NV_SERIALNO_ID,DEVICE_SERIAL_NO_SIZE,&gSerialNoCfg);

}

int Config_GetSerialNo(uint8_t *pucSerial)
{

    memcpy(pucSerial,gSerialNoCfg.aucSerialNo,DEVICE_SERIAL_NO_SIZE);
    return DEVICE_SERIAL_NO_SIZE;
}


void ConfigReset(void)
{
    SCB->AIRCR = 0X05FA0004; 
}


/* |BLOCKLEN(2BYTES)|PATTERN LEN(1BYTE)|PATTERN|PORT(1BYTE)|TRIGTYPE(1BYTES)|RSPLEN(1BYTE)|RSPCONT|*/
uint8 ConfigRs485Ctrl(uint8 *pCmd,uint8 CmdLen,uint8 *pRsp,uint8 *pucRspLen)
{
    UINT8 *pCanMsg = (UINT8 *)&pCmd[CONTENT_POS];

    //UINT8 ucLen = CmdLen - 4; // uartcmd len

    switch(pCanMsg[0])
    {
    case CMD_CMD_HOST2CLIENT_RS485_CTRL_SEND: // add an pattern
        break;
    default:
        break;
    }

    return 0;
}


uint8 Config_Entry(uint8 *pCmd,uint8 *pRsp,uint8 CmdLen,uint8 *pucRspLen)
{
    uint8 ucRet = 0;

    //VOS_LOG(VOS_LOG_DEBUG,"%d,%d",pCmd[0],pCmd[1]); // pCmd[0] For length

    switch(pCmd[1])
    {
#if 0   
    case CMD_HOST2CLIENT_DEVICEID_SET:
        ucRet = ConfigSetDeviceId(pCmd,CmdLen,pRsp,pucRspLen);
        break;
    case CMD_HOST2CLIENT_DEVICEID_GET:
        ucRet = ConfigGetDeviceId(pCmd,CmdLen,pRsp,pucRspLen);
        break;
#endif      
    case CMD_HOST2CLIENT_BEEP:
        MainBeepWithDuration(1);
        break;
    case CMD_HOST2CLIENT_LIGHT:
        MainAlarmWithDuration(1);
        break;
    }

    return ucRet;
}

uint8 Config_Sapp_Entry(uint8 *pCmd,uint8 *pRsp,uint8 CmdLen,uint8 *pucRspLen)
{
   
   return Config_Entry(pCmd,pRsp,CmdLen,pucRspLen);
}

void Config_Init(void)
{

    memset(&gAdmin,0,sizeof(gAdmin));
    
    if ( osal_snv_read( NV_DEVID_ID, sizeof ( gusDeviceId ), &gusDeviceId ) != ERROR_SUCCESS )
    {
        gusDeviceId = 0;
    }

    if ( (osal_snv_read( NV_DTYPE_ID, NV_DTYPE_SIZE, &aDevType[0]) != ERROR_SUCCESS) 
        || (0 != memcmp(aDevType,dtype,NV_DTYPE_ID)))
    {
        memcpy(aDevType,dtype,NV_DTYPE_SIZE);
    
        Config_SetItem(NV_DTYPE_ID,NV_DTYPE_SIZE,aDevType);
    }

    if (ERROR_SUCCESS !=  Config_GetItem(APP_NV_SERIALNO_ID,DEVICE_SERIAL_NO_SIZE,gSerialNoCfg.aucSerialNo))
    {
        //memcpy(gSerialNoCfg.aucSerialNo,"PWEA-XXXX",9); //EX_DCJ
        memcpy(gSerialNoCfg.aucSerialNo,"PWEA123456",10);
    }

    //printf("%d %d %d %d %d %d %d\r\n",gTslibLinearCfg.a[0],gTslibLinearCfg.a[1],gTslibLinearCfg.a[2],gTslibLinearCfg.a[3],gTslibLinearCfg.a[4],gTslibLinearCfg.a[5],gTslibLinearCfg.a[6]);

}

