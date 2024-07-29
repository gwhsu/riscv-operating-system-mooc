#include "os.h"

/* interval ~= 1s */
#define TIMER_INTERVAL CLINT_TIMEBASE_FREQ

static uint32_t _tick = 0;

/* 为下一个计时器中断加载计时器间隔（以节拍为单位）。*/
void timer_load(int interval)
{
	/* 每个CPU都有一个单独的定时器中断源。 */
	int id = r_mhartid();
	
	*(uint64_t*)CLINT_MTIMECMP(id) = *(uint64_t*)CLINT_MTIME + interval;
}

void timer_init()
{
	/*
	 * On reset, mtime is cleared to zero, but the mtimecmp registers 
	 * are not reset. So we have to init the mtimecmp manually.
	 */
	timer_load(TIMER_INTERVAL);

	/* enable machine-mode timer interrupts. */
	w_mie(r_mie() | MIE_MTIE);

	/* enable machine-mode global interrupts. */
	w_mstatus(r_mstatus() | MSTATUS_MIE);
}

void timer_handler()
{
	_tick++;
	// printf("tick: %d\n", _tick);

	timer_load(TIMER_INTERVAL);
}

void set_tick(uint32_t t)
{
	_tick = t;
}

uint32_t get_tick()
{
	return _tick;
}


