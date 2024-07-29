#ifndef __RISCV_H__
#define __RISCV_H__

#include "types.h"

/*
 * ref: https://github.com/mit-pdos/xv6-riscv/blob/riscv/kernel/riscv.h
 */

// 获取线程指针
static inline reg_t r_tp()
{
	reg_t x;
	asm volatile("mv %0, tp" : "=r" (x) );
	return x;
}

/* which hart (core) is this? */
// 获取当前的是那个 hart ，以id的形式获得
static inline reg_t r_mhartid()
{
	reg_t x;
	asm volatile("csrr %0, mhartid" : "=r" (x) );
	return x;
}

/* Machine Status Register, mstatus */
// Machine 状态寄存器，mstatus
// 该寄存器可以设置中断是否使能；
// 以及在发生Trap时，保存关闭中断之前的中断使能状态；
// 因为在 Trap 发生时会转换成 Machine 权限，
// 所以这里会保存在发生 Trap 之前的权限级别

#define MSTATUS_MPP (3 << 11)
#define MSTATUS_SPP (1 << 8)

#define MSTATUS_MPIE (1 << 7)
#define MSTATUS_SPIE (1 << 5)
#define MSTATUS_UPIE (1 << 4)

#define MSTATUS_MIE (1 << 3)
#define MSTATUS_SIE (1 << 1)
#define MSTATUS_UIE (1 << 0)

// 读取当前 Machine状态寄存器
static inline reg_t r_mstatus()
{
	reg_t x;
	asm volatile("csrr %0, mstatus" : "=r" (x) );
	return x;
}

// 设置 Machine状态寄存器
static inline void w_mstatus(reg_t x)
{
	asm volatile("csrw mstatus, %0" : : "r" (x));
}

/*
 * machine exception program counter, holds the
 * instruction address to which a return from
 * exception will go.
 */
// machine 异常程序计数器，保存异常返回的指令地址。

// mepc寄存器，
// 该寄存器用于保存Trap发生时，
// pc指向的当前指令或者下一条指令的地址

// 修改 mepc寄存器，
static inline void w_mepc(reg_t x)
{
	asm volatile("csrw mepc, %0" : : "r" (x));
}

// 获取 mepc寄存器，
static inline reg_t r_mepc()
{
	reg_t x;
	asm volatile("csrr %0, mepc" : "=r" (x));
	return x;
}

/* Machine Scratch register, for early trap handler */
static inline void w_mscratch(reg_t x)
{
	asm volatile("csrw mscratch, %0" : : "r" (x));
}

/* Machine-mode interrupt vector */
// mtvec 寄存器：
// Machine mode 中断的指向地址 寄存器，需要设置

// 写 mtvec 寄存器
static inline void w_mtvec(reg_t x)
{
	asm volatile("csrw mtvec, %0" : : "r" (x));
}



/* Machine-mode Interrupt Enable */
// mie 寄存器：
// 具体的中断使能寄存器
#define MIE_MEIE (1 << 11) // external  外部中断
#define MIE_MTIE (1 << 7)  // timer     定时器中断
#define MIE_MSIE (1 << 3)  // software  软件定义中断

// 读取 mie 寄存器
static inline reg_t r_mie()
{
	reg_t x;
	asm volatile("csrr %0, mie" : "=r" (x) );
	return x;
}

// 设置 mie 寄存器
static inline void w_mie(reg_t x)
{
	asm volatile("csrw mie, %0" : : "r" (x));
}

// mcause 寄存器：
// 用于了解是中断还是异常，
// 是什么样的中断或是什么样的异常

// 读取 mcause 寄存器
static inline reg_t r_mcause()
{
	reg_t x;
	asm volatile("csrr %0, mcause" : "=r" (x) );
	return x;
}

#endif /* __RISCV_H__ */
