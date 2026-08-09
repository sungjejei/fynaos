// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Kernel main logging system.
 *
 * Copyright (c) 2026 Seongjae Kim
 */

#include <fynaos/kernel.h>
#include <fynaos/rtl.h>
#include <fynaos/drivers/uart.h>
#include <fynaos/drivers/fbcon.h>

void vkprintf(const char *fmt, va_list args)
{
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    uart_write_str(buffer);
    fbcon_write(buffer);
}

void kprintf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vkprintf(fmt, args);
    va_end(args);
}
