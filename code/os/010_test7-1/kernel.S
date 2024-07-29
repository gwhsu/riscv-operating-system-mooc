
    .global	__main  # __main 是 kernel.S 中定义的标号

	.text   # 接下来的代码定向到 text section 

__main:
    call uart_init # 跳转到 uart_init 函数

    la a0, Hello_RVOS   # 获取字符串的首地址
    call uart_puts  # 跳转到 uart_puts 来打印字符串

stop:
    j stop  # 死循环

    
Hello_RVOS: # 要打印的字符串
    .string "Hello, RVOS!\n\0"

    .end

