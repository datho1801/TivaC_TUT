/*******************************************************************************
 * /Microwave/LCD_command.c
 *
 *  Created on: Nov 2, 2015
 *      Author: Dat Ho
 *		Email : dat.ho1801@live.com
 ******************************************************************************/

#include "LCD_command.h"

void LCD_Init(void) {
    RS_0;
    E_0;
    SysCtlDelay(200 * Delayms);

    GPIOPinWrite(LCDPort, D4 | D5 | D6 | D7, 0x30);
    E_Pulse();
    SysCtlDelay(50 * Delayms);

    GPIOPinWrite(LCDPort, D4 | D5 | D6 | D7, 0x30);
    E_Pulse();
    SysCtlDelay(50 * Delayms);

    GPIOPinWrite(LCDPort, D4 | D5 | D6 | D7, 0x30);
    E_Pulse();
    SysCtlDelay(10 * Delayms);

    GPIOPinWrite(LCDPort, D4 | D5 | D6 | D7, 0x20);
    E_Pulse();
    SysCtlDelay(10 * Delayms);

    Write_Command(0x01);
    Write_Command(0x06);
    Write_Command(0x0F);
    Write_Command(0x0E);
    Write_Command(0x0D);

}

void E_Pulse(void) {
    E_0;
    SysCtlDelay(1 * Delayus);
    E_1;
    SysCtlDelay(1 * Delayus);
    E_0;
}

void Write_Command(uint8_t cmd) {
    RS_0;
    SysCtlDelay(20 * Delayus);
    GPIOPinWrite(LCDPort, D4 | D5 | D6 | D7, (cmd & 0xF0));
    E_Pulse();

    GPIOPinWrite(LCDPort, D4 | D5 | D6 | D7, (cmd & 0x0F) << 4);
    E_Pulse();
    SysCtlDelay(Delayms * 5);
}

void Write_Char(uint8_t m_char) {
    RS_1;
    SysCtlDelay(20 * Delayus);
    E_0;
    GPIOPinWrite(LCDPort, D4 | D5 | D6 | D7, (m_char & 0xF0));
    E_Pulse();

    GPIOPinWrite(LCDPort, D4 | D5 | D6 | D7, (m_char & 0x0F) << 4);
    E_Pulse();
    SysCtlDelay(10 * Delayus);
    RS_0;
}

void Write_Str(uint8_t* str, uint8_t row, uint8_t col) {
    uint8_t address_d = 0;
    switch (row) {
    case 0:
        address_d = 0x80 + col;
        break;
    case 1:
        address_d = 0xC0 + col;
        break;
    case 2:
        address_d = 0x94 + col;
        break;
    case 3:
        address_d = 0xD4 + col;
        break;
    default:
        address_d = 0x80 + col;
        break;
    }
    Write_Command(address_d);
    while (*str) {
        Write_Char(*str++);
    }
}


