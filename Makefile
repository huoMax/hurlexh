#!Makefile

C_SOURCES = $(shell find . -name "*.c")
C_OBJECTS = $(patsubst %.c, %.o, $(C_SOURCES))
S_SOURCES = $(shell find . -name "*.s")
S_OBJECTS = $(patsubst %.s, %.o, $(S_SOURCES))

CC = gcc
LD = ld
ASM = nasm

# gcc 编译参数          含义
# -m32 					生成32位代码
# -ggdb 和 -gstabs+ 	添加相关的调试信息
# -nostdinc 			不包含C语言的标准库的头文件
# -fno-builtin  		要求gcc不主动使用自己的内建函数，除非显式声明
# -fno-stack-protector  不使用栈保护等检测

# ld链接命令参数		 含义
# -T scripts/kernel.ld  使用我们自己定义的链接脚本
# -m elf_i386			生成i386平台下elf格式的可执行文件

C_FLAGS = -c -Wall -m32 -ggdb -gstabs+ -nostdinc -fno-pic -fno-builtin -fno-stack-protector -I include
LD_FLAGS = -T scripts/kernel.ld -m elf_i386 -nostdlib
ASM_FLAGS = -f elf -g -F stabs

all: $(S_OBJECTS) $(C_OBJECTS) link update_image

.c.o:
	@echo 编译代码文件 $< ...
	$(CC) $(C_FLAGS) $< -o $@

.s.o:
	@echo 编译汇编文件 $< ...
	$(ASM) $(ASM_FLAGS) $<

link:
	@echo 链接内核文件 ...
	$(LD) $(LD_FLAGS) $(S_OBJECTS) $(C_OBJECTS) -o hxh_kernel

.PHONY:clean
clean:
	$(RM) $(S_OBJECTS) $(C_OBJECTS) hxh_kernel

.PHONY:update_image
update_image:
	sudo mount floppy.img .

.PHONY:mount_image
mount_image:
	sudo mount floppy.img .

.PHONY:umount_image
umount_image:
	sudo umount .

.PHONY:qemu
qemu:
	qemu -fda floppy.img -boot a

bochs:
	bochs -f tools/bochsrc.txt

.PHONY:debug
debug:
	qemu -S -s -fda floppy.img -boot a &
	sleep 1
	cgdb -x tools/gdbinit