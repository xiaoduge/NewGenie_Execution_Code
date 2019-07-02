#ifndef _DISPLAY_H_
#define _DISPLAY_H_

//#define TOC
//#define UF_FUNCTION


#include "stm32_eval.h"
#include "DtypeStm32.h"
#include "config.h"
#include "system_time.h "

#include "cminterface.h"

typedef enum
{
    DISP_CANITF_CODE_LOST_HB = 0, /* lost heart beat */

}DISP_CANITF_CODE_ENUM;

typedef enum
{
  HOLD_REGS_NAME_RPT0 = 0,
  HOLD_REGS_NAME_RPT1,	
  HOLD_REGS_NAME_PUMP1,  
  HOLD_REGS_NAME_PUMP2,  
}HOLD_REGS_NAME_ENUM;

//2*2 + 4
#define DISP_TOC_SAMPLES (APP_TOC_SAMPLES_PER_SECOND*2 + APP_TOC_FLUSH2_SAMPLES)

typedef union                                        
{
   float          f;
   unsigned char  auc[4];
   int            i;
   unsigned int   ul;
}un_data_type;

typedef struct
{
    float fToc;
    uint16_t usTemp;
}DISP_TOC_ITEM;
typedef struct
{
    /* for exe */

    uint32_t bit1PendingDinRpt : 1;

    uint8_t ucState;

    uint16_t ausInputRegs[APP_EXE_MAX_INPUT_REGISTERS];

    uint16_t ausInputRegsShadow[APP_EXE_MAX_INPUT_REGISTERS];

    uint16_t ausHoldRegs[APP_EXE_MAX_HOLD_REGISTERS];

    /* for FM */
    uint32_t  aulInputRegs_fm[APP_EXE_INPUT_REG_FM_NUM];
    uint16_t  ausHoldRegs_fm[1];
    /* for TOC Measurement */
    DISP_TOC_ITEM aTocData[DISP_TOC_SAMPLES];
    uint8_t  ucTocNum;
    uint8_t  ucTocIdx;
    uint8_t  ucTocStage;

    uint8_t  ucTrxMap;   /* refer APP_TRX_TYPE_ENUM */

    float fPRaw;
    float fBRaw;

    float fP;
    float fB;
    
}DISPLAY_STRU;

extern DISPLAY_STRU Display;

void Disp_Init(void);
void Disp_SecondTask(void);
void Disp_Reset(void);
void Disp_ModbusReadInputRegister(uint8_t ucTrxIndex,MODBUS_PACKET_COMM_STRU *pModReq,MODBUS_PACKET_COMM_STRU *pModRsp);
void Disp_ModbusReadHoldRegister(uint8_t ucTrxIndex,MODBUS_PACKET_COMM_STRU *pModReq,MODBUS_PACKET_COMM_STRU *pModRsp);
void Disp_ModbusReadInputStatus(uint8_t ucTrxIndex,MODBUS_PACKET_COMM_STRU *pModReq,MODBUS_PACKET_COMM_STRU *pModRsp);
void Disp_ModbusReadCoilStatus(uint8_t ucTrxIndex,MODBUS_PACKET_COMM_STRU *pModReq,MODBUS_PACKET_COMM_STRU *pModRsp);
void Disp_ModbusMaskWrite0XRegister(uint8_t ucTrxIndex,MODBUS_PACKET_COMM_STRU *pModReq,MODBUS_PACKET_COMM_STRU *pModRsp);
void Disp_ModbusMaskWrite4XRegister(uint8_t ucTrxIndex,MODBUS_PACKET_COMM_STRU *pModReq,MODBUS_PACKET_COMM_STRU *pModRsp);
void Disp_ModbusForceMultipleCoils(uint8_t ucTrxIndex,MODBUS_PACKET_COMM_STRU *pModReq,MODBUS_PACKET_COMM_STRU *pModRsp);
void Disp_ModbusForceSingleCoil(uint8_t ucTrxIndex,MODBUS_PACKET_COMM_STRU *pModReq,MODBUS_PACKET_COMM_STRU *pModRsp);
void Disp_ModbusPresetSingleRegister(uint8_t ucTrxIndex,MODBUS_PACKET_COMM_STRU *pModReq,MODBUS_PACKET_COMM_STRU *pModRsp);
void Disp_ModbusPresetMultipleRegister(uint8_t ucTrxIndex,MODBUS_PACKET_COMM_STRU *pModReq,MODBUS_PACKET_COMM_STRU *pModRsp);
void Disp_DinReportAck(uint8 ucState);
void Disp_DinReport(uint8 ucState);
void Disp_DinStateChange(uint8 ucState);
void Disp_CanItfToc(APP_PACKET_EXE_TOC_REQ_STRU *pToc);
void Disp_AddTocMeasurement(float fToc,uint16_t usTemp);
void Disp_zbResetInd(void);

#endif
