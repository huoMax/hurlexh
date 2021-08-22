/* 声明入口函数地址 该函数定义在 boot.s 中*/
/* 
备注 ：【语法】
每个段之后都要加一个空格再加冒号
 */
ENTRY(start)
SECTIONS
{
    /* 声明段的起始位置 . 表示当前链接位置 */
    . = 0x100000;

    /* 设置所有文件的代码段 .text 位置，然后重新设置链接地址，使用ALIGN使地址对齐 */ 
    .text :
    {
        *(.text)
        . = ALIGN(4096);
    }
    /* 设置所有文件的数据段 .data .rodata 位置，然后重新设置链接地址，使用ALIGN使地址对齐 */ 
    .data :
    {
        *(.data)
        *(.rodata)
        . = ALIGN(4096);
    }
    /* 设置所有文件的bss段 .bss 位置，然后重新设置链接地址，使用ALIGN使地址对齐 */
    .bss :
    {
        *(.bss)
        . = ALIGN(4096);
    }
    /* 设置所有文件的段表 .stab 位置，然后重新设置链接地址，使用ALIGN使地址对齐 */
    .stab :
    {
        *(.stab)
        . = ALIGN(4096);
    }
    /* 设置所有文件的符号表 .stabstr 位置，然后重新设置链接地址，使用ALIGN使地址对齐 */
    .stabstr :
    {
        *(.stabstr)
        . = ALIGN(4096);
    }

    /* 丢弃comment 段和 eh_frame 段 */
    /DISCARD/ : { *(.comment) *(.eh_frame) }
}