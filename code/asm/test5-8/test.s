 .text
    .global _start


_start:
    la sp, stack_end # 获取栈顶地址

    li a0, 3    # 参数赋值
    call sum_squares    # 跳转到sum_squares函数

stop:
    j stop  # endless loop


sum_squares:
    # 压栈两个寄存器数据
    addi sp, sp, -16
    sw s0, 0(sp)
    sw s1, 4(sp)
    sw s2, 8(sp)
    sw ra, 12(sp)


    mv s2, a0   # i = 3
    li s0, 0    # sum = 0
    li s1, 1    # j = 1
    # for (int j = 1; j <= i; j++)
    bgtu s1, s2, for1_end    # 一旦j>i，则直接退出
for1:
    mv a0, s1 # 把j赋值给形参i
    call square    # sum += square(j)
    add s0, s0, a0

    addi s1, s1, 1  # j++
    bleu s1, s2, for1   # j<=i，进入下一轮循环
for1_end:

    mv a0, s0

    # 出栈两个寄存器数据
    lw s0, 0(sp)
    lw s1, 4(sp)
    lw s2, 8(sp)
    lw ra, 12(sp)
    addi sp, sp, 16

    ret

square:
    addi sp, sp, -4
    sw s0, 0(sp)

    mv s0, a0
    
    # return i*i
    mul a0, s0, s0
    
    lw s0, 0(sp)
    addi sp, sp, 4

    ret



	# allocate stack space
stack_start:
	# 12*4 Byte = 48 Byte
	.rept 12
	.word 0
	.endr
stack_end:

    .end
