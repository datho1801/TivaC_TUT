/*******************************************************************************
 * /Microwave/main.c
 *
 *  Created on: Oct 30, 2015
 *      Author: Dat Ho
 *		Email : dat.ho1801@live.com
 ******************************************************************************/
#include "lib.h"

/*******************************************************************************
 * Định nghĩa những ngoại vi:
 *
 * LCD:
 *      * PA2: Chân RS
 *      * PA3: Chân Enable
 *      * PA4 - PA7: Chân data
 * 1 Timer: Timer A0
 * Ngõ vào
 *      * Mode: PF3
 *      * Cửa: PF2
 *      * Start/Stop: PF0
 *      * Tăng phút: PE0
 *      * Giảm phút: PE1
 *      * Tăng giây: PE2
 *      * Giảm giây: PE3
 * Ngõ ra:
 *      * Loa: PB2
 *      * Đèn: PB3
 *      * Quạt: PB4
 *      * Điện trở nhiệt: PB5
 *      * Đầu ra vi sóng: PB6
 ******************************************************************************/

#define CuaDong     GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2) == 1
#define CuaMo       GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2) == 0
#define MODE_MICRO           0
#define MODE_GRILL           1

#define USE_MODE_MICRO       1
#define USE_MODE_GRILL       2

#define GRILL_PIN           GPIO_PIN_5
#define MICRO_PIN           GPIO_PIN_6
#define LIGHT_PIN           GPIO_PIN_3
#define SPEAKER_PIN         GPIO_PIN_2

uint8_t mode = 0;
bool door_is_close = false, is_active = false, is_started = false;
uint32_t min_alarm, sec_alarm, time_val, min_display = 0, sec_display = 0;
uint8_t Micro_Grial = 1;

unsigned char * mode_def[2] = { "MicroWave", "Grill" };

void Interrupt_TimerA0(void) {
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    time_val--;
    min_display = time_val / 60;
    sec_display = time_val - (min_display * 60);
    Write_Time(min_display, sec_display, 0, 9);
    if (time_val == 0) {
        // Bật loa
        GPIOPinWrite(GPIO_PORTB_BASE, LIGHT_PIN, LIGHT_PIN);
        // Bật đèn
        GPIOPinWrite(GPIO_PORTB_BASE, SPEAKER_PIN, SPEAKER_PIN);
        // Tắt timer
        TimerDisable(TIMER0_BASE, TIMER_A);
        // Tắt thiết bị phát nhiệt
        GPIOPinWrite(GPIO_PORTB_BASE, GRILL_PIN | MICRO_PIN, 0x00);

        is_active = false;
    }

}

void Interrupt_PortF(void) {
    if (GPIOIntStatus(GPIO_PORTF_BASE, false) == GPIO_PIN_1) {
        GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_1);
        //time_alarm >= 3600 ? time_alarm = 3600 : time_alarm++;
        //Write_Num(time_alarm, 2, 8);
    }

    if (GPIOIntStatus(GPIO_PORTF_BASE, false) == GPIO_PIN_4) {
        GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);
        //time_alarm >= 10 ? time_alarm = 10 : time_alarm--;
        //Write_Num(time_alarm, 2, 8);

    }

    if (GPIOIntStatus(GPIO_PORTF_BASE, false) == GPIO_PIN_3) {
        GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_3);
//        if (door_is_close) {
//            if (mode == MODE_USETIMER) {
//                mode = MODE_AUTO;
//            } else {
//                mode++;
//            }
//        }
    }

    if (GPIOIntStatus(GPIO_PORTF_BASE, false) == GPIO_PIN_2) {
        GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_2);

    }

    if (GPIOIntStatus(GPIO_PORTF_BASE, false) == GPIO_PIN_0) {
        GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_0);
//        mode = MODE_USETIMER;
//        if (door_is_close) {
//            TimerEnable(TIMER0_BASE, TIMER_A);
//        }
    }

    SysCtlDelay(40000000 / 300);
}

void MCU_Clk_Init(void) {
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
    SYSCTL_OSC_MAIN); // Sử dụng 40MHz
}

void Input_Init(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    GPIODirModeSet(GPIO_PORTF_BASE, 0x1F,
    GPIO_DIR_MODE_IN);

    GPIODirModeSet(GPIO_PORTE_BASE, 0x0F,
    GPIO_DIR_MODE_IN);

    GPIOPadConfigSet(GPIO_PORTF_BASE, 0x1F,
    GPIO_STRENGTH_2MA,
    GPIO_PIN_TYPE_STD_WPU);

    GPIOPadConfigSet(GPIO_PORTE_BASE, 0x0F,
    GPIO_STRENGTH_2MA,
    GPIO_PIN_TYPE_STD_WPU);

    GPIOIntTypeSet(GPIO_PORTF_BASE, 0x1F, GPIO_RISING_EDGE);
    IntEnable(INT_GPIOF);
    GPIOIntEnable(GPIO_PORTF_BASE, 0x1F);
}

void Output_Init(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, 0x7C);
}

void Timer0A_Config(uint32_t Val) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, Val);

    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
}

int main(void) {

    MCU_Clk_Init();
    Input_Init();
    Output_Init();
    LCD_Init();
    Timer0A_Config(40000000);

    Write_Str("MICROWAVE_PROGRAM", 0, 0);
    Write_Str(mode_def[mode], 1, 0);

    while (1) {
        if (CuaDong) {
            door_is_close = true;
            GPIOPinWrite(GPIO_PORTB_BASE, LIGHT_PIN, LIGHT_PIN);
        } else {
            door_is_close = false;
            if (is_active) {
                GPIOPinWrite(GPIO_PORTB_BASE, GRILL_PIN, GRILL_PIN);
                GPIOPinWrite(GPIO_PORTB_BASE, MICRO_PIN, MICRO_PIN);
                TimerDisable(TIMER0_BASE, TIMER_A);
                GPIOPinWrite(GPIO_PORTB_BASE, SPEAKER_PIN, 0);
            } else {
                GPIOPinWrite(GPIO_PORTB_BASE, LIGHT_PIN, 0);
                GPIOPinWrite(GPIO_PORTB_BASE, SPEAKER_PIN, 0);
            }

        }

        if (!GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_0)) {
            min_alarm > 59 ? min_alarm = 0 : min_alarm++;
        }

        if (!GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_1)) {
            min_alarm <= 0 ? min_alarm = 0 : min_alarm--;
        }

        if (!GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_2)) {
            sec_alarm > 59 ? sec_alarm = 0 : sec_alarm++;
        }

        if (!GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_3)) {
            sec_alarm <= 0 ? sec_alarm = 0 : sec_alarm--;
        }

        if (!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0)) {
            if (is_started) {
                is_started = false;
            } else {
                is_started = true;
            }

        }

        if (!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_3)) {
            if (!is_active) mode > MODE_GRILL ? mode = MODE_MICRO : mode++;

        }

        if (is_started) {
            if (!is_active) {
                is_active = true;
                time_val = min_alarm * 60 + sec_alarm;
                if (time_val <= 9) {
                    Write_Str("Error! Time is small", 3, 0);
                } else {
                    if (door_is_close) {
                        Write_Str("Running:", 2, 0);
                        Write_Str(mode_def[mode], 2, 10);
                        TimerEnable(TIMER0_BASE, TIMER_A);
                        if (mode == MODE_GRILL) {
                            GPIOPinWrite(GPIO_PORTB_BASE, GRILL_PIN, GRILL_PIN);
                        } else {
                            GPIOPinWrite(GPIO_PORTB_BASE, MICRO_PIN, MICRO_PIN);
                        }
                    }
                }
            } else {
                min_alarm = sec_alarm = time_val = 0;
                TimerDisable(TIMER0_BASE, TIMER_A);
            }
        }

    }
}

