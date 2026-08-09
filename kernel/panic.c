// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Kernel panic handling.
 *
 * Copyright (c) 2026 Seongjae Kim
 */

#include <fynaos/cpu.h>
#include <fynaos/kernel.h>
#include <fynaos/drivers/uart.h>
#include <fynaos/drivers/fbcon.h>
#include <fynaos/rtl.h>

boolean_t panic_use_fbcon = FALSE;
boolean_t in_panic        = FALSE;

__noreturn void vpanic(const char *msg, va_list args)
{
    disable_interrupts();

    if (in_panic)
    {
        uart_write_str("*** Double panic detected\n");
        halt_cpu_forever();
    }
    else
    {
        in_panic = TRUE;
    }

    static char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), msg, args);

    uart_write_str("kernel panic: ");
    uart_write_str(buffer);
    uart_write('\n');

    if (panic_use_fbcon)
    {
        fbcon_write("kernel panic: ");
        fbcon_write(buffer);
        fbcon_write("\n");
    }

    halt_cpu_forever();
}

__noreturn void panic(const char *msg, ...)
{
    va_list args;
    va_start(args, msg);
    vpanic(msg, args);
}

void panic_use_console(void)
{
    panic_use_fbcon = TRUE;
}
