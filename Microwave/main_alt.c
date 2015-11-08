/*******************************************************************************
 * /Microwave/main_alt.c
 *
 *  Created on: Nov 8, 2015
 *      Author: Dat Ho
 *		Email : dat.ho1801@live.com
 ******************************************************************************/

/*******************************************************************************
 * /Microwave/main.c
 *
 *  Created on: Oct 30, 2015
 *      Author: Dat Ho
 *      Email : dat.ho1801@live.com
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
bool door_is_close = false, is_active = false, start_stop = false;
uint32_t min_alarm, sec_alarm, time_val, min_display = 0, sec_display = 0;
uint8_t Micro_Grial = 1;

unsigned char * mode_def[2] = { "MicroWave", "Grill" };

void Interrupt_TimerA0(void) {
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    // Giam bien dem
    time_val--;

    // Tinh phut de hien thi len man hinh
    min_display = time_val / 60;

    // Tinh giay de hien thi len man hinh
    sec_display = time_val - (min_display * 60);
    // Hien thi len man hinh
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

void Interrupt_PortE(void) {
    if (GPIOIntStatus(GPIO_PORTE_BASE, false) == GPIO_PIN_1) {
        GPIOIntClear(GPIO_PORTE_BASE, GPIO_PIN_1);
        min_alarm <= 0 ? min_alarm = 0 : min_alarm--;
    }

    if (GPIOIntStatus(GPIO_PORTE_BASE, false) == GPIO_PIN_3) {
        GPIOIntClear(GPIO_PORTE_BASE, GPIO_PIN_3);
        sec_alarm <= 0 ? sec_alarm = 0 : sec_alarm--;
    }

    if (GPIOIntStatus(GPIO_PORTE_BASE, false) == GPIO_PIN_2) {
        GPIOIntClear(GPIO_PORTE_BASE, GPIO_PIN_2);
        sec_alarm > 59 ? sec_alarm = 0 : sec_alarm++;
    }

    if (GPIOIntStatus(GPIO_PORTE_BASE, false) == GPIO_PIN_0) {
        GPIOIntClear(GPIO_PORTE_BASE, GPIO_PIN_0);
        min_alarm > 59 ? min_alarm = 0 : min_alarm++;
    }
    // Delay 10ms de han che nhieu tu nut nhan
    SysCtlDelay(40000000/300);
}

void Interrupt_PortF(void) {
    if (GPIOIntStatus(GPIO_PORTF_BASE, false) == GPIO_PIN_1) {
        GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_1);

    }

    if (GPIOIntStatus(GPIO_PORTF_BASE, false) == GPIO_PIN_4) {
        GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);

    }

    if (GPIOIntStatus(GPIO_PORTF_BASE, false) == GPIO_PIN_3) {
        GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_3);
        if (!is_active) mode > MODE_GRILL ? mode = MODE_MICRO : mode++;
    }

    if (GPIOIntStatus(GPIO_PORTF_BASE, false) == GPIO_PIN_2) {
        GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_2);

    }

    if (GPIOIntStatus(GPIO_PORTF_BASE, false) == GPIO_PIN_0) {
        GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_0);

        if (start_stop) {
            start_stop = false;
        } else {
            start_stop = true;
        }

        if (start_stop) {
            is_active = true;
            time_val = min_alarm * 60 + sec_alarm;
            if (time_val <= 9) {
                Write_Str("Error! Time is small", 3, 0);
            } else {
                // kiem tra neu cua dong moi bat dau nuong
                if (door_is_close) {
                    // hien thi trang thai cho nguoi dung
                    Write_Str("Running:", 2, 0);
                    // hien thi mode hien tai
                    Write_Str(mode_def[mode], 2, 10);
                    // bat dau dem nguoc thoi gian
                    TimerEnable(TIMER0_BASE, TIMER_A);
                    // lua chon mode la micro hay grill
                    if (mode == MODE_GRILL) {
                        GPIOPinWrite(GPIO_PORTB_BASE, GRILL_PIN, GRILL_PIN);
                    } else {
                        GPIOPinWrite(GPIO_PORTB_BASE, MICRO_PIN, MICRO_PIN);
                    }
                }
            }
        } else {
            // Xoa man hinh
            Write_Command(0x01);
            // Hien thi che do
            Write_Str("Stoped!", 2, 0);
            // Reset bien
            min_alarm = sec_alarm = time_val = 0;
            // Tat timer
            TimerDisable(TIMER0_BASE, TIMER_A);
        }
    }

    SysCtlDelay(40000000 / 300);
}

void MCU_Clk_Init(void) {
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
    SYSCTL_OSC_MAIN); // Sử dụng 40MHz
}

void Input_Init(void) {
    // Mo clock cho ngoai vi
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    // Cau hinh la ngo vao cho port F
    GPIODirModeSet(GPIO_PORTF_BASE, 0x1F,
    GPIO_DIR_MODE_IN);

    // Cau hinh la ngo vao cho port E
    GPIODirModeSet(GPIO_PORTE_BASE, 0x0F,
    GPIO_DIR_MODE_IN);

    // Cau hinh tro keo len cho port F
    GPIOPadConfigSet(GPIO_PORTF_BASE, 0x1F,
    GPIO_STRENGTH_2MA,
    GPIO_PIN_TYPE_STD_WPU);

    GPIOPadConfigSet(GPIO_PORTE_BASE, 0x0F,
    GPIO_STRENGTH_2MA,
    GPIO_PIN_TYPE_STD_WPU);

    // Cau hinh va enable ngat cho port F
    GPIOIntTypeSet(GPIO_PORTF_BASE, 0x1F, GPIO_RISING_EDGE);
    IntEnable(INT_GPIOF);
    GPIOIntEnable(GPIO_PORTF_BASE, 0x1F);

    GPIOIntTypeSet(GPIO_PORTE_BASE, 0x0F, GPIO_RISING_EDGE);
    IntEnable(INT_GPIOE);
    GPIOIntEnable(GPIO_PORTE_BASE, 0x0F);
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
    // Cau hinh timer xay ra ngat sau 1s
    Timer0A_Config(40000000);

    Write_Str("Hello World!", 0, 0);

    while (1) {
        if (CuaDong) {
            // Bat den
            door_is_close = true;
            GPIOPinWrite(GPIO_PORTB_BASE, LIGHT_PIN, LIGHT_PIN);
        } else {
            door_is_close = false;
            if (is_active) {
                // Neu dang nuong ma mo cua thi tat ngay lap tuc thiet bi nuong, tat timer va bat loa canh bao
                GPIOPinWrite(GPIO_PORTB_BASE, GRILL_PIN, GRILL_PIN);
                GPIOPinWrite(GPIO_PORTB_BASE, MICRO_PIN, MICRO_PIN);
                TimerDisable(TIMER0_BASE, TIMER_A);
                GPIOPinWrite(GPIO_PORTB_BASE, SPEAKER_PIN, 0);
            } else {
                // Neu da nuong xong va cua mo thi tat loa
                GPIOPinWrite(GPIO_PORTB_BASE, SPEAKER_PIN, 0);
            }

        }
    }
}

