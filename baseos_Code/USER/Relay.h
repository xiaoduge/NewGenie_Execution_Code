#ifndef _RELAY_H_
#define _RELAY_H_

#include "stm32_eval.h"

#include "DtypeStm32.h"

typedef enum
{
   VPUMP1_CTRL_EN_N       = 0,  // 紫外灯开启信号，高有效，默认关闭
   VPUMP2_CTRL_EN_N,
   RECTIFIER1_CTRL_EN,
   RECTIFIER2_CTRL_EN,
   RECTIFIER3_CTRL_EN,
   EDI_CTRL_EN,
   VALVE1_CTRL_EN,
   VALVE2_CTRL_EN,
   VALVE3_CTRL_EN,
   VALVE4_CTRL_EN,
   VALVE5_CTRL_EN,
   VALVE6_CTRL_EN,
   VALVE7_CTRL_EN,
   VALVE8_CTRL_EN,
   PUMP1_CTRL_EN,
   PUMP2_CTRL_EN,
   RELAY_NUMBER,
}RELAY_ENUM;

#define MAX_OUTPUT_PORT_NUMBER RELAY_NUMBER

#define OUTPUT_MASK ((1<<MAX_OUTPUT_PORT_NUMBER)-1)

#define MAX_RELAY_NUMBER  (RELAY_NUMBER)
typedef void (*RelayPulse_Cb)(void);

void InitRelays(void);
void RelayLogicCtrl(UINT8 ucChannel,UINT8 ucEnable);
UINT8 GetRelayLogicStatus(UINT8 ucChannel);
UINT16 GetAllRelayLogicStatus(void);
void RelayToggle(UINT8 ucChannel);
void RelayLogicTurnoffAll(void);
void RelayLogicTurnonAll(void);
void RelayPulse(UINT8 ucChannel,uint32_t duration,RelayPulse_Cb cb);

#endif
