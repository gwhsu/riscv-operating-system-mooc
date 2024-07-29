#ifndef _UART_H_
#define _UART_H_


#define RHR 0	// 接收保持寄存器（读取模式）
#define THR 0	// 发送保持寄存器（写入模式）
#define DLL 0	// 除法器锁存器的LSB（写入模式）
#define IER 1	// 中断使能寄存器（写入模式）
#define DLM 1	// 除法器锁存器的MSB（写入模式）
#define FCR 2	// FIFO控制寄存器（写入模式）
#define ISR 2	// 中断状态寄存器（读取模式）
#define LCR 3	// 线路控制寄存器
#define MCR 4	// 调制解调器控制寄存器
#define LSR 5	// 线路状态寄存器
#define MSR 6	// 调制解调器状态寄存器
#define SPR 7	// 高速暂存存储器寄存器

#endif /* _UART_H_ */
