#include "console.h"
#include "common.h"

/*
 * VGA(Video Graphics Array，视频图形阵列)是使用模拟信号的一种视频传输标准，内核可以通过它来控制屏幕上字符或者图形的显示。
 * 在默认的文本模式(Text-Mode)下，VGA控制器保留了一块内存(0x8b000~0x8bfa0)作为屏幕上字符显示的缓冲区，
 * 若要改变屏幕上字符的显示，只需要修改这块内存就好了。
 *
 */

// VGA 的显示缓冲的起点是 0xB8000
static uint16_t *video_memory = (uint16_t *)0xB8000;

// 屏幕光标的坐标
static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;

// 移动光标
static void move_cursor()
{

    // 屏幕是 25 * 80
    uint16_t cursor_location = cursor_y * 80 + cursor_x;

    // 0x3D4 是索引寄存器，用来寻找给定的寄存器
    // 0x3D5 是数据寄存器，用来存放准备写入目标寄存器的数据
    // 14 寄存器表示光标的高8位
    // 15寄存器表示光标的低8位

    outb(0x3D4, 14);                    // 告诉VGA我们准备要设置光标低八位
    outb(0x3D5, cursor_location >> 8);  // 设置高八位
    outb(0x3D4, 15);                    // 告诉VGA我们准备要设置光标低八位
    outb(0x3D5, cursor_location);       // 设置光标低八位

}

// 屏幕滚动操作
/*
    CPU PowerPC、IBM是大端模式，x86是小端模式
*/
static void scroll()
{
    uint8_t attribute_byte = (0 << 4) | (15 & 0x0F);
    uint16_t blank = 0x20 | (attribute_byte << 8); // space ASCII码是0x20;

    if (cursor_y >= 25){
        int i;
        for (i = 0 * 80; i < 24 * 80; i++) {
            video_memory[i] = video_memory[i+80];
        }

        // 最后的一行数据现在填充空格，，不显示任何字符
        for (i = 24 * 80; i < 25 * 80; i++) {
            video_memory[i] = blank;
        }
        cursor_y = 24;
    }
}

// 清屏操作
void console_clear()
{
    uint8_t attribute_byte = (0 << 4) | (15 & 0x0F);
    uint16_t blank = 0x20 | (attribute_byte << 8);

    int i;
    for (i = 0; i < 80 * 25; i++) {
        video_memory[i] = blank;
    }

    cursor_x = 0;
    cursor_y = 0;
    move_cursor();
}

// 向屏幕输出一个带颜色的字符
void console_putc_color(char c, real_color_t back, real_color_t fore)
{
    uint8_t back_color = (uint8_t)back;
    uint8_t fore_color = (uint8_t)fore;

    uint8_t attribute_byte = (back_color << 4) | (fore_color & 0x0F);
    uint16_t attribute = attribute_byte << 8;

    // 0x08 是退格键的ASCII码
    // 0x09 是 tab键的ASCII码
    if (c == 0x08 && cursor_x) {
        cursor_x--;
    }
    /*
    ~(8-1) 表示： 对 tab 占据的空间向下取整
    保证不溢出到下一行，即就算tab占据的空间
    溢出到下一行，cursor_x 的位置依旧从 0 开始
    */
    else if (c == 0x09){
        cursor_x = (cursor_x + 8) & ~(8-1);
    }
    else if (c == '\r'){
        cursor_x = 0;
    }
    else if (c == '\n'){
        cursor_x = 0;
        cursor_y++;
    }
    // 显示字符
    else if (c >= ' '){
        video_memory[cursor_y * 80 + cursor_x] = c | attribute;
        cursor_x++;
    }

    // 屏幕只能显示 25 * 80 大小的空间
    if (cursor_x >= 80){
        cursor_x = 0;
        cursor_y++;
    }

    // 如果需要的话滚动屏幕显示
    scroll();

    // 移动硬件的输入光标
    move_cursor();
}

// 屏幕打印一个 以 \0 结尾的字符串（黑底白字）
void console_write(char *cstr)
{
    while(*cstr){
        console_putc_color(*cstr++,rc_black,rc_white);
    }
}

// 屏幕打印一个以 \0 结尾的字符串（带颜色）
void console_write_color(char *cstr, real_color_t back, real_color_t fore)
{
    while (*cstr) {
        console_putc_color(*cstr++, back, fore);
    }
}

// 屏幕输出一个十六进制的整型数
void console_write_hex(uint32_t n, real_color_t back, real_color_t fore)
{
    int tmp;
    char noZeroes = 1;

    console_write_color("0x", back, fore);

    int i;
    for(i = 28;i >= 0; i-= 4) {
        tmp = (n >> i) & 0xF;
        if(tmp ==0 && noZeroes != 0) {
            continue;
        }
        noZeroes = 0;
        if (tmp >= 0xA) {
            console_putc_color(tmp-0xA+'a', back, fore);
        } else {
            console_putc_color(tmp+'0', back, fore);
        }
    }
}

// 屏幕输出一个十进制的整型数
void console_write_dec(uint32_t n, real_color_t back, real_color_t fore)
{
    if (n == 0) {
        console_putc_color('0', back, fore);
        return;
        }
    uint32_t acc = n;
    char c[32];
    int i = 0;
    /*
    通过不断对 acc 对10取余，得到acc每一位是什么，从低位取到高位，
    此时c[] 是acc的倒序
    i记录acc最大位数
    */
    while (acc > 0){
        c[i] = '0' + acc % 10;
        acc /= 10;
        i++;
    }
    c[i] = 0;

    char c2[32];
    c2[i--] = 0;

    int j = 0;
    while(i >= 0) {
        c2[i--] = c[j++];
    }

    console_write_color(c2, back, fore);
}