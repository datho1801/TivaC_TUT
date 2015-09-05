/*
 * main.c
 */
#include <stdint.h>
#include <stdbool.h>
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/flash.h"
#include "inc/hw_gpio.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"

#define USE_API


int main(void) {

	SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
#ifndef USE_API
	(*(uint32_t*) 0x400FE608) |= 1 << 5;
	(*(uint32_t*) (0x40025000 + 0x500)) |= 6;
	(*(uint32_t*) (0x40025000 + 0x51C)) |= 6;
	(*(uint32_t*) (0x40025000 + 0x400)) |= 6;
	(*(uint32_t*) (0x40025000 + (6 << 2))) |= 6;
#else
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,
			GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
	//GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2, 0xff);
#endif

	while (1) {
		GPIOPinWrite(GPIO_PORTF_BASE, 2, 0)
		GPIOPinWrite(GPIO_PORTF_BASE, 4, 0);
		SysCtlDelay(40000000 / 3);
		GPIOPinWrite(GPIO_PORTF_BASE, 4, 4);
		GPIOPinWrite(GPIO_PORTF_BASE, 2, 0);
		SysCtlDelay(40000000 / 3);
	}
}
