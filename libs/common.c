#include "common.h"

/*
dN 约束表示使用edx寄存器且对 port 端口限制在 0——255
=a 约束表示 使用 eax寄存器
asm volatile 是 gcc 内联汇编语法，使用AT&T的汇编语法
机器指令的最后一个字符表示操作数的位数:
    inb : 表示读取一个字节
    inw ：表示读取两个字节
    inl : 表示读取四个字节
*/

// 向端口写一个字节
inline void outb(uint16_t port, uint8_t value)
{
    asm volatile("outb %1, %0" : : "dN"(port),"a"(value));
}

// 向端口读一个字节
inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "dN"(port));
    return ret;
}

// 向端口读两个字节
inline uint16_t inw(uint16_t)
{
    uint16_t ret;

    asm volatile("inw %1, %0" : "=a"(ret) : "dN" (port));

    return ret;
}