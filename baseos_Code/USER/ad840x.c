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

#include "ad840x.h"

#include "task.h"

#include "ctype.h"

#include "hal_spi_driver.h"


static const uint8_t spi_cs[AD840X_MAX_NUM] = {
    STM32F103_GPA(15),     
    STM32F103_GPG(15),     
};


typedef struct
{
    uint8_t aucCache[1];
    struct spi_adpater *pSpiAdpter;
    
}AD840X_STRU;


static AD840X_STRU sAd840x[AD840X_MAX_NUM];


void AD840x_Write(uint8_t ucChl,uint8_t ucValue)
{
    stm32_gpio_set_value(spi_cs[ucChl],0); // ylf:  inactive low
    sAd840x[ucChl].pSpiAdpter->send(0);
    sAd840x[ucChl].pSpiAdpter->send(ucValue);
    sAd840x[ucChl].aucCache[0] = ucValue;
    stm32_gpio_set_value(spi_cs[ucChl],1); // ylf:  inactive low
}

uint8_t AD840x_BufferedRead(uint8_t ucChl)
{
    return sAd840x[ucChl].aucCache[0];
}


void AD840x_Init(void)
{
    int iLoop;

    struct spi_adpater *spi = hal_spi_Init(HAL_SPI0,SPI_CPOL_Low,SPI_CPHA_1Edge,SPI_BaudRatePrescaler_32);

    memset(sAd840x,0,sizeof(sAd840x));

    for (iLoop = 0; iLoop < AD840X_MAX_NUM; iLoop++)
    {
        stm32_gpio_cfgpin(spi_cs[iLoop],MAKE_PIN_CFG(GPIO_PuPd_NOPULL,GPIO_OType_PP,GPIO_Speed_50MHz,GPIO_Mode_OUT)); 
        
        stm32_gpio_set_value(spi_cs[iLoop],1); // ylf:  inactive low

        sAd840x[iLoop].pSpiAdpter = spi;
    }

    // Init output zero
    
    for (iLoop = 0; iLoop < AD840X_MAX_NUM; iLoop++)
    {
        AD840x_Write(iLoop,0);    
    }

}

