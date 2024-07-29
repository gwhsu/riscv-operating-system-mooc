#include "types.h"
#include "platform.h"

#include <stdarg.h>

// UART控制寄存器被内存映射到地址UART0。此宏返回其中一个寄存器的地址。
// UART0 的地址加上对应寄存器的偏移量，即可设置不同的寄，存器范围（0~7）
#define UART_REG(reg) ((volatile uint8_t *)(UART0 + reg))

// UART控制寄存器映射。参见[1] "PROGRAMMING TABLE"
// 注意，有些被多个函数重用
// 0（写入模式）：THR/DLL
// 1（写入模式）：IER/DLM
// 每个地址可以访问8个字节
typedef enum {
    RHR = 0,    // 接收保持寄存器（读取模式）
    THR = 0,    // 发送保持寄存器（写入模式）
    DLL = 0,    // 除法器锁存器的LSB（写入模式）
    IER = 1,    // 中断使能寄存器（写入模式）
    DLM = 1,    // 除法器锁存器的MSB（写入模式）
    FCR = 2,    // FIFO控制寄存器（写入模式）
    ISR = 2,    // 中断状态寄存器（读取模式）
    LCR = 3,    // 线路控制寄存器
    MCR = 4,    // 调制解调器控制寄存器
    LSR = 5,    // 线路状态寄存器
    MSR = 6,    // 调制解调器状态寄存器
    SPR = 7     // 高速暂存存储器寄存器
}UART0_REGISTER;

/*
 * 上电默认值
 * IER = 0: TX/RX 保持寄存器中断均被禁用
 * ISR = 1: 无中断暂停
 * LCR = 0
 * MCR = 0
 * LSR = 60 HEX
 * MSR = BITS 0-3 = 0, BITS 4-7 = inputs
 * FCR = 0
 * TX = High
 * OP1 = High
 * OP2 = High
 * RTS = High
 * DTR = High
 * RXRDY = High
 * TXRDY = Low
 * INT = Low
 */

// 线路状态寄存器
// LSR bit 0：
// 0 = 接收保持寄存器 或 FIFO中无数据。
// 1 = 数据已接收并保存 在 接收保持寄存器或FIFO中。
//  ......
// LSR bit 5：
// 0 = 发送保持寄存器已满。16550将不接受任何用于传输的数据。
// 1 = 发送者保持寄存器（或FIFO）为空。CPU可以加载下一个字符。
#define LSR_RX_READY (1 << 0)
#define LSR_TX_IDLE  (1 << 5)


// 读写串口寄存器
#define uart_read_reg(reg) (*(UART_REG(reg)))
#define uart_write_reg(reg, v) (*(UART_REG(reg)) = (v))



void uart_init()
{
    // 失能中断
    uart_write_reg(IER, 0x00);

    // 设置波特率。如果我们关心除数，这里只是一个演示，
    // 但对于我们的目的[QEMU-virt]，这实际上没有什么作用。
    //
    // 请注意，除数寄存器DLL（最小除数锁存器）和DLM（最大除数锁紧器）
    // 具有与接收器/发送器和中断使能寄存器相同的基址。
    // 为了改变基址指向的内容，我们通过将1写入除数锁存访问位（DLAB）
    // 来打开“除数锁”，该位是行控制寄存器（LCR）的位索引7。
    // 
    // 关于波特率值，请参见[1]“波特率生成器编程表”。
    // 当1.8432 MHZ晶体时，我们使用38.4K，因此对应的值是3。
    // 由于除数寄存器是两个字节（16位），因此我们需要将3（0x0003）的值
    // 拆分为两个字节，DLL存储低字节，DLM存储高字节。

    // 获取当前LCR寄存器的值
    uint8_t lcr = uart_read_reg(LCR);

    // 设置LCR寄存器，除数锁存器使能
    uart_write_reg(LCR, lcr | 0x80);

    // 设置晶振分频值，分频之后的就是波特率。
    uart_write_reg(DLL, 0x03);
    uart_write_reg(DLM, 0x00);

    // 继续设置异步数据通信格式。
    // -字长数：8位
    // -停止位数：字长为8位时为1位
    // -无奇偶校验
    // -无中断控制
    // -禁用波特锁存器
    lcr = 0;
    uart_write_reg(LCR, lcr | 0x03);
}

// 等待发送一个字符，返回发送的字符
int uart_putc(char ch)
{
    // 等待可以发送数据
    while ((uart_read_reg(LSR) & LSR_TX_IDLE) == 0);
    return uart_write_reg(THR, ch);
}

// 发送一个字符串
void uart_puts(char *s)
{
    while(*s)
    {
        uart_putc(*s++);
    }
}


// 等待接收一个字符，返回接收的字符
uint8_t uart_getc(void)
{
    // 等待接收到数据
    while ((uart_read_reg(LSR) & LSR_RX_READY) == 0);
    return uart_read_reg(RHR);
}




