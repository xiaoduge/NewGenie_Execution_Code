/**
  ******************************************************************************
  * @file    hal_flash.h
  * @author  STM32 Team
  * @version V1.0.0
  * @date    10/10/2014
  * @brief   HAL Interface for FLASH access.
  ******************************************************************************
  *          DO NOT MODIFY ANY OF THE FOLLOWING CODE EXCEPT UNDER PERMISSION OF ITS PROPRIETOR
  * @copy
  *
  *
  * <h2><center>&copy; COPYRIGHT 2012 Shanghai ZhenNeng Corporation</center></h2>
  */ 

#ifndef HAL_FLASH_H
#define HAL_FLASH_H

#ifdef __cplusplus
extern "C"
{
#endif

/* ------------------------------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------------------------
 */

#include "common.h"
#include "hal_types.h"

#define HAL_FLASH_INVALID_PAGE_NO (0XFFFF)

/**************************************************************************************************
 * @fn          HalFlashRead
 *
 * @brief       This function reads 'cnt' bytes from the internal flash.
 *
 * input parameters
 *
 * @param       pg - Valid HAL flash page number (ie < 256).
 * @param       offset - Valid offset into the page (so < HAL_NV_PAGE_SIZE and byte-aligned is ok).
 * @param       buf - Valid buffer space at least as big as the 'cnt' parameter.
 * @param       cnt - Valid number of bytes to read: a read cannot cross into the next 32KB bank.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 **************************************************************************************************
 */
void HalFlashRead(uint16 pg, uint32 offset, uint8 *buf, uint32 cnt);

/**************************************************************************************************
 * @fn          HalFlashWrite
 *
 * @brief       This function reads 'cnt' bytes from the internal flash.
 *
 * input parameters
 *
 * @param       addr - Valid HAL flash write address: actual addr / 4 and quad-aligned.
 * @param       buf - Valid buffer space at least as big as the 'cnt' parameter.
 * @param       cnt - Valid number of bytes to write: a write cannot cross into the next 32KB bank.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 **************************************************************************************************
 */
uint8 HalFlashWrite(uint32 addr, uint8 *buf, uint16 cnt);


/**************************************************************************************************
 * @fn          HalFlashWritePage
 *
 * @brief       This function writes 'cnt' bytes to the internal flash.
 *
 * input parameters
 *
 * @param       pg - A valid flash page number.
 * @param       offset - A valid offset into the page,must be multiplex of 4.
 * @param       buf - A valid buffer space at least as big as the 'cnt' parameter.
 * @param       cnt - A valid number of bytes to write,must be multiplex of 4.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 **************************************************************************************************
 */
uint8 HalFlashWritePage(uint16 pg, uint32 offset, uint8 *buf, uint32 cnt);

/**************************************************************************************************
 * @fn          HalFlashErase
 *
 * @brief       This function erases  one sector of the internal flash.
 *
 * input parameters
 *
 * @param       pg - Valid HAL flash page number (ie < 128) to erase.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 **************************************************************************************************
 */
uint8 HalFlashErase(uint16 sector);

/**************************************************************************************************
 * @fn          HalFlashGetSector
 *
 * @brief       mapping between address and sector
 *
 * input parameters
 *
 * @param       pg - A valid flash address to map.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 **************************************************************************************************
 */
uint16 HalFlashGetSector(uint32 ulAddr);

/**************************************************************************************************
 * @fn          HalFlashGetSize
 *
 * @brief       Mapping between page and addresss.
 *
 * input parameters
 *
 * @param       pg - A valid flash page number to map.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 **************************************************************************************************
 */
uint32 HalFlashGetSize(uint16 ucPage);

/**************************************************************************************************
 * @fn          HalFlashCheckPage
 *
 * @brief       check if Address is starting address of any valid page 
 *
 * input parameters
 *
 * @param       ulAddr - flash address .
 *
 * output parameters
 *
 * None.
 *
 * @return      sector number , otherwise OXFFFF.
 **************************************************************************************************
 */
uint16 HalFlashCheckPage(uint32 ulAddr);

/**************************************************************************************************
 * @fn          HalFlashErasePage
 *
 * @brief       This function erases the specified page of the internal flash.
 *
 * input parameters
 *
 * @param       pg - A valid flash page number to erase.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 **************************************************************************************************
 */
uint8 HalFlashErasePage(uint16 usPage);

/**************************************************************************************************
 * @fn          HalGetPageAddress
 *
 * @brief       check if Address is starting address of a page 
 *
 * input parameters
 *
 * @param       ucPage - flash page .
 *
 * output parameters
 *
 * None.
 *
 * @return      page address.
 **************************************************************************************************
 */
uint32_t HalGetPageAddress(uint16 ucPage);

#ifdef __cplusplus
};
#endif

#endif

/**************************************************************************************************
*/
