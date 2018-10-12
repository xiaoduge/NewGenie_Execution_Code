/*! @file radio.c
 * @brief This file contains functions to interface with the radio chip.
 *
 * @b COPYRIGHT
 * @n Silicon Laboratories Confidential
 * @n Copyright 2012 Silicon Laboratories, Inc.
 * @n http://www.silabs.com
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "task.h"

#include "ctype.h"

#include "system_time.h "

#include "adc.h"

#include "valve.h"

#include "relay.h"

#include "ad840x.h"

#include "Display.h"


VALVE_STRU gValve;


void ValveInit(void)
{
    uint8_t ucLoop;

    uint8_t ucStart = 0;

    memset(&gValve,0xFF,sizeof(VALVE_STRU));

    /* for magnetic gValve 1~8 */
    for (ucLoop = ucStart; ucLoop < ucStart + VALVE_MAGNETIC_NUM; ucLoop++)
    {
        gValve.aValves[ucLoop].ucType         = VALVE_TYPE_MAGNETIC;
        gValve.aValves[ucLoop].ucRelayIndex   = VALVE1_CTRL_EN + (ucLoop - ucStart);
        gValve.aValves[ucLoop].ucEnableLevel  = 1;

        gValve.aValves[ucLoop].ucAdcChl4I     = 0XFF;
        gValve.aValves[ucLoop].ucAdcChl4V     = 0XFF;
    }

    ucStart += VALVE_MAGNETIC_NUM;
    
    /* for general pump 1~2 */
    for (ucLoop = ucStart; ucLoop < ucStart + VALVE_GENERAL_PUMP_NUM; ucLoop++)
    {
        gValve.aValves[ucLoop].ucType         = VALVE_TYPE_GENERAL_PUMP;
        gValve.aValves[ucLoop].ucRelayIndex   = PUMP1_CTRL_EN + ucLoop - ucStart;
        gValve.aValves[ucLoop].ucEnableLevel  = 1;

        gValve.aValves[ucLoop].ucAdcChl4I     = 7 + ucLoop - ucStart;
        gValve.aValves[ucLoop].ucAdcData4IPtr = APP_EXE_INPUT_REG_PUMP_OFFSET + ucLoop - ucStart;//  index into input register files

    }

    ucStart += VALVE_GENERAL_PUMP_NUM;

    /* for pressure  regulating pump 1~2 */
    for (ucLoop = ucStart; ucLoop < ucStart + VALVE_PRESSURE_REGULATING_PUMP_NUM; ucLoop++)
    {
        gValve.aValves[ucLoop].ucType         = VALVE_TYPE_PRESSURE_REGULATING_PUMP;
        gValve.aValves[ucLoop].ucRelayIndex   = VPUMP1_CTRL_EN_N + ucLoop - ucStart;
        gValve.aValves[ucLoop].ucEnableLevel  = 0;

        gValve.aValves[ucLoop].ucAdcChl4I     = 3 + ucLoop - ucStart;
        gValve.aValves[ucLoop].ucAdcData4IPtr = APP_EXE_INPUT_REG_REGPUMPI_OFFSET + ucLoop - ucStart;//  index into input register files

        gValve.aValves[ucLoop].ucAdcChl4V     = 5 + ucLoop - ucStart;;    
        gValve.aValves[ucLoop].ucAdcData4VPtr = APP_EXE_INPUT_REG_REGPUMPV_OFFSET + ucLoop - ucStart;//  index into input register files

    }

    ucStart += VALVE_PRESSURE_REGULATING_PUMP_NUM;
    
    /* for rectifier 1~3 */
    for (ucLoop = ucStart; ucLoop < ucStart + VALVE_RECTIFIER_NUM; ucLoop++)
    {
        gValve.aValves[ucLoop].ucType         = VALVE_TYPE_RECTIFIER;
        gValve.aValves[ucLoop].ucRelayIndex   = RECTIFIER1_CTRL_EN + ucLoop - ucStart;
        gValve.aValves[ucLoop].ucEnableLevel  = 1;

        gValve.aValves[ucLoop].ucAdcChl4I     = 10 + ucLoop - ucStart;
        gValve.aValves[ucLoop].ucAdcData4IPtr = APP_EXE_INPUT_REG_RECTIFIER_OFFSET + ucLoop - ucStart;//  index into input register files
    }

    ucStart += VALVE_RECTIFIER_NUM;
    
    /* for edi 1~1 */
    for (ucLoop = ucStart; ucLoop < ucStart + VALVE_EDI_NUM; ucLoop++)
    {
        gValve.aValves[ucLoop].ucType         = VALVE_TYPE_EDI;
        gValve.aValves[ucLoop].ucRelayIndex   = ucLoop - ucStart + EDI_CTRL_EN;
        gValve.aValves[ucLoop].ucEnableLevel  = 1;

        gValve.aValves[ucLoop].ucAdcChl4I     = 9 + ucLoop - ucStart;
        gValve.aValves[ucLoop].ucAdcData4IPtr = APP_EXE_INPUT_REG_EDI_OFFSET + ucLoop - ucStart;//  index into input register files
    }

    
}


void ValveCtrl(uint8_t ucChl,uint8_t ucEnable)
{
    RelayLogicCtrl(gValve.aValves[ucChl].ucRelayIndex,gValve.aValves[ucChl].ucEnableLevel ? ucEnable : !ucEnable);

    // active / deactive measurement
    switch(gValve.aValves[ucChl].ucType)
    {
    case VALVE_TYPE_GENERAL_PUMP:
    case VALVE_TYPE_RECTIFIER:    
    case VALVE_TYPE_EDI: 
        if (ucEnable)
        {
        }
        else
        {
        }
        break;
    case VALVE_TYPE_PRESSURE_REGULATING_PUMP:
        if (ucEnable)
        {
        }
        else
        {

        }
        
        break;
    }

    if (ucEnable)
    {
        Display.ausHoldRegs[HOLD_REGS_NAME_RPT1] |= 1 << ucChl;
    }
    else
    {
        Display.ausHoldRegs[HOLD_REGS_NAME_RPT1] &= ~(1 << ucChl);
    }
        
}

uint8_t ValveGetStatus(uint8_t ucChl)
{
    uint8_t ucValue = GetRelayLogicStatus(gValve.aValves[ucChl].ucRelayIndex);
    return gValve.aValves[ucChl].ucEnableLevel ? ucValue : !ucValue;
}

uint16_t ValveGetAllStatus(void)
{
    uint16_t usValue = 0;

    uint8_t ucLoop;

    for (ucLoop = 0; ucLoop < VALVE_TOTAL_NUMBER; ucLoop++)
    {
        uint8_t ucValue = GetRelayLogicStatus(gValve.aValves[ucLoop].ucRelayIndex);

        ucValue = gValve.aValves[ucLoop].ucEnableLevel ? ucValue : !ucValue;

        if (ucValue)
        {
            usValue |= 1 << ucLoop;
        }
    }

    return usValue;
}
