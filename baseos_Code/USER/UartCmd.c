#include    <ucos_ii.h>

#include    <string.h>
#include    <stdarg.h>

#include "stm32_eval.h"

#include "UartCmd.h"

#include "task.h"

#include "common.h"

#include "sapp.h"

#include "serial_driver.h"


static int UcLogLevel = VOS_LOG_ERROR;


void UartCmdCallback(uint8 port, uint8 event)
{
  
  if (event & (HAL_UART_RX_FULL|HAL_UART_RX_ABOUT_FULL|HAL_UART_RX_TIMEOUT))
  {
      SHZNAPP_SerialParse(0);
  }

  if (event & HAL_UART_TX_EMPTY)
  {
  
      if (sappFlags & (1 << SAPP_CMD_RESET))
      {
#if USB_SUPPORT > 0
          USBCDC_disconnect(); 
#endif
          HAL_SYSTEM_RESET();
      }
      else if (sappFlags & (1 << SAPP_CMD_SYS_INIT))
      {
#if USB_SUPPORT > 0
          USBCDC_disconnect(); 
#endif
          HAL_SYSTEM_RESET();
      }
  }
  
}


UART_CMD_STRU UartCmdStru;

void UartCmdInit(void)
{
#ifdef USE_USART1_STDIO

    NVIC_InitTypeDef NVIC_InitStructure;

    USART_InitTypeDef USART_InitStructure;

    USART_InitStructure.USART_BaudRate   = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits   = USART_StopBits_1;
    USART_InitStructure.USART_Parity     = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;
    
    STM_EVAL_COMInit(COM1, &USART_InitStructure);

    /* Enable the USARTx Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // others are initialized in Printf
    
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

#endif
    // USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
    memset(&UartCmdStru,0,sizeof(UART_CMD_STRU));

    UartCmdStru.cb = UartCmdCallback;

	SHZNAPP_SerialInit();

}

void UartCmdPutData(UINT8 ucData)
{
    UartCmdStru.UART_Rcvbuff[UartCmdStru.rcvfront++] = ucData;

    UartCmdStru.cb(0,HAL_UART_RX_TIMEOUT);
}

void UartCmdSendData(UINT8 ucData)
{
#ifdef USE_USART1_STDIO
    USART_SendData(USART1, ucData);
    
    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
    {
    }
#endif    
}

void UartCmdSendMsg(UINT8 *pData,UINT8 ucLength)
{
    UINT8 ucLoop;
    for (ucLoop = 0; ucLoop < ucLength; ucLoop++)
    {
        UartCmdSendData(pData[ucLoop]);
    }

    UartCmdStru.cb(0,HAL_UART_TX_EMPTY);
}

void UartCmdSerialWrite(uint8 ucCmd,uint8 *pBuffer, uint16 length)
{
    if (length <= SAPP_MAX_SIZE)
    {
        sbTxBuf[RPC_POS_LEN]  = length;
        sbTxBuf[RPC_POS_CMD0] = RPC_SYS_APP;
        sbTxBuf[RPC_POS_CMD1] = ucCmd;
    
        memcpy(&sbTxBuf[RPC_POS_DAT0],pBuffer, length);
    
        (void)SHZNAPP_SerialResp(sappItfPort);
    }
}

void UartCmdRs485Report(UINT8 ucPort ,UINT8 *pData,UINT8 ucLength)
{
    UartCmdSerialWrite(SAPP_CMD_USER + ucPort,pData,ucLength);

}


uint16_t HalUARTWrite(uint8_t port, uint8_t *pBuffer, uint16_t length)
{   
    switch(port)
    {
    case 0:
        UartCmdSendMsg(pBuffer,length);
        break;
#if USB_SUPPORT > 0
    case 1:
        USBCDC_senddata(pBuffer,length);
        break;
#endif		
    default:
        return 0;
    }

    return length;
}

uint16_t HalUARTRead(uint8_t port, uint8_t *pBuffer, uint16_t length)
{
    switch(port)
    {
    case 0:
        if ( UartCmdStru.rcvfront != UartCmdStru.rcvrear)
        {
          *pBuffer = UartCmdStru.UART_Rcvbuff[UartCmdStru.rcvrear];
          UartCmdStru.rcvrear = (UartCmdStru.rcvrear + 1 ) % MAX_RCV_BUFF_LEN;
          return 1;
        }
        return 0;
#if USB_SUPPORT > 0		
    case 1:
        return USBCDC_recvdata(pBuffer,length);
#endif		
    default:
        return 0;
    }


}

