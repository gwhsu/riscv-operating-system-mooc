#include "os.h"

// 下面的函数只会在这里调用一次，
// 所以只声明了一次，而不包含在文件os.h中。
extern void uart_init(void);
extern void malloc_init(void);

extern void word_test();

void start_kernel(void)
{
    // 串口初始化
    uart_init();
    // Hello, RVOS!
    uart_puts("Hello, RVOS!\n");

	malloc_init();

    word_test(); // 测试

    while(1)
    {
    }
}

