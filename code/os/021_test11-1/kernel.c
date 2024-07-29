#include "os.h"

// 下面的函数只会在这里调用一次，
// 所以只声明了一次，而不包含在文件os.h中。
extern void uart_init(void);
void enable_uart0_interrupt(uint8_t en);

extern void page_init(void);
extern void page_test();

extern void sched_init(void);
extern void schedule(void);
extern void os_main(void);

extern void trap_init(void);
extern void trap_test(void);

extern void plic_init(void);



#define DELAY 1000

void user_task0(void)
{
	uart_puts("Task 0: Created!\n");
	while (1) {
		uart_puts("Task 0: Running...\n");

		// trap_test();

		task_delay(DELAY);
		task_yield();
	}
}

void user_task1(void)
{
	char c = '\n';
	uart_puts("Task 1: Created!\n");
	while (1) {
		
		c = uart_getc();

		// 使能串口0发送中断
		enable_uart0_interrupt(1);
		uart_putc((char)c);

		

		task_yield();
	}
}

void user_create_tesk(void)
{
	task_create(user_task0);
	task_create(user_task1);
}


void start_kernel(void)
{
    // 串口
    uart_init();
    uart_puts("Hello, RVOS!\n");

    // 页分配
	page_init();
    page_test(); // 测试

	// trap
	trap_init();

	// 中断
	plic_init();

    // 协作式多任务
    sched_init();
    user_create_tesk(); // 创建几个新任务
	task_yield();  // 主动放开当前任务

    uart_puts("Would not go here!\n");
    while(1)
    {
    }
}

