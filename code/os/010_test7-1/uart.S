#include "uart.h"
#include "platform.h"




# 线路状态寄存器
# LSR bit 0：
# 0 = 接收保持寄存器 或 FIFO中无数据。
# 1 = 数据已接收并保存 在 接收保持寄存器或FIFO中。
#  ......
# LSR bit 5：
# 0 = 发送保持寄存器已满。16550将不接受任何用于传输的数据。
# 1 = 发送者保持寄存器（或FIFO）为空。CPU可以加载下一个字符。
.macro LSR_RX_READY DATA
    li \DATA, 0x01  # LSR_RX_READY (1 << 0)
.endm
.macro LSR_TX_IDLE DATA
    li \DATA, 0x20  # LSR_TX_IDLE  (1 << 5)
.endm

# UART控制寄存器被内存映射到地址UART0。此宏返回其中一个寄存器的地址。
# UART0 的地址加上对应寄存器的偏移量，即可设置不同的寄，存器范围（0~7）
## define UART_REG(reg) ((volatile uint8_t *)(UART0 + reg))
# 读写串口寄存器
.macro uart_write_reg REG, DATA     # define uart_write_reg(reg, v) (*(UART_REG(reg)) = (v))
    la s1, UART0
    sb \DATA, \REG(s1)    # 写串口寄存器
.endm

.macro uart_read_reg REG, DATA      # define uart_read_reg(reg) (*(UART_REG(reg)))
    la s1, UART0
    lbu \DATA, \REG(s1)     # 读串口寄存器
.endm

    .global	uart_init   # 引用外部符号（这里是函数）
    .global uart_puts

	.text   # 接下来的代码定向到 text section 


uart_init:
    # 压栈
    addi sp, sp, -12
    sw s0, 0(sp)
    sw s1, 4(sp)
    sw ra, 8(sp)

    li s0, 0x00
    uart_write_reg IER, s0    # uart_write_reg(IER, 0x00);

    uart_read_reg LCR, s0     # uint8_t lcr = uart_read_reg(LCR);

    ori s0, s0, -128
    andi s0, s0, 255
    uart_write_reg LCR, s0    # uart_write_reg(LCR, lcr | 0x80);

    # 设置晶振分频值，分频之后的就是波特率。
    li s0, 0x03
    uart_write_reg DLL, s0    # uart_write_reg(DLL, 0x03);
    li s0, 0x00
    uart_write_reg DLM, s0    # uart_write_reg(DLM, 0x00);

    li s0, 0x00             # lcr = 0;
    ori s0, s0, 0x03
    andi s0, s0, 255
    uart_write_reg LCR, s0    # uart_write_reg(LCR, lcr | 0x03);

    # 出栈
    lw s0, 0(sp)
    lw s1, 4(sp)
    lw ra, 8(sp)
    addi sp, sp, 12
    ret

uart_puts:
    # 压栈
    addi sp, sp, -12
    sw s0, 0(sp)
    sw s1, 4(sp)
    sw ra, 8(sp)

    mv s0, a0
    lbu s1, 0(s0)

    # while(*str)
    # {
    #     uart_putc(*str++);
    # }
    beqz s1, uart_puts_while1_2    # *str == '\0' 不进循环
    uart_puts_while1_1:

    mv a0, s1       # uart_putc(*str++)
    call uart_putc

    addi s0, s0, 1  # str++
    lbu s1, 0(s0)   # *str

    bnez s1, uart_puts_while1_1    # *str != '\0' 继续循环
    uart_puts_while1_2:

    # 出栈
    lw s0, 0(sp)
    lw s1, 4(sp)
    lw ra, 8(sp)
    addi sp, sp, 12
    ret

uart_putc:
    # 压栈
    addi sp, sp, -12
    sw s0, 0(sp)
    sw s1, 4(sp)
    sw ra, 8(sp)

    # 等待可以发送数据
    # while ((uart_read_reg(LSR) & LSR_TX_IDLE) == 0);
    uart_putc_while1:
        uart_read_reg LSR, s0 # uart_read_reg(LSR)
        LSR_TX_IDLE s1     # LSR_TX_IDLE
        and s0, s0, s1      # (uart_read_reg(LSR) & LSR_TX_IDLE)
        andi s0, s0, 255
        beqz s0, uart_putc_while1   # 如果等于零跳转到 uart_putc_while1

    # return uart_write_reg(THR, ch);
    mv s0, a0
    uart_write_reg THR, s0
    mv a0, s0

    # 出栈
    lw s0, 0(sp)
    lw s1, 4(sp)
    lw ra, 8(sp)
    addi sp, sp, 12
    ret

    .end
