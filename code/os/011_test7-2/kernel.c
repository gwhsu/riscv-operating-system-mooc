#include "types.h"


extern void uart_init(void);
extern void uart_puts(const char *s);
extern uint8_t uart_getc(void);


void start_kernel(void)
{
    uint8_t rxChar = 0;

    // 串口初始化
    uart_init();
    // Hello, RVOS!
    uart_puts("Hello, RVOS!\n");

    while(1)
    {
        rxChar = uart_getc();
        if((rxChar == '\r') || (rxChar == '\r')) // 按下 Enter时，换行
        {
            uart_puts("\n");
        }
        else
        {
            uart_puts((const char *)&rxChar);
        }
    }
}

