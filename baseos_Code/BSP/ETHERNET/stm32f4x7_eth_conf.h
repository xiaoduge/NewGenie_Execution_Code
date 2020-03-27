#ifndef __STM32F4x7_ETH_CONF_H
#define __STM32F4x7_ETH_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx.h"

#define USE_ENHANCED_DMA_DESCRIPTORS

//#define USE_Delay

#ifdef USE_Delay
  #include "main.h"                /* Header file where the Delay function prototype is exported */  
  #define _eth_delay_    Delay     /* User can provide more timing precise _eth_delay_ function */
#else
  #define _eth_delay_    ETH_Delay /* Default _eth_delay_ function with less precise timing */
#endif


#ifdef  CUSTOM_DRIVER_BUFFERS_CONFIG
/* Redefinition of the Ethernet driver buffers size and count */   
 #define ETH_RX_BUF_SIZE    ETH_MAX_PACKET_SIZE /* buffer size for receive */
 #define ETH_TX_BUF_SIZE    ETH_MAX_PACKET_SIZE /* buffer size for transmit */
 #define ETH_RXBUFNB        20                  /* 20 Rx buffers of size ETH_RX_BUF_SIZE */
 #define ETH_TXBUFNB        5                   /* 5  Tx buffers of size ETH_TX_BUF_SIZE */
#endif

/* PHY Reset delay */ 
#define PHY_RESET_DELAY    ((uint32_t)0x000FFFFF)
/* PHY Configuration delay */ 
#define PHY_CONFIG_DELAY   ((uint32_t)0x00FFFFFF)
/* Delay when writing to Ethernet registers*/
#define ETH_REG_WRITE_DELAY ((uint32_t)0x0000FFFF)

//PHY Status Reg
#ifdef EHT_PHY_AR8035
#define PHY_SR		((uint16_t)0x11) //  for AR8035
#define PHY_SPEED_STATUS            ((uint16_t)0xC000)  // for ar8035 SPEED
#define PHY_DUPLEX_STATUS           ((uint16_t)0x2000) //  for ar8035 Duplex
#define PHY_SPEED_10M               (0x0000)
#define PHY_SPEED_100M              (0x4000)
#else
// phy speed & duplex 
#define PHY_SR                      ((uint16_t)31) //  for LAN87320
#define PHY_SPEED_STATUS            ((uint16_t)0x0004)  // for LAN8720 SPEED
#define PHY_DUPLEX_STATUS           ((uint16_t)0x0010) //  for LAN8720 Duplex
#endif



#ifdef __cplusplus
}
#endif

#endif 


