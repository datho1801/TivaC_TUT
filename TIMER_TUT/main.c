/*******************************************************************************
 * /TIMER_TUT/main.c
 *
 *  Created on: Nov 27, 2015
 *      Author: Dat Ho
 *		Email : dat.ho1801@live.com
 ******************************************************************************/
#include "lib.h"

#define SYSTICK_CONFIG_100MS    40000000
#define TIMER_VAL_1S            4000000

uint32_t count_2 = 0, count_1 = 4;

void SysTickHandler(void) {
    if(count_1++ >= 4)
    {
        count_1 = 0;
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1,
                    ~GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1));
    }
    if(count_2++ >= 7)
    {
        count_2 = 0;
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2,
                            ~GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2));
    }

}

void Timer0AISR(void) {
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3,
            ~GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_3));
}

bool Timer0A_Config(uint32_t Val) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, Val);
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    return true;
}

bool Led_Config(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,
    GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3,
            GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    return true;
}

uint8_t SysTick_Config(uint32_t time) {
    SysTickPeriodSet(time);
    SysTickIntEnable();
    return true;
}

int main(void) {
    SysCtlClockSet(
    SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

    Led_Config();

    Timer0A_Config(TIMER_VAL_1S);
    TimerEnable(TIMER0_BASE, TIMER_A);

    SysTick_Config(SYSTICK_CONFIG_100MS);
    SysTickEnable();

    while (1) {
    }
}

