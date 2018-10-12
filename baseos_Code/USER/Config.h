#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "memory.h"
#include "msg.h"
#include "timer.h"

#include "App_cfg.h"

#include "osal_snv.h"

#include "common.h"

#include "DICA.h"

#include "cminterface.h"

// common for all

#pragma   pack(1)

typedef struct
{
    UINT16 usTgtAdr;    // for target adr
    UINT8  ucTgtObj;    // for target obj
    UINT8  ucTgtIdx;    // for target index
    UINT8  ucTgtOps;    // for target operation code
	UINT8  ucTgtData;   // delay output time
    UINT8  ucCtlType;   //  input trigger type
    UINT8  ucRsvd1;     //  reserved
}INPUT_MAP_STRU;

typedef struct
{
    UINT8  aucSrcAdr[2];
    UINT8  ucSrcObj;
	UINT8  ucSrcIdx;
	UINT8  ucDstObj;
	UINT8  ucDstIdx;
	UINT8  ucCtlLen;
	UINT8  aucData[1];
}CFG_PEER_CTL;

typedef enum
{
    APP_NV_LOCAL_ID   = 0x80,  // just for example
    APP_NV_SERIALNO_ID,
    

    APP_NV_BACKUP = 0x90,

}APP_NV_ID_ENUM;


#define CMD_HOST2CLIENT_LIGHT 0XF0   //
#define CMD_HOST2CLIENT_BEEP 0xF1    // 
#define CMD_HOST2CLIENT_QUERY_PIN 0xF4    //
#define CMD_HOST2CLIENT_BUZZ 0xF6        // 
#define CMD_HOST2CLIENT_RESET 0xF7        // 
#define CMD_HOST2CLIENT_MOTOR_CTRL 0xFA        // Pulse Oximeter Command
#define CMD_HOST2CLIENT_CAN_CTRL 0xFB        // local simulating can testing
#define CMD_HOST2CLIENT_RS485_CTRL 0xFD // ird configuration bin downloading
        #define CMD_CMD_HOST2CLIENT_RS485_CTRL_SEND 0
        #define CMD_CMD_HOST2CLIENT_RS485_CTRL_GET  1

#define CMD_HOST2CLIENT_PARAMETER_SET 0xD0        // 
#define CMD_HOST2CLIENT_PARAMETER_GET 0xD1        // 

#define CMD_HOST2CLIENT_DEVICEID_SET 0xD2        // 
#define CMD_HOST2CLIENT_DEVICEID_GET 0xD3        // 

#define CMD_HOST2CLIENT_CANID_SET 0xD4        // 
#define CMD_HOST2CLIENT_CANID_GET 0xD5        // 

#define CMD_HOST2CLIENT_DEVICE_RESET 0xD6     // RESET
#define CMD_HOST2CLIENT_FORCE_BOOT   0xd7


#define CMD_HOST2CLIENT_TIME_CTRL 0xC0        // 
#define CMD_HOST2CLIENT_TIME_GET  0xC1        // 
#define CMD_HOST2CLIENT_TIME_ADJ  0xC2        // 

#define CMD_CLIENT2HOST_REPORT 0X00  //管脚状态上报
#define CMD_CLIENT2HOST_PARAMETER_SET_REPORT 0X10  //管脚状态上报
#define CMD_CLIENT2HOST_ID_SET_REPORT 0X11  //设备ID上报
#define CMD_CLIENT2HOST_CANID_SET_REPORT 0X12  //设备ID上报

#define CMD_CLIENT2HOST_TUNNEL_REPORT 0X80  //设备ID上报

typedef struct
{
    UINT32 BaundRate;
    UINT8  ucDataBits;
    UINT8  ucStopBits;
    UINT8  ucParity;
    UINT8  ucIdleCheckThd;
    UINT16 usRsv;
}SERIAL_CONFIG_STRU;


typedef struct
{
    uint32_t ulCloudIp;
    uint16_t usCloudPort;
    uint8_t  ucCloudNameLen;
    char     aucCludeName[CLOUD_NAME_MAX_LENGTH+1];
}CLOUD_CONFIG_STRU;

typedef struct
{
    uint32_t ulLocalIp;
    uint32_t ulLocalAdrMsk;
    uint32_t ulGatewayIp;
    //uint32_t ulGatewayMsk;
    uint8_t  ucDhcp;
}LOCIP_CONFIG_STRU;

typedef struct
{
    uint16_t usHeatbeatTime;
}HBT_CONFIG_STRU;

#define MAX_DEVICE_SERIAL_NO_LENGTH  APP_SN_LENGTH

#define DEVICE_SERIAL_NO_SIZE        MAX_DEVICE_SERIAL_NO_LENGTH

typedef struct
{
    UINT8  aucSerialNo[DEVICE_SERIAL_NO_SIZE];
}SERIALNO_CONFIG_STRU;

#define ADMIN_TEXT_LENGTH (16)

typedef struct
{
    char NAME[ADMIN_TEXT_LENGTH+1]; // late implement
    char PSD[ADMIN_TEXT_LENGTH+1];

}ADMIN_STRU;

#define PARAMETER_SERIALNO_SIZE      (sizeof(SERIALNO_CONFIG_STRU))
#define PARAMETER_SERIAL_SIZE        (sizeof(SERIAL_CONFIG_STRU))
#define PARAMETER_CLOUD_SIZE         (sizeof(CLOUD_CONFIG_STRU))
#define PARAMETER_LOCIP_SIZE         (sizeof(LOCIP_CONFIG_STRU))
#define PARAMETER_HBT_SIZE           (sizeof(HBT_CONFIG_STRU))

#define INVALID_CONFIG_VALUE_SHORT (0XFFFF)

#define INVALID_CONFIG_VALUE_LONG  (0XFFFFFFFF)

typedef enum
{
    BAUD_DATA_8BITS = 0,
    BAUD_DATA_9BITS  ,
    BAUD_DATA_BITS_NUM,

}ENUM_BAUD_DATA_BITS;

typedef enum
{
    BAUD_STOP_1BITS = 0,
    BAUD_STOP_1_5_BITS  ,
    BAUD_STOP_2_BITS ,
    BAUD_STOP_BITS_NUM,

}ENUM_BAUD_STOP_BITS;

typedef enum
{
    BAUD_PARITY_NO = 0,
    BAUD_PARITY_EVEN  ,
    BAUD_PARITY_ODD ,
    BAUD_PARITY_NUM,
}ENUM_BAUD_PARITY;

typedef struct
{
    
    SERIALNO_CONFIG_STRU SerialNoCfg;

}CONFIG_BACKUP_STRU;


extern SERIALNO_CONFIG_STRU gSerialNoCfg;

extern UINT8 Config_buff[1024];

extern uint8_t aDevType[NV_DTYPE_SIZE];

extern uint16_t CanAddress ;

uint8 Config_SetItem(uint16 id, uint16 len, void *buf);
uint8 Config_GetItem(uint16 id, uint16 len, void *buf);
void Config_Init(void);

int Config_GetSerialNo(uint8_t *pucSerial);
void Config_SaveSerialNo(UINT8 *pData);

void ConfigReset(void);
uint8 Config_Sapp_Entry(uint8 *pCmd,uint8 *pRsp,uint8 CmdLen,uint8 *pucRspLen);

const char *Config_GetVersion(void);

#pragma pack()

#endif
