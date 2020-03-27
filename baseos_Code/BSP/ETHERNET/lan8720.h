#ifndef __LAN8720_H
#define __LAN8720_H
#include "sys.h"
#include "stm32f4x7_eth.h"
//////////////////////////////////////////////////////////////////////////////////	   
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//LAN8720 ��ʼ�� 
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/8/15
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved
//********************************************************************************
//�޸�˵�� 
//��
////////////////////////////////////////////////////////////////////////////////// 	
		
 
extern uint8_t Rx_Buff[]; 							//��̫���ײ���������buffersָ�� 
extern uint8_t Tx_Buff[]; 							//��̫���ײ���������buffersָ��
extern ETH_DMADESCTypeDef  *DMATxDescToSet;			//DMA����������׷��ָ��
extern ETH_DMADESCTypeDef  *DMARxDescToGet; 		//DMA����������׷��ָ�� 
extern ETH_DMADESCTypeDef DMARxDscrTab[];			//��̫��DMA�������������ݽṹ��ָ��
extern ETH_DMADESCTypeDef DMATxDscrTab[];			//��̫��DMA�������������ݽṹ��ָ�� 

u8 PHY_Init(void);
u8 PHY_Get_Speed(void);
u8 PHY_Get_LinkState(void);
u8 PHY_Init_MAC(void);

u8 ETH_MACDMA_Config(void);
FrameTypeDef ETH_Rx_Packet(void);
u8 ETH_Tx_Packet(u16 FrameLength);
u32 ETH_GetCurrentTxBuffer(void);
#endif 

