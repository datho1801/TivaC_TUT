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
 *      * Tăng: PF1
 *      * Cửa: PF2
 *      * Giảm: PF4
 *      * Start/Stop: PF0
 * Ngõ ra:
 *      * Loa: PB2
 *      * Đèn: PB3
 *      * Quạt: PB4
 *      * Điện trở nhiệt: PB5
 *      * Đầu ra vi sóng: PB6
 ******************************************************************************/

#define CuaDong     GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2) == 1
#define CuaMo       GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2) == 0
#define MODE_AUTO       0
#define MODE_MANUAL     1
#define MODE_USETIMER   2
#define USE_MODE_MICRO       1
#define USE_MODE_GRIAL       2
#define GRIAL_PIN   GPIO_PIN_5
#define MICRO_PIN   GPIO_PIN_6
#define LIGHT_PIN   GPIO_PIN_3
#define SPEAKER_PIN GPIO_PIN_2

uint8_t mode = 1;
bool door_is_close = false;
uint32_t time_alarm = 60;
uint8_t Micro_Grial = 1;

unsigned char * mode_def[3] = {"Auto", "Manual", "Use timer"};

void Interrupt_TimerA0(void) {
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    time_alarm == 0 ? time_alarm = 0 : time_alarm--;
}

void Interrupt_PortF(void) {
    if (GPIOIntStatus(GPIO_PORTF_BASE, false) == GPIO_PIN_1) {
        GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_1);
        time_alarm >= 3600 ? time_alarm = 3600 : time_alarm++;
        Write_Num(time_alarm, 2, 8);
    }

    if (GPIOIntStatus(GPIO_PORTF_BASE, false) == GPIO_PIN_4) {
        GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);
        time_alarm >= 10 ? time_alarm = 10 : time_alarm--;
        Write_Num(time_alarm, 2, 8);

    }

    if (GPIOIntStatus(GPIO_PORTF_BASE, false) == GPIO_PIN_3) {
        GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_3);
        if (door_is_close) {
            if (mode == MODE_USETIMER) {
                mode = MODE_AUTO;
            } else {
                mode++;
            }
        }
    }

    if (GPIOIntStatus(GPIO_PORTF_BASE, false) == GPIO_PIN_2) {
        GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_2);

    }

    if (GPIOIntStatus(GPIO_PORTF_BASE, false) == GPIO_PIN_0) {
            GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_0);
            mode = MODE_USETIMER;
            if(door_is_close)
            {
                TimerEnable(TIMER0_BASE, TIMER_A);
            }
        }


    SysCtlDelay(40000000 / 300);
}

void MCU_Clk_Init(void) {
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
    SYSCTL_OSC_MAIN); // Sử dụng 40MHz
}

void Input_Init(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    GPIODirModeSet(GPIO_PORTF_BASE, 0x1F,
    GPIO_DIR_MODE_IN);

    GPIOPadConfigSet(GPIO_PORTF_BASE, 0x1F,
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
    Write_Str(mode_def[mode],1,0);
    Write_Num(time_alarm, 2, 8);

    while (1) {
        if (CuaDong) {
            door_is_close = true;
            GPIOPinWrite(GPIO_PORTB_BASE, LIGHT_PIN, LIGHT_PIN);
        } else {
            door_is_close = false;
            GPIOPinWrite(GPIO_PORTB_BASE, LIGHT_PIN, 0);
            GPIOPinWrite(GPIO_PORTB_BASE, SPEAKER_PIN, 0);
        }

        switch (mode) {
        case MODE_USETIMER:
            if (time_alarm == 0) {
                TimerDisable(TIMER0_BASE, TIMER_A);
                if (Micro_Grial == USE_MODE_GRIAL) {
                    GPIOPinWrite(GPIO_PORTB_BASE, GRIAL_PIN, 0);
                } else if (Micro_Grial == USE_MODE_MICRO) {
                    GPIOPinWrite(GPIO_PORTB_BASE, MICRO_PIN, 0);
                }
                GPIOPinWrite(GPIO_PORTB_BASE, SPEAKER_PIN, SPEAKER_PIN);
            }
            break;
        case MODE_MANUAL:
            break;
        case MODE_AUTO:
            break;
        default:
            break;
        }

    }
}

