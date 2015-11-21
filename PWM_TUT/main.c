/*******************************************************************************
 * /PWM_TUT/main.c
 *
 *  Created on: Nov 21, 2015
 *      Author: Dat Ho
 *		Email : dat.ho1801@live.com
 ******************************************************************************/
#include "lib.h"

uint32_t Var = 1800;

void Test_Servo() {

    if (!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4)) {
        if (Var < 900) {
            Var = 900;
        } else {
            Var -= 50;
        }
    }

    if (!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0)) {
        if (Var > 2750) {
            Var = 2750;
        } else {
            Var += 50;
        }
    }
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, Var);
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, Var);
    SysCtlDelay(400000);
}

bool ButtonInit(void) {
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;

    GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0, GPIO_DIR_MODE_IN);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0,
    GPIO_STRENGTH_2MA,
    GPIO_PIN_TYPE_STD_WPU);

    return true;
}

bool PwmInit(void) {
    /*******************************************************************************
     * Set clock cho PWM
     * Pwm clock = System clock/32
     ******************************************************************************/
    SysCtlPWMClockSet(SYSCTL_PWMDIV_32);

    /*******************************************************************************
     * Cấu hình chân PWM là PD0 (kết nối tới RC Servo) và PF1 (kết nối tới LED)
     ******************************************************************************/
    GPIOPinTypePWM(GPIO_PORTD_BASE, GPIO_PIN_0);
    GPIOPinConfigure(GPIO_PD0_M1PWM0);
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1);
    GPIOPinConfigure(GPIO_PF1_M1PWM5);

    /*******************************************************************************
     * Cấu hình chu kỳ cho PWM ~ 50Hz
     ******************************************************************************/
    PWMGenConfigure(PWM1_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);
    PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN);
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_0, 24999);
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, 24999);

    PWMOutputState(PWM1_BASE, PWM_OUT_0_BIT | PWM_OUT_5_BIT, true);

    PWMGenEnable(PWM1_BASE, PWM_GEN_0);
    PWMGenEnable(PWM1_BASE, PWM_GEN_2);

    return true;
}

int main(void) {

    SysCtlClockSet(
    SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);

    ButtonInit();
    PwmInit();
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, 1800);
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, 1800);

    while (1) {
        Test_Servo();
    }
}

