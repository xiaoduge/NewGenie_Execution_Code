#include    <ucos_ii.h>

#include    <string.h>

#include "stm32_eval.h"

#include "app_cfg.h"

#include "Dica.h"

#include "Display.h"

#include "Beep.h"


#define INVALID_INDEX       0xFF

#define IOCTRL_INPUT_DEBOUNCE_COUNT    3 

// INPUT PINS ARE MAP TO 2.0~P2.7

/// Defines the Debounce Entry value and index variables
typedef struct  
{
    /// Debounce count down value
    UINT8   count;    
    
    /// Debounce index value
    UINT8   index;    

} IOCTRL_INPUT_DEBOUNCE_ENTRY;


UINT16  Ioctrl_input_last_report_key_status ;

UINT16  Ioctrl_input_last_key_status;

/*temp value */
UINT16    Ioctrl_input_current_state;
UINT16    Ioctrl_input_last_state;
UINT16    Ioctrl_input_key_delta;
UINT16    Ioctrl_input_key_change;
UINT16    Ioctrl_input_key ;

static IOCTRL_INPUT_DEBOUNCE_ENTRY    Ioctrl_input_debounce_queue[INPUT_NUMBER];
const uint8_t inputs[INPUT_NUMBER] = {
    STM32F103_GPG(2),     
    STM32F103_GPG(3),     
    STM32F103_GPG(4),     
};

//--------------------------------------------------------------------------
// get_key_state
//
/// This helper function reads the state of the buttons and translates button 
/// state to report state.
//--------------------------------------------------------------------------
UINT16 Ioctrl_input_get_key_state( void )
{
    UINT16 keystate;

    keystate = (GPIO_ReadInputData(GPIOG) >> 2) & INPUT_MASK;
    
    return keystate;
}

//--------------------------------------------------------------------------
// Debounce_Init
//
/// This function initializes the debounce logic for the buttons.
//--------------------------------------------------------------------------
void Ioctrl_input_Debounce_Init( void )
{
    UINT8   i;

    //initialize debounce queue
    for ( i = 0; i < INPUT_NUMBER; ++i )
    {
        Ioctrl_input_debounce_queue[i].index = INVALID_INDEX;
    }
}



//--------------------------------------------------------------------------
//
// Function: age_debounce_queue
//
//  Description:
///     This function decrements debounce values for all buttons every time 
///     the mouse buttons are polled. This provides a debounce period 
///     for when a button is pressed.
///
/// Returns:
///     void
//
//--------------------------------------------------------------------------
void Ioctrl_input_age_debounce_queue( void )
{
    UINT8   i;
    UINT8   value;

    for ( i = 0; i < INPUT_NUMBER; ++i )
    {
        IOCTRL_INPUT_DEBOUNCE_ENTRY *debounceEntry = &Ioctrl_input_debounce_queue[i];
        
        //Get debounce value
        value = debounceEntry->count;

        //Check for debounce value greater than 0
        if ( 0 < value )
        {
            //Decrement debounce value
            debounceEntry->count = --value;

            //If value reaches 0 ignore the key
            if ( value == 0 )
            {
                //Remove index
                debounceEntry->index = INVALID_INDEX;
            }
        }
    }
}


//--------------------------------------------------------------------------
//
// Function: debounce
//
//  Description:
///     This function starts the debounce clock when a button is 
///     pressed and blocks out further button state changes until the 
///     debounce time has expired.
///
/// Returns:
///     - TRUE  : if index debounce has occured
///     - FALSE : if index already exists or no free entry in table
//
//--------------------------------------------------------------------------
BOOL Ioctrl_input_debounce( UINT8 index )
{
    UINT8   i;

    //Check for index
    for ( i = 0; i < INPUT_NUMBER; ++i )
    {
        if ( index == Ioctrl_input_debounce_queue[i].index )
        {
            if (Ioctrl_input_debounce_queue[i].count < 2)
                return TRUE;
            else
                return FALSE;
        }
    }

    //Add index (if space available)
    for ( i = 0; i < INPUT_NUMBER; ++i )
    {
        if ( Ioctrl_input_debounce_queue[i].index == INVALID_INDEX )
        {
            //Add index to first available entry
            Ioctrl_input_debounce_queue[i].index = index;
            Ioctrl_input_debounce_queue[i].count = IOCTRL_INPUT_DEBOUNCE_COUNT;

            return ( FALSE );
        }
    }

    //No free entry
    return ( FALSE );
}

//--------------------------------------------------------------------------
// buttons_get_report
//
/// This function polls the button status and performs debouncing on any buttons
/// that change state.  It also formats the report for transmission.
//--------------------------------------------------------------------------
BOOL Ioctrl_input_Keys_Get_Report( void)
{

    UINT8    i;
    BOOL     report_changed = TRUE;

    Ioctrl_input_key = 0x1;

    Ioctrl_input_current_state = Ioctrl_input_get_key_state();

    Ioctrl_input_last_state = Ioctrl_input_last_key_status;
    
    // XOR out repeating button presses
    Ioctrl_input_key_delta = (Ioctrl_input_current_state ^ Ioctrl_input_last_state) & INPUT_MASK;


    // Debounce buttons
    for ( i = 0; i < INPUT_NUMBER; ++i )
    {
        if ( ( Ioctrl_input_key_delta & Ioctrl_input_key ) && Ioctrl_input_debounce( i ) ) // button changed state
        {
            if ( Ioctrl_input_current_state & Ioctrl_input_key )
            {
                Ioctrl_input_last_state |= Ioctrl_input_key;                  // button on
            }
            else
            {
                Ioctrl_input_last_state &= ~Ioctrl_input_key;                 // button off
            }
        }

        Ioctrl_input_key <<= 1;
    }

    Ioctrl_input_age_debounce_queue();

    if ( Ioctrl_input_last_state == Ioctrl_input_last_key_status)
    {
        report_changed = FALSE;        
    }
    else
    {
        Ioctrl_input_last_key_status = Ioctrl_input_last_state;

        Ioctrl_input_key_change = Ioctrl_input_key_delta;
    }

    return ( report_changed );
}

/*********************************************************************
 * Function:        void InputStateReport( void )
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Check input state
 *
 * Note:            None.
 * History :
               1. 2012/04/18 Change Manul input from fixed to toggle mode
 ********************************************************************/

void InputStateReport(void)
{

    if (!Ioctrl_input_Keys_Get_Report())
    {
        return ;
    }

    // other input pin state changed
    //if (Ioctrl_input_key_change & (~Input_usage))
    if (Ioctrl_input_key_change & INPUT_MASK) // 20121011
    {
        // report to host
        // add promoting BEEP according to chenggong 2012/10/19
        //InputChangeReportEntry(Ioctrl_input_key_change & (~Input_usage));
        Disp_DinStateChange(GetAllInputLogicState());
    }
        
}




//--------------------------------------------------------------------------
// buttons_init
//
/// This function initializes the port, state and interrupt handler for the 
/// buttons component.
//--------------------------------------------------------------------------
void Ioctrl_input_keys_init( void )
{
    Ioctrl_input_last_key_status        = 0XFFFF;
    Ioctrl_input_last_report_key_status = 0xFFff;

    Ioctrl_input_current_state = 0;
    Ioctrl_input_last_state    = 0;
    Ioctrl_input_key_delta     = 0;
    Ioctrl_input_key_change    = 0;
    Ioctrl_input_key           = 0x1;

}

/*********************************************************************
 * Function:        void InputReset( void )
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Restore Input state to init
 *
 * Note:            None.
 * History :
               1. 2012/04/18 Change Manul input from fixed to toggle mode
 ********************************************************************/
void InputReset(void)
{
    Ioctrl_input_keys_init();
}


//--------------------------------------------------------------------------
// buttons_init
//
/// This function initializes the port, state and interrupt handler for the 
/// buttons component.
//--------------------------------------------------------------------------
void Ioctrl_input_init( void )
{
    int iLoop;

    Ioctrl_input_keys_init();

    Ioctrl_input_Debounce_Init();

    // begin user add code here to initialize input configuration

    // end user add code here
#if (RTC_SUPPORT == 0)   
    /* Enable PWR and BKP clocks */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR , ENABLE);
    
    /* Allow access to BKP Domain */
    PWR_BackupAccessCmd(ENABLE);
    
    RCC_LSEConfig(RCC_LSE_OFF);// turn off LSE to enable  PC14 PC15 as gpio
    
    //BKP_TamperPinCmd(DISABLE);
    PWR_BackupAccessCmd(DISABLE);
#endif    

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    for (iLoop = 0; iLoop < INPUT_NUMBER; iLoop++)
    {
        stm32_gpio_cfgpin(inputs[iLoop],MAKE_PIN_CFG(GPIO_PuPd_UP,GPIO_OType_PP,GPIO_Speed_50MHz,GPIO_Mode_IN)); 
    }

    Display.ausInputRegs[APP_EXE_INPUT_REG_DIN_OFFSET] = GetAllInputLogicState();

}

UINT8 GetInputLogicStatus(UINT8 ucChannel)
{
    UINT16 R1;
    R1 = (Ioctrl_input_last_state & (1 << ucChannel));
    return !!(R1);
}

UINT16 GetAllInputLogicState(void)
{
    UINT8 ucLoop;
    UINT16 usMask = 0;

    for (ucLoop = 0; ucLoop < INPUT_NUMBER; ucLoop++)
    {
        if (GetInputLogicStatus(ucLoop))
        {
            usMask |=  1 << ucLoop;
        }
    }

    return usMask;
}

