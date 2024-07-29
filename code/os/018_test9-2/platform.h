#ifndef _PLATFOEM_H_
#define _PLATFOEM_H_

// QEMU RISC-V Virt machine with 16550a UART and VirtIO MMIO

// maximum number of CPUs
// see https://github.com/qemu/qemu/blob/master/include/hw/riscv/virt.h
// #define VIRT_CPUS_MAX 8

#define MAXNUM_CPU 8

// MemoryMap
// see https://github.com/qemu/qemu/blob/master/hw/riscv/virt.c, virt_memmap[] 
// 0x00001000 -- boot ROM, provided by qemu
// 0x02000000 -- CLINT
// 0x0C000000 -- PLIC
// 0x10000000 -- UART0
// 0x10001000 -- virtio disk
// 0x80000000 -- boot ROM jumps here in machine mode, where we load our kernel

// 这个虚拟的板子把 uart0 的起始地址放在 0x10000000 的物理地址上
#define UART0 0x10000000L









#endif /* _PLATFOEM_H_ */
