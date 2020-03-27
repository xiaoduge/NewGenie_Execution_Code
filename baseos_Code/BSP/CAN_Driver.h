#ifndef _CAN_DRIVER_H_
#define _CAN_DRIVER_H_

#include "CanDriverItf.h"

void STM_EVAL_CANInit(CAN_InitTypeDef* CAN_InitStruct);
uint8_t CANSendMsgNoWait(uint16_t Identifier, uint8_t* Msg, uint8_t MsgSize );
uint8_t STM32_CANSendMsgNoWait(CAN_FRAME *frame );

void STM_EVAL_CANInit_ext(int iChl,CAN_InitTypeDef* CAN_InitStruct);
uint8_t CANSendMsgNoWait_ext(int iChl,uint16_t Identifier, uint8_t* Msg, uint8_t MsgSize );
uint8_t STM32_CANSendMsgNoWait_ext(int iChl,CAN_FRAME *frame );

#endif
