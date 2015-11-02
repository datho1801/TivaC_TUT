/*******************************************************************************
 * /Microwave/lib.h
 *
 *  Created on: Oct 30, 2015
 *      Author: Dat Ho
 *		Email : dat.ho1801@live.com
 ******************************************************************************/

#ifndef LIB_H_
#define LIB_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "driverlib/rom.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "driverlib/flash.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/debug.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/fpu.h"
#include "utils/uartstdio.h"
#include "driverlib/timer.h"
#include "LCD_command.h"


#define SysClkHz    40000000
#define Delayms     (SysClkHz/3000)
#define Delayus     (SysClkHz/3000000)


#endif /* LIB_H_ */
