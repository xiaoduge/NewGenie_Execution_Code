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

 
#include "sapp_ex.h"
#if (IWDG_SUPPORT > 0)
#include "IWDG_Driver.h"
#endif

#include "mfddriver.h"



const char DataStr[]=__DATE__; 
const char TimeStr[]=__TIME__; 

//const char version[] = {"STM32F103_PWEA_V1.0"};
const char version[] = {"0.1.3.200408_93801"};

uint16_t gusDeviceId;

const char dtype[]   = {"SHLFPWEB"};
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

int Config_SaveMmInfo(UINT8 *pData)
{
    int iRet = write_mfd(pData);

    if (0 == iRet) 
    {
        mfd_Init(STM32F103_GPB(11),0);
    }
    return iRet;
}

int Config_GetMmInfo(UINT8 *pInData,UINT8 *pOutData)
{
	return read_mfd(pInData,pOutData);
}

void ConfigReset(void)
{
    SCB->AIRCR = 0X05FA0004; 
}


uint8 Config_Entry(uint8 *pCmd,uint8 *pRsp,uint8 CmdLen,uint8 *pucRspLen)
{
    uint8 ucRet = 0;

    //VOS_LOG(VOS_LOG_DEBUG,"%d,%d",pCmd[0],pCmd[1]); // pCmd[0] For length

    switch(pCmd[1])
    {
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
        memcpy(gSerialNoCfg.aucSerialNo,"PWEA-XXXX",9);
    }

    //printf("%d %d %d %d %d %d %d\r\n",gTslibLinearCfg.a[0],gTslibLinearCfg.a[1],gTslibLinearCfg.a[2],gTslibLinearCfg.a[3],gTslibLinearCfg.a[4],gTslibLinearCfg.a[5],gTslibLinearCfg.a[6]);

}

