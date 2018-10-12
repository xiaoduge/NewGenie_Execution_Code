/**
  ******************************************************************************
  * @file    IAP/inc/common.h 
  * @author  MCD Application Team
  * @version V3.3.0
  * @date    10/15/2010
  * @brief   This file provides all the headers of the common functions.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _COMMON_H
#define _COMMON_H

#include "stm32f4xx.h"

/* Includes ------------------------------------------------------------------*/
#include "hal_types.h"


/*** MACROS  ***/
#define ERROR_SUCCESS                   0x00
#define ERROR_FAILURE                   0x01
#define ERROR_INVALIDPARAMETER          0x02
#define ERROR_INVALID_TASK              0x03
#define ERROR_MSG_BUFFER_NOT_AVAIL      0x04
#define ERROR_INVALID_MSG_POINTER       0x05
#define ERROR_INVALID_EVENT_ID          0x06
#define ERROR_INVALID_INTERRUPT_ID      0x07
#define ERROR_NO_TIMER_AVAIL            0x08
#define ERROR_NV_ITEM_UNINIT            0x09
#define ERROR_NV_OPER_FAILED            0x0A
#define ERROR_INVALID_MEM_SIZE          0x0B
#define ERROR_NV_BAD_ITEM_LEN           0x0C

/* NV Items for system : begin*/

#define NV_SERIAL_ID (0x10)

#define calcSerialNvID(Idx)               ( Idx + NV_SERIAL_ID)

#define NV_CANID_ID                       ( 0X20) // fixed , do not change

#define NV_DEVID_ID                       ( 0x21) // fixed , do not change

#define NV_DTYPE_ID                       ( 0x22) // fixed , do not change

#define NV_CANID_SIZE                     (2)     // 2 bytes

#define NV_DEVID_SIZE                     (4)     // 4 bytes

#define NV_DTYPE_SIZE                     (8)     // 2 bytes

/* NV Items  for system : End */


/* Exported types ------------------------------------------------------------*/
typedef  void (*pFunction)(void);



#if defined (STM32F40_41xxx)
 #define PAGE_SIZE                         (0x00004000)    /* 16 Kbyte */
 #define FLASH_SIZE                        (0x00100000)    /* 1  MBytes */
#else 
 #error "Please select first the STM32 device to be used (in stm32f4xx.h)"    
#endif

#define HAL_ELEC_ID_SIZE                   (96/32*2)   // 6 * 16bits            

#define HAL_FLASH_BEGIN                    (0x08000000)

#define BOOTLOAD_PAGE_CNT                  (1) 
#define BOOTLOAD_PAGE_BGN                  (0)
#define BOOTLOAD_PAGE_END                  (BOOTLOAD_PAGE_BGN+BOOTLOAD_PAGE_CNT-1) 

#define HAL_NV_PAGE_CNT                    (2) 
#define HAL_NV_PAGE_BGN                    (BOOTLOAD_PAGE_END+1) // YLF: WE GO HERE
#define HAL_NV_PAGE_END                    (HAL_NV_PAGE_BGN+HAL_NV_PAGE_CNT-1) // YLF: WE GO HERE

#define HAL_NV_SIZE                        (HAL_NV_PAGE_CNT*PAGE_SIZE)


#define HAL_FLASH_WORD_SIZE                4
#define HAL_FLASH_PAGE_SIZE                PAGE_SIZE

#define ApplicationAddress                 (HAL_FLASH_BEGIN + (HAL_NV_PAGE_CNT + BOOTLOAD_PAGE_CNT) * PAGE_SIZE)


/* Compute the FLASH upload image size */  

#define FLASH_IMAGE_SIZE                   (uint32) (FLASH_SIZE - ApplicationAddress)

/* ------------------------------------------------------------------------------------------------
 *                                         Constants
 * ------------------------------------------------------------------------------------------------
 */

#define HAL_SBL_IMG_BEG                (uint32)((ApplicationAddress - HAL_FLASH_BEGIN) / HAL_FLASH_WORD_SIZE)
#define HAL_SBL_IMG_CRC                (uint32)((ApplicationAddress - HAL_FLASH_BEGIN) / HAL_FLASH_WORD_SIZE)
#define HAL_SBL_IMG_END                (uint32)(FLASH_SIZE/HAL_FLASH_WORD_SIZE)


#define HAL_FLASH_ADR_SECTOR_0            ((u32)0x08000000) 	//扇区0起始地址, 16 Kbytes  
#define HAL_FLASH_ADR_SECTOR_1            ((u32)0x08004000) 	//扇区1起始地址, 16 Kbytes  
#define HAL_FLASH_ADR_SECTOR_2            ((u32)0x08008000) 	//扇区2起始地址, 16 Kbytes  
#define HAL_FLASH_ADR_SECTOR_3            ((u32)0x0800C000) 	//扇区3起始地址, 16 Kbytes  
#define HAL_FLASH_ADR_SECTOR_4            ((u32)0x08010000) 	//扇区4起始地址, 64 Kbytes  
#define HAL_FLASH_ADR_SECTOR_5            ((u32)0x08020000) 	//扇区5起始地址, 128 Kbytes  
#define HAL_FLASH_ADR_SECTOR_6            ((u32)0x08040000) 	//扇区6起始地址, 128 Kbytes  
#define HAL_FLASH_ADR_SECTOR_7            ((u32)0x08060000) 	//扇区7起始地址, 128 Kbytes  
#define HAL_FLASH_ADR_SECTOR_8            ((u32)0x08080000) 	//扇区8起始地址, 128 Kbytes  
#define HAL_FLASH_ADR_SECTOR_9            ((u32)0x080A0000) 	//扇区9起始地址, 128 Kbytes  
#define HAL_FLASH_ADR_SECTOR_10           ((u32)0x080C0000) 	//扇区10起始地址,128 Kbytes  
#define HAL_FLASH_ADR_SECTOR_11           ((u32)0x080E0000) 	//扇区11起始地址,128 Kbytes  

/* Base address of the Flash sectors Bank 2 */
#define HAL_FLASH_ADR_SECTOR_12           ((u32)0x08100000) /* Base @ of Sector 0, 16 Kbytes */
#define HAL_FLASH_ADR_SECTOR_13           ((u32)0x08104000) /* Base @ of Sector 1, 16 Kbytes */
#define HAL_FLASH_ADR_SECTOR_14           ((u32)0x08108000) /* Base @ of Sector 2, 16 Kbytes */
#define HAL_FLASH_ADR_SECTOR_15           ((u32)0x0810C000) /* Base @ of Sector 3, 16 Kbytes */
#define HAL_FLASH_ADR_SECTOR_16           ((u32)0x08110000) /* Base @ of Sector 4, 64 Kbytes */
#define HAL_FLASH_ADR_SECTOR_17           ((u32)0x08120000) /* Base @ of Sector 5, 128 Kbytes */
#define HAL_FLASH_ADR_SECTOR_18           ((u32)0x08140000) /* Base @ of Sector 6, 128 Kbytes */
#define HAL_FLASH_ADR_SECTOR_19           ((u32)0x08160000) /* Base @ of Sector 7, 128 Kbytes */
#define HAL_FLASH_ADR_SECTOR_20           ((u32)0x08180000) /* Base @ of Sector 8, 128 Kbytes  */
#define HAL_FLASH_ADR_SECTOR_21           ((u32)0x081A0000) /* Base @ of Sector 9, 128 Kbytes  */
#define HAL_FLASH_ADR_SECTOR_22           ((u32)0x081C0000) /* Base @ of Sector 10, 128 Kbytes */
#define HAL_FLASH_ADR_SECTOR_23           ((u32)0x081E0000) /* Base @ of Sector 11, 128 Kbytes */

// HalFlashRead page & offset for the CRC and CRC-shadow.
//#define SBL_CRC_PAGE            (HAL_SBL_IMG_CRC * HAL_FLASH_WORD_SIZE / HAL_FLASH_PAGE_SIZE)
//#define SBL_CRC_OFFSET          (HAL_SBL_IMG_CRC * HAL_FLASH_WORD_SIZE % HAL_FLASH_PAGE_SIZE)
// FOR CAN INTERFACE


#define CAN_ADDRESS_MASK (0X3FF)

#define CAN_SRC_ADDRESS_POS (10) // BIT0~BIT9 in indentifer

#define CAN_SRC_ADDRESS(indenfier)((indenfier >> CAN_SRC_ADDRESS_POS) & CAN_ADDRESS_MASK)

#define CAN_DST_ADDRESS_POS (0) // BIT0~BIT9 in indentifer

#define CAN_DST_ADDRESS(indenfier)((indenfier >> CAN_DST_ADDRESS_POS) & CAN_ADDRESS_MASK)

#define CAN_BROAD_CAST_ADDRESS (0X3FF)

#define CAN_BUILD_ADDRESS_IDENTIFIER(ulIdentifier,usSrcCanAdr,usDstCanAdr)(ulIdentifier=(usSrcCanAdr<<CAN_SRC_ADDRESS_POS)|(usDstCanAdr << CAN_DST_ADDRESS_POS)|CAN_EFF_FLAG)

#define CAN_ADDRESS_POS (0) // BIT0~BIT9 in indentifer

#define CAN_ADDRESS(indenfier)((indenfier >> CAN_ADDRESS_POS) & CAN_ADDRESS_MASK)

#define CAN_ADDRESS_LINKDIR_MASK (0X1UL<<10)

// for uplink packet
#define CAN_BUILD_HOST_IDENTIFIER(usIdentifier,usCanAdr)(usIdentifier=(usCanAdr & CAN_ADDRESS_MASK))

// for downlink packet
#define CAN_BUILD_CLIENT_IDENTIFIER(usIdentifier,usCanAdr)(usIdentifier= (1 << 10)|(usCanAdr & CAN_ADDRESS_MASK))

#define CAN_INVALID_ADDRESS (0XFFFF)

#define INVALID_CAN_ADDRESS(usAddress) ((usAddress & CAN_BROAD_CAST_ADDRESS) == CAN_BROAD_CAST_ADDRESS)


/* Exported functions ------------------------------------------------------- */

void HAL_SYSTEM_RESET(void);

void FLASH_DisableWriteProtectionPages(void);

void FLASH_Prepare(void);

uint32_t FLASH_PagesMask(__IO uint32_t Size);

uint8 checkRC(void);

void GetDeviceElecId(uint8 *id);

uint8 CmpDeviceElecId(uint8 *id);

uint16 HashDeviceElecId(void);

#endif  /* _COMMON_H */

/*******************(C)COPYRIGHT 2010 STMicroelectronics *****END OF FILE******/
