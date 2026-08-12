// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Kernel main initialization.
 *
 * Copyright (c) 2026 Seongjae Kim
 */

#include "fynaos/mm.h"
#include <fynaos/types.h>
#include <fynaos/kernel.h>
#include <fynaos/cpu.h>
#include <fynaos/rtl.h>

#include <fynaos/drivers/fbcon.h>
#include <fynaos/drivers/framebuffer.h>
#include <fynaos/drivers/uart.h>

#include <multiboot2.h>

uint8_t                            multiboot_info_buffer[8192 * 2];
struct multiboot2_info            *multiboot_info_address = (struct multiboot2_info*)multiboot_info_buffer;
struct multiboot2_tag_mmap        *multiboot_mmap_info = NULL;
struct multiboot2_tag_framebuffer *multiboot_framebuffer_info = NULL;

void __noreturn kmain(uint32_t magic, struct multiboot2_info *info);

static void validate_multiboot(uint32_t magic)
{
    if (magic != MULTIBOOT2_LOADER_MAGIC)
    {
        panic("FYNAOS must be loaded by GRUB.\n");
    }
}

static void copy_multiboot_info(struct multiboot2_info *info)
{
    if (info->total_size >= sizeof(multiboot_info_buffer))
    {
        panic("Bootloader information is too big.");
    }
    memcpy(multiboot_info_address, info, info->total_size);
}

static void parse_multiboot_info(void)
{
    for (struct multiboot2_tag_header *i = MULTIBOOT2_FIRST_TAG(multiboot_info_address);
         i->type != MULTIBOOT2_TAG_END;
         i = MULTIBOOT2_NEXT_TAG(i))
    {
        switch (i->type)
        {
        case MULTIBOOT2_TAG_MEMORY_MAP:
            multiboot_mmap_info = (struct multiboot2_tag_mmap*)i;
            break;

        case MULTIBOOT2_TAG_FRAMEBUFFER_INFORMATION:
            multiboot_framebuffer_info = (struct multiboot2_tag_framebuffer*)i;
            break;
        }
    }
}

void __noreturn kmain(uint32_t magic, struct multiboot2_info *info)
{
    uart_init();

    validate_multiboot(magic);
    
    init_gdt();
    init_interrupt();
    
    copy_multiboot_info(info);
    parse_multiboot_info();

    framebuffer_init(multiboot_framebuffer_info);
    fbcon_init(multiboot_framebuffer_info);

    kprintf("FYNAOS is initializing..\n");

    init_memory(multiboot_mmap_info->entries, MULTIBOOT2_MMAP_ENTRY_COUNT(multiboot_mmap_info));

    for (;;) halt_cpu();
}
