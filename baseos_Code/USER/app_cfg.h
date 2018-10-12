/**
  ******************************************************************************
  * @file    app_cfg.h 
  * @author  STM32 Team
  * @version V1.0.0
  * @date    10/10/2014
  * @brief   Interface for app entry.
  ******************************************************************************
  *          DO NOT MODIFY ANY OF THE FOLLOWING CODE EXCEPT UNDER PERMISSION OF ITS PROPRIETOR
  * @copy
  *
  *
  * <h2><center>&copy; COPYRIGHT 2012 Shanghai ZhenNeng Corporation</center></h2>
  */ 

#ifndef  __APP_CFG_H__
#define  __APP_CFG_H__

#include "stm32f4xx.h"


/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/
#define STACK_ALIGN __align(8)


#define  APP_TASK_MAIN_PRIO                   3  // DO NOT CHANGE!!

#define  APP_TASK_NUMBER                     (3) // DO NOT CHANGE!! AT MOST 3 APP task

#define  APP_TASK_ROOT_PRIO                  (APP_TASK_MAIN_PRIO + APP_TASK_NUMBER)

#define  APP_TASK_NETITF_PRO                 (APP_TASK_MAIN_PRIO + 1)

#define  OS_TASK_TMR_PRIO                    (OS_LOWEST_PRIO - 2)

#define  PID_NETITF_TASK                     (PID_MAIN_TASK + APP_TASK_NETITF_PRO - APP_TASK_MAIN_PRIO)

/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*********************************************************************************************************
*/


#define  APP_TASK_MAIN_STK_SIZE             2048
#define  APP_TASK_ROOT_STK_SIZE             512

#define  APP_TASK_PROBE_STR_STK_SIZE         64

#define  OS_PROBE_TASK_STK_SIZE              64

#define  APP_TIMER_TICK                        (10)             // in ms

#define CHECK_KEY_DURATION                    (10/APP_TIMER_TICK) //10ms ->  in check tick
#define SECOND_DRUATION                       (1000/APP_TIMER_TICK) // 1000MS -> in check tick
#define LIGHT_ADJUST_DURATION                 (100/APP_TIMER_TICK) // 100MS -> in check tick


#define	TIMER_CHECK_SECOND                     0 

#define	TIMER_CHECK_KEY                        1 

#define	TIMER_CHECK_BEEP                       2 

#define	TIMER_CHECK_ALARM                      3 

#define	TIMER_CHECK_HBT                        4 

#define TIMER_USER_BASE   (TIMER_CHECK_HBT + 1) // id = 3

#define TIMER_CCB_BEGIN   (TIMER_USER_BASE)

#define TIMER_CCB_END (TIMER_CCB_BEGIN + MAX_CAN_CCB_NUMBER) //4..11


#define TIMER_CCB_HEART_BEAT (TIMER_CCB_END) // id = 12




//#define DEBUG
enum KEYS_ENUM
{
    KEY_CODE_UP = 0,
    KEY_CODE_FN   ,
    KEY_CODE_NUM  ,   
};



typedef enum
{
   SELF_MSG_CODE_INIT = 0,
   SELF_MSG_CODE_DELAY,

   // user add here
   SELF_MSG_CODE_USER_BEGIN = 0X10,
   SELF_MSG_CODE_USER_DISP,
   SELF_MSG_CODE_USER_CANCMD,
   SELF_MSG_CODE_USER_QM,
   SELF_MSG_CODE_NUM,
}SELF_MSG_CODE_ENUM;

//#define DEBUG



#define IWDG_SUPPORT 1 

#define RTC_SUPPORT  0

#define ALARM_SUPPORT 1

#define BEEP_SUPPORT 1

#define RUN_SUPPORT 1

#define ADC_SUPPORT 1

#define SERIAL_SUPPORT 1

#define CAN_SUPPORT 1

#define ETH_SUPPORT  0


#endif
