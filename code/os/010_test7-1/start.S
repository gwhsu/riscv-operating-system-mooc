#include "platform.h"

	# 每个 hart 堆栈的大小为1024字节
	.equ	STACK_SIZE, 1024

	.global	_start

	.text

_start:
    # 排除 id 不为0的 hart
    csrr t0, mhartid # 获取 hart_id
    mv tp, t0        # 将CPU的 hart_id 保存在 tp 中，以备以后使用。
    bnez t0, park    # 如果不是 hart0，就将该 hart 停下来

    # 设置堆栈时，堆栈从下到上增长，因此我们将堆栈指针放在堆栈范围的最末端。
    # 将 hart_id（0~7） 乘上 1024
    slli t0, t0, 10
    # 将初始堆栈指针，设置为第一个堆栈空间的末尾
    la sp, stacks + STACK_SIZE
    # 将当前hart堆栈指针，移动到堆栈空间中的位置
    add sp, sp, t0

    j __main  # hart0 跳转到 主函数

stop:
    j stop


park:
    wfi # 休眠
    j park

stacks:
    # 为所有 hart 堆栈分配空间
	.skip	STACK_SIZE * MAXNUM_CPU


    .end
