/*******************************************************************************
 * /ADC_TUT/main.c
 *
 *  Created on: Oct 3, 2015
 *      Author: Dat Ho
 *		Email : dat.ho1801@live.com
 ******************************************************************************/

#include "define.h"
uint32_t ADC_value[4];

uint32_t ADC_getVal(void) {
    uint32_t avg_Value = 0, tempC = 0;
    ADCIntClear(ADC0_BASE, 1);
    ADCProcessorTrigger(ADC0_BASE, 1);
    while (!ROM_ADCIntStatus(ADC0_BASE, 1, false)) {
    }
    ADCSequenceDataGet(ADC0_BASE, 1, ADC_value);
    avg_Value = (ADC_value[0] + ADC_value[1] + ADC_value[2] + ADC_value[3] + 2)
            / 4;
    tempC = (1475 - ((2475 * avg_Value)) / 4096) / 10;

    return tempC;
}

int main(void) {
    SysCtlClockSet(
    SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
    UARTStdioConfig(0, 115200, 16000000);

    ADCSequenceDisable(ADC0_BASE, 1);
    ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 3,
    ADC_CTL_TS | ADC_CTL_IE | ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 1);

    UARTprintf("This is ADC examlpe");

    while (1) {
        UARTprintf("Temperature is: %d \210 C\n", ADC_getVal());
        SysCtlDelay(40000000 / 3);
    }
}

