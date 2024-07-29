#include "os.h"

extern void trap_vector(void);
extern void uart_isr(void);
extern void timer_handler(void);


void trap_init()
{
    // 设置 trap-vector 基地址 给 machine 特权模式
    w_mtvec((reg_t)trap_vector);
}

void external_interrupt_handler()
{
	// 获取是哪种外部中断，并清除 Pending 标志位。
	int irq = plic_claim();

	// 是串口中断
	if(irq == UART0_IRQ)
	{
		// 调用串口回调函数
		uart_isr();
	}
	// 其他中断
	else if(irq)
	{
		printf("unexpected interrupt irq = %d\n", irq);
	}

	// 中断处理完，清除complete标志位
	if(irq)
	{
		plic_complete(irq);
	}

}

// trap 回调函数
reg_t trap_handler(reg_t epc, reg_t cause)
{
    reg_t return_pc = epc;
    reg_t cause_code = cause & 0xff;

	if (cause & 0x80000000) {
		/* Asynchronous trap - interrupt */
        // 这里的 异步trap 是中断
		switch (cause_code) {
		case 3:
			uart_puts("software interruption!\n");
			break;
		case 7:
			// uart_puts("timer interruption!\n");
			timer_handler();
			break;
		case 11:
			uart_puts("external interruption!\n");
			external_interrupt_handler();
			break;
		default:
			uart_puts("unknown async exception!\n");
			break;
		}
	} else {
		/* Synchronous trap - exception */
        // 这里的 同步trap 是异常
		printf("Sync exceptions!, code = %d\n", cause_code);
		panic("OOPS! What can I do!");
		// return_pc += 4;
	}

    return return_pc;
}


void trap_test()
{
	/*
	 * Synchronous exception code = 7
	 * Store/AMO access fault
	 */
	*(int *)0x00000000 = 100;

	/*
	 * Synchronous exception code = 5
	 * Load access fault
	 */
	//int a = *(int *)0x00000000;

	uart_puts("Yeah! I'm return back from trap!\n");
}

