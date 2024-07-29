# 保存所有 General-Purpose(GP) 寄存器到上下文中
# struct context *base = &ctx_task;
# base->ra = ra;
# ......
.macro reg_save base
	sw ra, 0(\base)
	sw sp, 4(\base)
	sw gp, 8(\base)
	sw tp, 12(\base)
	sw t0, 16(\base)
	sw t1, 20(\base)
	sw t2, 24(\base)
	sw s0, 28(\base)
	sw s1, 32(\base)
	sw a0, 36(\base)
	sw a1, 40(\base)
	sw a2, 44(\base)
	sw a3, 48(\base)
	sw a4, 52(\base)
	sw a5, 56(\base)
	sw a6, 60(\base)
	sw a7, 64(\base)
	sw s2, 68(\base)
	sw s3, 72(\base)
	sw s4, 76(\base)
	sw s5, 80(\base)
	sw s6, 84(\base)
	sw s7, 88(\base)
	sw s8, 92(\base)
	sw s9, 96(\base)
	sw s10, 100(\base)
	sw s11, 104(\base)
	sw t3, 108(\base)
	sw t4, 112(\base)
	sw t5, 116(\base)
    # 这里没有将 t6 保存，因为这里的 宏变量base的值，是来自外面的t6
.endm

# 恢复所有 General-Purpose(GP) 寄存器到上下文中
# struct context *base = &ctx_task;
# ra = base->ra;
# ......
.macro reg_restore base
	lw ra, 0(\base)
	lw sp, 4(\base)
	lw gp, 8(\base)
	lw tp, 12(\base)
	lw t0, 16(\base)
	lw t1, 20(\base)
	lw t2, 24(\base)
	lw s0, 28(\base)
	lw s1, 32(\base)
	lw a0, 36(\base)
	lw a1, 40(\base)
	lw a2, 44(\base)
	lw a3, 48(\base)
	lw a4, 52(\base)
	lw a5, 56(\base)
	lw a6, 60(\base)
	lw a7, 64(\base)
	lw s2, 68(\base)
	lw s3, 72(\base)
	lw s4, 76(\base)
	lw s5, 80(\base)
	lw s6, 84(\base)
	lw s7, 88(\base)
	lw s8, 92(\base)
	lw s9, 96(\base)
	lw s10, 100(\base)
	lw s11, 104(\base)
	lw t3, 108(\base)
	lw t4, 112(\base)
	lw t5, 116(\base)
	lw t6, 120(\base)
.endm
#有关 save/restore 的一些注意事项：
# -我们使用 mscratch 保存指向上一任务上下文的指针。
# 我们使用 t6 作为 reg_save/reg_store 的 “base”，
# 因为它是最底层的寄存器（x31），在加载过程中不会被覆盖。


.text

# 在 machine 模式下出现 中断或异常
.globl trap_vector
# trap vector base address 必须始终与4字节边界对齐,
# 因为寄存器的低2位是设置模式的，
# 所以设置基地址的时候没有不与4对齐的具体地址，所以必须与4字节对齐
.align 4
trap_vector:
	# 保存上下文（寄存器）。
	csrrw	t6, mscratch, t6	# 交换 t6 and mscratch
	reg_save t6

	# 保存实际的t6寄存器，我们将其交换到mscratch中
	mv	t5, t6		# t5指向当前任务的上下文
	csrr	t6, mscratch	# 从mscratch读取t6
	sw	t6, 120(t5)	# 以t5为基础保存t6

	# 将上下文指针还原为mscratch
	csrw	mscratch, t5

	# 调用 trap.c 中的 C语言的 trap handler
	csrr	a0, mepc	# 把trap发生的地址给 trap_handler函数第1个参数
	csrr	a1, mcause	# 把trap的种类给 trap_handler函数第2个参数
	call	trap_handler # 调用处理函数

	# trap_handler 将通过a0返回返回地址。
	csrw	mepc, a0

	# 恢复上下文(registers).
	csrr	t6, mscratch
	reg_restore t6

	# 回到 trap 前我们所做的一切。
	mret

# void switch_to(struct context *next);
# a0 : 指向下一任务上下文的指针
.global switch_to
.align 4
switch_to:
	csrrw	t6, mscratch, t6	# 交换 t6 和 mscratch
    beqz    t6, 1f      # 上一个任务可能为空，也就是说现在是第一次调用该函数
	reg_save t6			# 保存上一个任务的上下文

    # 保存我们交换到的实际t6寄存器
    # mscratch
    mv t5, t6   # t5指向当前任务的上下文
    csrr t6, mscratch   # 读取t6的值，给到mscratch
    sw	t6, 120(t5) # 以t5为base保存t6

1:
	# 切换mscratch以指向下一个任务的上下文
	csrw	mscratch, a0

    # 还原所有GP寄存器
    # 使用t6指向新任务的上下文
	mv	t6, a0
	reg_restore t6

    # 进行实际的上下文切换。
	ret


.end
