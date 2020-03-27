#ifndef _ADC_DRIVER_H_
#define _ADC_DRIVER_H_

void ADC_DMA_Configuration ( uint32_t dstAddr,uint32_t dstSize, uint32_t chlNum,uint8_t ADC_SampleTime );
void ADC_DMA_Configuration_ex ( uint32_t dstAddr,uint32_t dstSize, uint32_t chlStart ,uint32_t chlNum, uint8_t ADC_SampleTime);

#endif
