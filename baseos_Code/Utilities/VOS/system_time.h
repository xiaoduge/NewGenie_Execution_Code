#ifndef _SYS_SHZN_TIME_H_
#define _SYS_SHZN_TIME_H_

/**
  ******************************************************************************
  * @file    system_time.h 
  * @author  STM32 Team
  * @version V1.0.0
  * @date    10/10/2014
  * @brief   Interface for timer access.
  ******************************************************************************
  *          DO NOT MODIFY ANY OF THE FOLLOWING CODE EXCEPT UNDER PERMISSION OF ITS PROPRIETOR
  * @copy
  *
  *
  * <h2><center>&copy; COPYRIGHT 2012 Shanghai ZhenNeng Corporation</center></h2>
  */ 

#include <ucos_ii.h>
#include "DtypeStm32.h"

#define SYS_TIMER_UNIT ((1000 / OS_TICKS_PER_SEC  )) //  ms per tick

enum
{
  SYS_TIMER_ONE_SHOT = 0,
  SYS_TIMER_PERIOD,
  SYS_TIMER_NUM,
};


typedef void (* system_timeout_handler)(void *arg);

typedef struct  system_timeo{
  struct system_timeo *next;
  unsigned int time;
  system_timeout_handler h;
  void *arg;
  unsigned int type; // refer SYS_TIMER_ONE_SHOT
  unsigned int period;
}system_timeo;

void system_inittime(void);
void system_untimeout(system_timeo *to);
void system_timeout(unsigned int msecs, int type,unsigned int period,system_timeout_handler h,void *arg,system_timeo *timeout);
void system_time_proc(void);
int system_time_ticking(system_timeo *to);

#endif
