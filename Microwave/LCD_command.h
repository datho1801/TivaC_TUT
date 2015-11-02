/*******************************************************************************
 * /Microwave/LCD_command.h
 *
 *  Created on: Nov 2, 2015
 *      Author: Dat Ho
 *		Email : dat.ho1801@live.com
 ******************************************************************************/
#include "lib.h"

#ifndef LCD_COMMAND_H_
#define LCD_COMMAND_H_

#define RS_0    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0)
#define RS_1    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, GPIO_PIN_2)
#define E_0     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0)
#define E_1     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_3)

#define LCDPort GPIO_PORTA_BASE
#define D4  GPIO_PIN_4
#define D5  GPIO_PIN_5
#define D6  GPIO_PIN_6
#define D7  GPIO_PIN_7

void LCD_Init(void);
void E_Pulse(void);
void Write_Command(uint8_t cmd);
void Write_Char(uint8_t m_char);
void Write_Str(uint8_t* str, uint8_t row, uint8_t col);

#endif /* LCD_COMMAND_H_ */
