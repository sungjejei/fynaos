// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * 16550 UART Driver Code
 *
 * Copyright (c) 2026 Seongjae Kim
 */

#include <fynaos/types.h>
#include <fynaos/cpu.h>
#include <fynaos/drivers/uart.h>

#define COM1 0x3F8
#define COM2 0x2F8
#define COM3 0x3E8
#define COM4 0x2E8

#define COM  COM3

void uart_init(void)
{
    outb(COM + 1, 0x00);
    outb(COM + 3, 0x80);
    outb(COM + 0, 0x01);
    outb(COM + 1, 0x00);
    outb(COM + 3, 0x03);
    outb(COM + 2, 0xC7);
    outb(COM + 4, 0x0B);
}

void uart_write(char ch)
{
    while (!(inb(COM + 5) & 0x20));
    outb(COM, (uint8_t)ch);
}

void uart_write_str(const char *s)
{
    while (*s)
    {
        uart_write(*s++);
    }
}
