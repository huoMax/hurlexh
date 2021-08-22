#include "types.h"

int kern_entry()
{
    uint8_t *input = (uint8_t *)0xB8000;
    uint8_t color = (0 << 4) | (15 & 0x0F);

    *input++ = "T"; *input++ = color;
    *input++ = "h"; *input++ = color;
    *input++ = "i"; *input++ = color;
    *input++ = "s"; *input++ = color;
    *input++ = " "; *input++ = color;
    *input++ = "t"; *input++ = color;
    *input++ = "h"; *input++ = color;
    *input++ = "e"; *input++ = color;
    *input++ = " "; *input++ = color;
    *input++ = "f"; *input++ = color;
    *input++ = "i"; *input++ = color;
    *input++ = "r"; *input++ = color;
    *input++ = "s"; *input++ = color;
    *input++ = "t"; *input++ = color;
    *input++ = " "; *input++ = color;
    *input++ = "d"; *input++ = color;
    *input++ = "e"; *input++ = color;
    *input++ = "m"; *input++ = color;
    *input++ = "o"; *input++ = color;
    *input++ = "!"; *input++ = color;
}