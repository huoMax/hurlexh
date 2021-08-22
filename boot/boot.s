; Multiboot 魔数
MBOOT_HEADER_MAGIC equ 0x1BADB002

; 0 号位表示所有的引导模块将按页 (4KB) 边界对齐
MBOOT_PAGE_ALIGN equ 1 << 0

; 1号位通过Multiboot 信息结构的 mem_* 域包括可用内存的信息
; (告诉GRUB把内存空间的信息包括在Multiboot信息结构中)
MBOOT_MEM_INFO equ 1 << 1

; 定义我们使用的 Multiboot 的标记
MBOOT_HEADER_FLAGS equ MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO

; 域checksum是一个32位的无符号值，当与其它的magic域(也就是magic和flags)
; 相加时，要求其结果必须是32位的无符号值 0 (即magic+flags+checksum = 0)
MBOOT_CHECKSUM equ -(MBOOT_HEADER_MAGIC+MBOOT_HEADER_FLAGS)


; 所有代码以32位方式编译
[BITS 32]

; 设置代码段
section .text

; 在代码段的起始位置设置符合MUltiboot 规范的标记
dd MBOOT_HEADER_MAGIC
dd MBOOT_HEADER_FLAGS
dd MBOOT_CHECKSUM

; 向外部声明内核代码入口，这里提供给链接器
[GLOBAL start]

; 向外部声明 struct multiboot * 变量
[GLOBAL glb_mboot_ptr]

; 声明内核 C 代码的入口函数
[EXTERN kern_entry]

; 按照协议，GRUB把一些计算机硬件和内核文件相关信息放在一个结构体中，并且
; 把这个结构体的指针放在 ebx 中
start:
    cli ; 此时还没有设置好保护模式的中断处理，要关闭中断

    mov esp, STACK_TOP ; 设置内核栈地址
    mov ebp, 0 ; 帧指针修改为0
    add esp, 0FFFFFFF0H ; 栈地址按照16字节对齐
    mov [glb_mboot_ptr], ebx ;将 ebx 中存储的指针存入全局变量
    call kern_entry ; 调用内核入口函数
stop:
    hlt ; 停机指令
    jmp stop ; 暂时到这里，以后再处理

section .bss ; 未初始化的数据段从这里开始
stack:
    resb 32768 ; 预留32768的空间作为内核栈
glb_mboot_ptr:
    resb 4 ; 预留4字节给全局multiboot指针

STACK_TOP equ $-stack-1 ; 设置内核栈顶，$ 符号指代当前地址