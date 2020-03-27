#include    <ucos_ii.h>

#include    <string.h>
#include    <stdarg.h>

#include "stm32_eval.h"

#include "UartCmd.h"

#include "task.h"

#include "common.h"

#include "sapp_ex.h"

#include "serial_driver.h"

#define UARTITF_SERIAL_PORT (SERIAL_PORT0)
#define UARTITF_SERIAL_UART (USART1)
#define UARTITF_SERIAL_IRQ  (USART1_IRQn)

static SAPP_PARSE_STRU sAppParse;


void SerialCmd_Callback(uint8 port, uint8 event)
{
  
  if (event & (HAL_UART_RX_FULL|HAL_UART_RX_ABOUT_FULL|HAL_UART_RX_TIMEOUT))
  {
      SHZNAPP_SerialParse(0,&sAppParse);
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
        pBuffer[RPC_POS_LEN]  = length;
        pBuffer[RPC_POS_CMD0] = RPC_SYS_APP;
        pBuffer[RPC_POS_CMD1] = ucCmd;
    
        (void)SHZNAPP_SerialResp(0,RPC_UART_SOF,pBuffer);
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


void SerialCmd_config_cb(uint8_t ucPort)
{
    memset(&Serial[ucPort],0,offsetof(SERIAL_STRU,ucDriverType));

    Serial[ucPort].ucDriverType = MSG_DRIVER;
    Serial[ucPort].ucPortType   = RS232;
    Serial[ucPort].ucPortCtrl   = 0; // DONT CARE FOR RS232
    Serial[ucPort].iIrq         = UARTITF_SERIAL_IRQ;
    Serial[ucPort].iComIdx      = UARTITF_SERIAL_PORT;
    Serial[ucPort].ucPortMode   = UART_CMD;    
    Serial[ucPort].UsartDef     = UARTITF_SERIAL_UART;
        
    Serial[ucPort].SerialConfig.BaundRate   = 115200; // FXIED setting
    Serial[ucPort].SerialConfig.ucDataBits  = BAUD_DATA_8BITS; // FXIED setting
    Serial[ucPort].SerialConfig.ucStopBits  = BAUD_STOP_1BITS; // FXIED setting
    Serial[ucPort].SerialConfig.ucParity    = BAUD_PARITY_NO; // FXIED setting
    Serial[ucPort].SerialConfig.ucIdleCheckThd = 20; // FXIED setting

}

void SerialCmd_RcvData(UINT8 ucData)
{
    UartCmdStru.UART_Rcvbuff[UartCmdStru.rcvfront++] = ucData;

    if (UartCmdStru.cb) UartCmdStru.cb(0,HAL_UART_RX_TIMEOUT);

}

UINT8  SerialCmd_FillSndBuf(UINT8 *pData,UINT16 usLength)
{
    return Serial_FillSndBuf(UARTITF_SERIAL_PORT,pData,usLength);
}

void SerialCmd_Init(void)
{
    Serial[UARTITF_SERIAL_PORT].sccb = SerialCmd_config_cb;
	
    Serial[UARTITF_SERIAL_PORT].mcb  = NULL;

    Serial[UARTITF_SERIAL_PORT].ccb  = SerialCmd_RcvData;

	
    SerialInitPort(UARTITF_SERIAL_PORT);

    memset(&UartCmdStru,0,sizeof(UART_CMD_STRU));

    UartCmdStru.cb = SerialCmd_Callback;
    
    SHZNAPP_ProtolInit();
}


void UartCmd_SetFilter(serial_filter_cb fcb)
{
    Serial[UARTITF_SERIAL_PORT].fcb = fcb;

    /* override default mode */
    //UartCmdStru.cb = NULL;

    Serial[UARTITF_SERIAL_PORT].ucPortMode   = UART_STDIO;
}

