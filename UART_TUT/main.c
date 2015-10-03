/*******************************************************************************
 * /UART_TUT/main.c
 *
 *  Created on: Sep 25, 2015
 *      Author: Dat Ho
 *      Email : dat.ho1801@live.com
 ******************************************************************************/

/*******************************************************************************
 * Chương trình sử dụng UART0 được tích hợp chuyển đổi UART<->USB ngay trên kit
 * để demo UART.
 * Trong vòng lặp while(1), chương trình sẽ in chuỗi text ra sau mỗi 1s.
 * Đọc datasheet và user manual để hiểu rõ hơn về cách sử dụng các API
 ******************************************************************************/
#include "library.h"
#include <stdio.h>
uint32_t cout_Char = 0;
uint32_t num;

bool UART_putStr(const char * str) {
    while (*str) {
        UARTCharPut(UART0_BASE, *(str++));
    }
    return true;
}

bool UART_debug(const char* str, ...) {
    char buffer[50];
    va_list list_arg;
    va_start(list_arg, str);
    vsprintf(buffer, str, list_arg);
    UART_putStr(buffer);
    va_end(list_arg);
    return true;
}

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

    /* Mở UART0 */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    /* Mở PortA để sử dụng UART0 */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    /*******************************************************************************
     * Cấu hình chân cho UART0. Chân PA0 là UART0_RX và PA1 là UART0_TX
     ******************************************************************************/
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_1 | GPIO_PIN_1);

    /*******************************************************************************
     * Cấu hình clock và khung truyền cho UART0
     * Cấu hình khung truyền:
     *      8 bits data
     *      1 bit stop
     *      Kiểm tra chẵn lẽ: None
     ******************************************************************************/
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    /*******************************************************************************
     * In ra 1 chuỗi lên màn hình terminal (Hercules)
     ******************************************************************************/
    UART_putStr("Hello everybody!\nThis is UART tutorial for Tiva C\n\n\n");
    while (1) {
        /*******************************************************************************
         * Đoạn text dưới đây sẽ được in ra màn hình mỗi 1s
         * In ra giá trị của cout_Char từ 0 tới 59 sau mỗi giây
         ******************************************************************************/
        UART_debug("Value of count char is %d\n",
                cout_Char >= 59 ? cout_Char = 0 : ++cout_Char);
        SysCtlDelay(40000000 / 3);
    }
}
