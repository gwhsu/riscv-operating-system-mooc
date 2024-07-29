#include "os.h"

// 下面的函数只会在这里调用一次，
// 所以只声明了一次，而不包含在文件os.h中。
extern void uart_init(void);

extern void page_init(void);
extern void page_test();

extern void sched_init(void);
extern void schedule(void);
extern void os_main(void);

extern void trap_init(void);
extern void trap_test(void);

extern void plic_init(void);

extern void timer_init(void);
void set_tick(uint32_t t);
uint32_t get_tick();



#define DELAY 1000

void user_task0(void)
{
	while (1) {
		// printf("tick: %d\n", get_tick());
		task_delay(DELAY);
		task_yield();
	}
}

void user_task1(void)
{
	static uint32_t sec1 = 0;
	static uint32_t sec2 = 0;
	static uint32_t min = 0;
	static uint32_t hour = 0;

	set_tick(56);

	while (1) {
		if(sec1 != (get_tick() % 60))
		{
			sec1 = get_tick() % 60;
			sec2 = get_tick() % 60;

			if(sec2 >= 59)
			{
				sec2 = 0;
				min++;
				if(min>=59)
				{
					min = 0;
					hour++;
					if(hour >= 23)
					{
						hour = 0;
					}
				}
			}
			
			printf("                      \r");
			printf("%d:%d:%d\r", hour, min, sec2);
		}

		// task_delay(1);
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

	// trap 测试
	trap_init();

	// 中断
	plic_init();

	// 定时器
	timer_init();

    // 协作式多任务
    sched_init();
    user_create_tesk(); // 创建几个新任务
	task_yield();  // 主动放开当前任务

    uart_puts("Would not go here!\n");
    while(1)
    {
    }
}

