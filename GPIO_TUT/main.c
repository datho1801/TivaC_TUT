/*******************************************************************************
 * GPIO_TUT/main.c
 *         Author: Dat Ho
 *         2015 Sep 11
 *      Chương trình sử dụng 2 nút nhấn có sẵn trên kit Tiva C TM4C123G LauchPad
 * được kết nối tới chân PF0 và PF4 để thay đổi tần số chớp nháy 3 của kit.
 * 3 Leds này được kết nối tới 3 chân PF1, PF2, PF3 của kit.
 *      Trong code có sử dụng define symbol USE_API nhằm chuyển đổi giữa 2 cách
 *cấu hình cho kit: Cấu hình bằng API hoặc bằng cách ghi giá trị trực tiếp vào
 * thanh ghi. Đọc datasheet để biết rõ hơn về các thanh ghi.
 ******************************************************************************/
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

//#define USE_API
#define USE_PORTF_INTERRUPT

uint8_t var = 0x02, f_blink = 3;

#ifdef USE_PORTF_INTERRUPT

/*******************************************************************************
 * Hàm xử lý ngắt của port F, được khai báo extern trong file
 * tm4c123gh6pm_startup_ccs.c
 ******************************************************************************/
void PFIntHandler(void) {
    if (GPIOIntStatus(GPIO_PORTF_BASE, true) == GPIO_PIN_0) {
        GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_0);
        if (f_blink++ > 9) f_blink = 9;
    }

    if (GPIOIntStatus(GPIO_PORTF_BASE, true) == GPIO_PIN_4) {
        GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);
        if(f_blink-- < 1) f_blink = 1;
    }
    SysCtlDelay(40000000/300);
}

#endif /* USE_PORTF_INTERRUPT */

int main(void) {
/*******************************************************************************
 * Cấu hình clock cho hệ thống
 * Cấu hình:
 *          Sử dụng thạch anh ngoài 16MHz
 *          Sử dụng PLL chia xung lên 400MHz
 *          Sau khi qua bộ chia 5, xung đầu ra sẽ là 400/2/5 MHz
 *          (Tham khảo thêm bộ chia vòng khóa pha PLL để biết chi tiết)
 ******************************************************************************/
    SysCtlClockSet(
            SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

#ifndef USE_API
    /*******************************************************************************
     * Sử dụng bằng cách ghi các giá trị tương ứng vào thanh ghi
     ******************************************************************************/

    /* Mở xung cho port F */
    (*(uint32_t*) 0x400FE608) |= 1 << 5;
    /* 2-mA Drive Select for Pin1, Pin2, Pin3 */
    (*(uint32_t*) (0x40025000 + 0x500)) |= 0x0E;
    /* Digital enable for Pin1, Pin2, Pin3 */
    (*(uint32_t*) (0x40025000 + 0x51C)) |= 0x0E;
    /* Cấu hình Pin1, Pin2, Pin3 là output */
    (*(uint32_t*) (0x40025000 + 0x400)) |= 0x0E;
    /* Ghi dữ liệu tới Pin1, Pin2, Pin3 */
    (*(uint32_t*) (0x40025000 + (0x0E << 2))) |= 0x0E;
#else
    /*******************************************************************************
     * Sử dụng các API trong tivaware
     ******************************************************************************/

    /* Mở xung cho port F */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    /* Cấu hình Pin1, Pin2, Pin3 là output */
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,
            GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    /* Ghi dữ liệu tới Pin1, Pin2, Pin3 */
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3,
            GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
#endif /* USE_API */

#ifdef USE_PORTF_INTERRUPT
    /*******************************************************************************
     * PF0 là chân NMI, do đó phải sử dụng lệnh dưới đây để unlock và sử dụng như
     * GPIO
     ******************************************************************************/
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;

    /* Cấu hình chân PF0 và PF4 là input */
    GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0, GPIO_DIR_MODE_IN);
    /* Sử dụng điện trở kéo lên */
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0, GPIO_STRENGTH_2MA,
        GPIO_PIN_TYPE_STD_WPU);
    /* Cấu hình ngắt xảy ra ở cạnh lên */
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, GPIO_FALLING_EDGE);
    /* Mở ngắt cho port F */
    IntEnable(INT_GPIOF);
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);
#endif /* USE_PORTF_INTERRUPT */

    while (1) {
        if ((var *= 2) > 8) var = 2;
#ifdef USE_API
        GPIOPinWrite(GPIO_PORTF_BASE, 0x0E, var);
#else
        (*(uint32_t*) (0x40025000 + (0x0E << 2))) = var;
#endif /* USE_API */
        SysCtlDelay(40000000 / f_blink);
    }
}
