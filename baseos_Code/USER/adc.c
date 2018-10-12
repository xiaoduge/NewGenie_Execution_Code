#include "stm32_eval.h"

#include "Timer_Driver.h"

#include "Adc_Driver.h"

#include "memory.h"
#include "msg.h"

#include "adc.h"

#include "valve.h"

#include "cminterface.h"

#include "Display.h"


#define ADC_REFER_VOLT (3440)

#define CM_MAX_NUMBER (13)

#define CM_MAX_INPUT_NUMBER (CM_MAX_NUMBER)

#define CM_INPUT_MASK       ((1 << CM_MAX_INPUT_NUMBER) - 1)

#define ADC_Oversampling_NUM  (16)       /*pow(4, ADC_Additional_Bits)*/

#define ADC_Additional_Bits 4   // should satisfy : 2^ADC_Additional_Bits =  ADC_Oversampling_NUM

#define ADC_Oversampling_Factor  (CM_MAX_NUMBER*ADC_Oversampling_NUM)       /*pow(4, ADC_Additional_Bits)*/

extern u16 ADC_ConvertedValue[ADC_Oversampling_NUM][CM_MAX_INPUT_NUMBER];

u16 ADC_ConvertedValue[ADC_Oversampling_NUM][CM_MAX_NUMBER];

u32 ADC_Result[CM_MAX_NUMBER];


/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Enable DMA channel1 IRQ Channel -----------------------------------------*/
  NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;//DMA1_Stream0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0xf;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* ADC中断配置 */
  NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQn;           //嵌套通道为ADC_IRQn
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0xf; //抢占优先级为 1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;    //响应优先级为 2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;     //通道中断使能
  NVIC_Init(&NVIC_InitStructure);  
  
}

int TIM2_Handler(int Tim,int event,void *para)
{
    DMA_Stream_TypeDef *DmaStream = DMA2_Stream0;

    /* Update the Number of DMA transfer */ 
    DmaStream->NDTR = ADC_Oversampling_Factor;
    
    /* Update the Destination Memory of the DMA pointer */ 
    DmaStream->M0AR = (u32)ADC_ConvertedValue;  
    
    DmaStream->CR |= DMA_SxCR_EN;

    ADC_Cmd(ADC1, ENABLE);
    
    ADC_SoftwareStartConv(ADC1);
	
	return 0;

}

/*******************************************************************************
* Function Name  : TIM2_Configuration
* Description    : Configures the TIM2 to generate an interrupt after each 
*                     sampling period                  
* Input          : The required oversampling period in us
* Output         : None
* Return         : None
*******************************************************************************/
void TIM2_Configuration ( u32 Sampling_Period )
{  
    int freq = 1000000/Sampling_Period;

    TIM_Init_General(TIMER1,freq);

    TIM_InstallHandler(TIMER1,TIM_IT_Update,TIM2_Handler,NULL);
}  


/*******************************************************************************
* Function Name  : CurrentMeas_Init
* Description    : Configures the Init current measurement 
* Input          : 
*       @param Sampling_Period: sampling period ,unit 100us
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_Meas_Init(u32 Sampling_Period )
{
  /* Peripherals InitStructure define -----------------------------------------*/
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOA, ENABLE);//
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //

  GPIO_StructInit(&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_InitStructure.GPIO_Speed = (GPIOSpeed_TypeDef)0;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1 |GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_InitStructure.GPIO_Speed = (GPIOSpeed_TypeDef)0;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_InitStructure.GPIO_Speed = (GPIOSpeed_TypeDef)0;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1 |GPIO_Pin_2 ;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  NVIC_Configuration();

  ADC_DMA_Configuration((u32)ADC_ConvertedValue,ADC_Oversampling_Factor,CM_MAX_NUMBER,ADC_SampleTime_15Cycles);
  
  ADC_SoftwareStartConv(ADC1);
  
  TIM2_Configuration(Sampling_Period);
    
}

/*******************************************************************************
* Function Name  : u32 Oversampling_GetConversion
* Description    : Gives the ADC oversampled conversion result  
* Input          : None 
* Output         : Oversampled Result
* Return         : None
*******************************************************************************/
void Oversampling_GetConversion ( void )
{
  u32 index = 0;
  u32 chloop = 0;

  for (index = 0; index < CM_MAX_NUMBER; index++)
  {
      ADC_Result[index] = 0;
  }
  
  for( index = 0; index < ADC_Oversampling_NUM ; index++)
  {
    for (chloop = 0; chloop < CM_MAX_NUMBER;chloop++)
    {
        ADC_Result[chloop] += ADC_ConvertedValue[index][chloop];
    }
  }

  for (index = 0; index < CM_MAX_NUMBER; index++)
  {
      ADC_Result[index] = ADC_Result[index] >> ADC_Additional_Bits;
  }
    
}  

/*******************************************************************************
* Function Name  : GetAdcData
* Description    : get the current measurement 
* Input          : 
*       @param ucChl: channel number ,in range [0~2]
* Output         : None
* Return         : None
*******************************************************************************/
uint32_t GetAdcData(uint8_t ucChl)
{
    if (ucChl >= CM_MAX_NUMBER)
    {
        return 0;
    }
    return ADC_Result[ucChl] ;
}


/*******************************************************************************
* Function Name  : PidAdcProcess
* Description    : PID proc for adc driver module 
* Input          : 
*       @param pMsg: message pointer
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 PidAdcProcess(Message *pMsg)
{

    uint8_t ucLoop;
    VALVE_NODE_STRU     *pValveNode;

    for (ucLoop = 0; ucLoop < VALVE_TOTAL_NUMBER; ucLoop++)
    {
       pValveNode = &gValve.aValves[ucLoop];
       
        if (pValveNode->ucAdcChl4I < CM_MAX_NUMBER)
        {
            if (pValveNode->ucAdcData4IPtr < APP_EXE_MAX_INPUT_REGISTERS)
            {
                Display.ausInputRegs[pValveNode->ucAdcData4IPtr] = GetAdcData(pValveNode->ucAdcChl4I);
            }
        }
        
        if (pValveNode->ucAdcChl4V < CM_MAX_NUMBER)
        {
            if (pValveNode->ucAdcData4VPtr < APP_EXE_MAX_INPUT_REGISTERS)
            {
                Display.ausInputRegs[pValveNode->ucAdcData4VPtr] = GetAdcData(pValveNode->ucAdcChl4V);
            }        
        }
    }

    for (ucLoop = 0; ucLoop < APP_EXE_INPUT_REG_LOOP_NUM; ucLoop++)
    {
        Display.ausInputRegs[APP_EXE_INPUT_REG_LOOP_OFFSET + ucLoop] = GetAdcData(ucLoop);
     }  
    
     return 0;
}


