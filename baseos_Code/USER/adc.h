#ifndef _ADC_MEASUREMENT_H_
#define _ADC_MEASUREMENT_H_

UINT8 PidAdcProcess(Message *pMsg);

void  Oversampling_GetConversion ( void );

void ADC_Meas_Init(u32 Sampling_Period );

uint32_t GetAdcData(uint8_t ucChl);

#endif


