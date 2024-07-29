#include "os.h"

/*
 * ref: https://github.com/cccriscv/mini-riscv-os/blob/master/05-Preemptive/lib.c
 */


// 可变参数是由宏实现的，但是由于硬件平台的不同，编译器的不同，宏的定义也不相同，下面是VC6.0中x86平台的定义 ：
// typedef char * va_list;     // TC中定义为void*

// 对齐，<= 系统对齐字节的，就返回系统对齐字节；>就返回本身的字节数。
// 对于16bit（2Byte）的系统，传进来 1、2Byte 返回2，传进来 4Byte 的就返回4，进来 8Byte -> 8
// 对于32bit（4Byte）的系统，传进来 1、2、4Byte 返回4，传进来 8Byte -> 8
// #define _INTSIZEOF(n)    ((sizeof(n)+sizeof(int)-1)&~(sizeof(int) - 1) )

// 获取第一个变参1的地址
// #define va_start(ap,v)   ( ap = (va_list)&v + _INTSIZEOF(v) )

// 获取当前变参的指针，然后自增指向下一个变参
// #define va_arg(ap,t)     ( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )

// 清空va_list，即结束变参的获取
// #define va_end(ap)       ( ap = (va_list)0 )


/*  // 函数解释
参数：
    参数1：接收新字符串的缓冲区
    参数2：接收新字符串的缓冲区的大小
    参数3：原字符串
    参数4：可变参数列表

内容：
    进入for循环判断，当前字符的前一个字符是否是%（format == 1），
    若是不是，则判断当前字符是否是 %，若不是则把该字符添加到新字符串中，
    若是则令 format=1，一旦（format == 1），
    则下个循环进入if(format == 1)，
    再判断后面跟着的是 l、p、d、x、c、s 中的哪一个，对应把变参内容转成字符串。
    函数结尾再把最后一个位置写入'\0'。

返回值：
    pos = 原字符串长度 + 可变参数扩展的字符串长度
    这个值不会管 接收新字符串的缓冲区 有多大，
    他一律返回 原字符串长度 + 可变参数扩展的字符串长度
*/
static int _vsnprintf(char * out, size_t n, const char* s, va_list vl)
{
	int format = 0;
	int longarg = 0;
	size_t pos = 0;
	for (; *s; s++) {
		if (format) {
			switch(*s) {
			case 'l': {
				longarg = 1;
				break;
			}
			case 'p': {
				longarg = 1;
				if (out && pos < n) {
					out[pos] = '0';
				}
				pos++;
				if (out && pos < n) {
					out[pos] = 'x';
				}
				pos++;
			}
			case 'x': {
				long num = longarg ? va_arg(vl, long) : va_arg(vl, int);
				int hexdigits = 2*(longarg ? sizeof(long) : sizeof(int))-1;
				for(int i = hexdigits; i >= 0; i--) {
					int d = (num >> (4*i)) & 0xF;
					if (out && pos < n) {
						out[pos] = (d < 10 ? '0'+d : 'a'+d-10);
					}
					pos++;
				}
				longarg = 0;
				format = 0;
				break;
			}
			case 'd': {
				long num = longarg ? va_arg(vl, long) : va_arg(vl, int);
				if (num < 0) {
					num = -num;
					if (out && pos < n) {
						out[pos] = '-';
					}
					pos++;
				}
				long digits = 1;
				for (long nn = num; nn /= 10; digits++);
				for (int i = digits-1; i >= 0; i--) {
					if (out && pos + i < n) {
						out[pos + i] = '0' + (num % 10);
					}
					num /= 10;
				}
				pos += digits;
				longarg = 0;
				format = 0;
				break;
			}
			case 's': {
				const char* s2 = va_arg(vl, const char*);
				while (*s2) {
					if (out && pos < n) {
						out[pos] = *s2;
					}
					pos++;
					s2++;
				}
				longarg = 0;
				format = 0;
				break;
			}
			case 'c': {
				if (out && pos < n) {
					out[pos] = (char)va_arg(vl,int);
				}
				pos++;
				longarg = 0;
				format = 0;
				break;
			}
			default:
				break;
			}
		} else if (*s == '%') {
			format = 1;
		} else {
			if (out && pos < n) {
				out[pos] = *s;
			}
			pos++;
		}
    }
	if (out && pos < n) {
		out[pos] = 0;
	} else if (out && n) {
		out[n-1] = 0;
	}
	return pos;
}

// _vprintf() 函数的缓冲区
static char out_buf[1000];

/*  // 函数解释

参数:
    参数1：原字符串
    参数2：可变参数列表

内容：
    先判断缓冲区是否可以承载 原字符串长度 + 可变参数扩展的字符串长度 的大小，
    若是不可以，则发送error信息，并用死循环卡死在这里，
    若是可以，则获取新字符串，然后打印到串口上，

返回值：
    返回新字符串长度。

*/
static int _vprintf(const char* s, va_list vl)
{
	int res = _vsnprintf(NULL, -1, s, vl);
	if (res+1 >= sizeof(out_buf)) {
		uart_puts("error: output string size overflow\n");
		while(1) {}
	}
	_vsnprintf(out_buf, res + 1, s, vl);
	uart_puts(out_buf);
	return res;
}

/*  // 函数解释

参数:
    参数1：原字符串
    参数2：可变参数

内容：
    先获取可变参数列表，然后打印到串口，最后结束变参列表。

返回值：
    返回新字符串长度。

*/
int printf(const char* s, ...)
{
	int res = 0;
	va_list vl;
	va_start(vl, s);
	res = _vprintf(s, vl);
	va_end(vl);
	return res;
}

// 出错了，直接卡死在这里。
void panic(char *s)
{
	printf("panic: ");
	printf(s);
	printf("\n");
	while(1){};
}






