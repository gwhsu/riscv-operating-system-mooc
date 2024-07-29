#include "os.h"



// 定义来自 entry.S
extern void switch_to(struct context *next);

#define MAX_TASKS 10
#define STACK_SIZE 1024
uint8_t task_stack[MAX_TASKS][STACK_SIZE];
struct context ctx_tasks[MAX_TASKS];

/*
 * _top 用于存储有多少个任务
 * _current 用于指向当前任务的上下文
 */
static int _top = 0;
static int _current = -1;


void sched_init()
{
    w_mscratch(0);
}

// 一种简单循环FIFO调度程序的实现
void schedule()
{
    if(_top <= 0)
    {
        panic("Num of task should be greater than zero!");
        return;
    }

    // 获取下一个该调度的任务
    _current = (_current + 1) % _top;
    struct context *next = &(ctx_tasks[_current]);
    switch_to(next);
}


/*
 * DESCRIPTION
 * 	新建一个任务
 * 	- start_routin: 任务回调函数
 * RETURN VALUE
 * 	0: success
 * 	-1: if error occured
 */
int task_create(void (*start_routin)(void))
{
    // 判断添加的任务数量是否超过限制的数量
    if(_top < MAX_TASKS)
    {
        ctx_tasks[_top].sp = (reg_t)&task_stack[_top][STACK_SIZE - 1];
        ctx_tasks[_top].ra = (reg_t)start_routin;
        _top++;
        return 0;
    }
    else
    {
		return -1;
	}
}

/*
 * DESCRIPTION
 * 	task_yield()  
 * 	使当前任务放开CPU，并运行新任务。
 */
void task_yield()
{
	schedule();
}

/*
 * a very rough implementaion, just to consume the cpu
 * 软件的阻塞延时，只是为了卡住cpu
 */
void task_delay(volatile int count)
{
	count *= 100000;
	while (count--);
}
