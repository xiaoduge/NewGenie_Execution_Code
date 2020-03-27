#include    <stdio.h>
#include    <string.h>
#include    <ctype.h>
#include    <stdlib.h>
#include    <stdarg.h>

#include    <ucos_ii.h>

#include    <cpu.h>
#include    <lib_def.h>
#include    <lib_mem.h>
#include    <lib_str.h>

#include    <stdint.h>

#include    <app_cfg.h>
#include    <bsp.h>

#include "stm32_eval.h"

#include "memory.h"
#include "task.h"
#include "timer.h"

#include "Beep.h"

#include "system_time.h"

#include "keyboard.h"

#include "UartCmd.h"

#include "sapp_ex.h"

#if (ADC_SUPPORT > 0)
#include "adc.h"
#endif

#if (IWDG_SUPPORT > 0)
#include "IWDG_Driver.h"
#endif

#if (SERIAL_SUPPORT > 0)
#include "serial.h"
#endif

#if (RTC_SUPPORT > 0)
#include "RTC_Driver.h"
#endif

#include "ad840x.h"
#include "adc.h"

#include "CanCmd.h"

#include "config.h"

#include "qmi.h"

#include "relay.h"

#include "valve.h"

#include "display.h"
#include "mfddriver.h"
#include "leakage.h"



uint32_t gulSecond;

#if (RTC_SUPPORT > 0)
tm gBootTimer;
#endif


STACK_ALIGN static OS_STK AppTaskMainStk[APP_TASK_MAIN_STK_SIZE];
STACK_ALIGN static OS_STK AppTaskRootStk[APP_TASK_ROOT_STK_SIZE];

void App_TaskIdleHook      (void)
{
    #if (IWDG_SUPPORT > 0)
    IWDG_Feed();
    #endif
    
}


void App_TimeTickHook(void)
{

    TimerProc();

    system_time_proc();

    LeakageTickTask();

#if (SERIAL_SUPPORT > 0)
    {
        UINT8 ucLoop;

        for (ucLoop = 0; ucLoop < SERIAL_MAX_NUMBER; ucLoop++)
        {
            Serial_CheckRcvIdleTime(ucLoop);
        }
    }
#endif    
}


void InitKeys(void)
{
#if 0
    GPIO_KEY key;

    key.gpio = STM32F103_GPC(6);
    key.key = KEY_CODE_UP;
    RegisterKey(&key);
#endif    
}




void Main_SerialItfFilter(uint8_t ucPort,uint8_t ucData)
{
    /* command terminated by '\n' */
    if ('\n' == ucData)
    {
       Serial_TriggerIdleEvent(ucPort);    
    }
}

void  MainInit (void)
{
//   int ret;
   RCC_ClearFlag();

   VOS_SetLogLevel(VOS_LOG_INFO);

   stm32_gpio_init();

   SerialCmd_Init();

   UartCmd_SetFilter(Main_SerialItfFilter);

   osal_snv_init();
   
   Config_Init();

#if (OS_TASK_STAT_EN > 0)
   OSStatInit();                                               /* Determine CPU capacity                                   */
#endif

   // DON'T remove following line!!
   mfd_Init(STM32F103_GPB(11),0);

   InitRelays();  

   Ioctrl_input_init();

   BeepInit();
   
   AlarmInit();
   
   RunInit();


#if (RTC_SUPPORT > 0)
   RTC_Config();
   RTC_Get();
#endif

   KeyboardInit();

   InitKeys();

   ValveInit();

   AD840x_Init();


#if (ADC_SUPPORT > 0)
   ADC_Meas_Init(1000000);
#endif

    CanCmdInit();

    appQmiInit();

    Disp_Init();

#if (IWDG_SUPPORT > 0)
    IWDG_Init();
#endif

    // ylf: start timers
    AddTimer(TIMER_CHECK_SECOND,OS_TMR_OPT_PERIODIC,OS_TICKS_PER_SEC,0xffff);

    AddTimer(TIMER_CHECK_KEY,OS_TMR_OPT_PERIODIC,(OS_TICKS_PER_SEC*10)/1000,0xffff); // 10ms

}


UINT8 PidSelfProcess(Message *pMsg)
{
    switch(pMsg->msgHead.nMsgType)
    {
    case SELF_MSG_CODE_INIT:
        MainInit();
        break;
    case SELF_MSG_CODE_DELAY:
        break;
      
    case SELF_MSG_CODE_USER_QM:
    
        appQmi_msg_Handler(pMsg);
        break;
    case SELF_MSG_CODE_USER_CANCMD:
        CanCmd_msg_Handler(pMsg);
        break;
    }
    return 0;
}

UINT8 PidKeyboardProcess(Message *pMsg)
{
    //(pMsg->msgHead.AddData) // key value
    return 0;
}


void SecondTimer(void)
{
    gulSecond++;
    RunToggle();

    CanCcbSecondTask();

    Disp_SecondTask();

}


UINT8 PidTimerProcess(Message *pMsg)
{
    UINT8 ucTimer = pMsg->msgHead.nMsgType;

    switch(ucTimer) /*MsgLen is userid */
    {
    case TIMER_CHECK_SECOND:
        SecondTimer();
        break;
    case TIMER_CHECK_ALARM:
        Alarm(0);
        break;
    case TIMER_CHECK_BEEP:
        break;
    case TIMER_CHECK_KEY:
        InputStateReport();
        break;
    case TIMER_CHECK_HBT:
        break;
    default:
        if (ucTimer >= TIMER_CCB_BEGIN && ucTimer < TIMER_CCB_END)
        {
            CanCcbTimerProc(ucTimer,pMsg->msgHead.AddData);
        }
        break;
    }
    
    return 0;
}

UINT8 MainSappProc(Message *pMsg)
{
    UINT8 ucRet = TRUE;
    UINT8 aucBuffer[256];
    uint8_t *sbRxBuf = (uint8_t *)pMsg->data;
    uint8_t *sbTxBuf = aucBuffer + 1;

    MainAlarmWithDuration(1);               
    
    switch(sbRxBuf[RPC_POS_CMD0])
    {
    case RPC_SYS_APP:
        ucRet = SHZNAPP_SerialAppProc(sbRxBuf,sbTxBuf);
        break;
    case RPC_SYS_BOOT:
        ucRet = SHZNAPP_SerialBootProc(sbRxBuf,sbTxBuf);
        break;
    default:
        ucRet = SHZNAPP_SerialUnknowProc(sbTxBuf);
        break;
    }
    if (ucRet )
    {
        (void)SHZNAPP_SerialResp(pMsg->msgHead.MsgSeq,RPC_UART_SOF,sbTxBuf);  // Send the SB response setup in the sbRxBuf passed to sblProc().
    }

    return 0;
}



void Main_Cmd( char *cmd , int len)
{
    // intentional blank
}


UINT8 Msg_proc(Message *pMsg)
{
    switch(pMsg->msgHead.nSendPid)
    {
    case PID_TIMER:
        return PidTimerProcess(pMsg);
    case PID_SELF:
        return PidSelfProcess(pMsg);
    case PID_KB:
        return PidKeyboardProcess(pMsg);
    case PID_SAPP:
        return MainSappProc(pMsg);   
#if (ADC_SUPPORT > 0)
    case PID_ADC:
        return PidAdcProcess(pMsg);
#endif 
#if (SERIAL_SUPPORT > 0)
    case PID_RS485:
        if (0 == pMsg->msgHead.nMsgType)
        {
            Main_Cmd(pMsg->data,pMsg->msgHead.nMsgLen);
        }
        else 
        {
            return PidSerialProcess(pMsg);
        }
        break;
#endif        
#if (CAN_SUPPORT > 0)
    case PID_CAN:
        return PidCanProcess(pMsg);
#endif    

    default:
        break;
    }
      
    return 0;
}


void Main_Entry(Message *msg)
{
    Msg_proc(msg);

}

void MainKickoff(void)
{
    Message *Msg;
    
    Msg = MessageAlloc(PID_SELF,0);
    
    if (Msg)
    {
        Msg->msgHead.nMsgType = SELF_MSG_CODE_INIT;
        MessageSend(Msg);
    }
}


static  void  AppTaskRoot (void *p_arg)
{
 
 (void)p_arg;


  VOS_CreateTask(APP_TASK_MAIN_PRIO,(OS_STK *)&AppTaskMainStk[0],APP_TASK_MAIN_STK_SIZE,Main_Entry);

#if (OS_TASK_NAME_SIZE > 13)
  {
     INT8U err;
     OSTaskNameSet(APP_TASK_MAIN_PRIO, "Main-Task", &err);
  }
#endif


  MainKickoff();
  
  while(DEF_TRUE) 
  {
      OSTimeDlyHMSM(0,0,0,100);
  }
}



int main(void)
{ 
 
    INT8U  err;

    CPU_IntDis();

    BSP_Init(); 
    
	/* Set the Vector Table base location at 0x08000000 */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0); 
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // ylf: 2bits for priority group 

    OSInit();                /* Initialize "uC/OS-II, The Real-Time Kernel"              */
   /* Create the start task */
   OSTaskCreateExt(AppTaskRoot,(void *)0,(OS_STK *)&AppTaskRootStk[APP_TASK_ROOT_STK_SIZE-1],APP_TASK_ROOT_PRIO,APP_TASK_ROOT_PRIO,(OS_STK *)&AppTaskRootStk[0],
                     APP_TASK_ROOT_STK_SIZE,
                     (void *)0,
                     OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);

#if (OS_TASK_NAME_SIZE > 13)
   OSTaskNameSet(APP_TASK_ROOT_PRIO, "Root-Task", &err);
#endif

    OSStart(); 
}



