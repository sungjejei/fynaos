// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Kernel main initialization.
 *
 * Copyright (c) 2026 Seongjae Kim
 */

#include <fynaos/kernel.h>
#include <fynaos/cpu.h>
#include <fynaos/types.h>
#include <fynaos/drivers/uart.h>
#include <fynaos/drivers/framebuffer.h>
#include <fynaos/drivers/fbcon.h>
#include <multiboot2.h>

__noreturn void kmain(uint32_t magic, void *info);

static struct multiboot2_tag_framebuffer *get_framebuffer_info(struct multiboot2_info *info)
{
    for (struct multiboot2_tag_header *i = MULTIBOOT2_FIRST_TAG(info);
         i->type != 0;
         i = MULTIBOOT2_NEXT_TAG(i))
    {
        if (i->type == 8) return (struct multiboot2_tag_framebuffer*)i;
    }

    return NULL;
}

__noreturn void kmain(uint32_t magic, void *info)
{
    uart_init();
    init_interrupt();
    
    if (magic != MULTIBOOT2_LOADER_MAGIC)
    {
        panic("boot failed: invalid bootloader: fynaos must be loaded by GRUB.");
    }

    struct multiboot2_tag_framebuffer *framebuffer = get_framebuffer_info(info);
    
    if (framebuffer)
    {
        framebuffer_init(framebuffer);
        fbcon_init(framebuffer);
        panic_use_console();
    }

    init_gdt();

    halt_cpu_forever();
}
