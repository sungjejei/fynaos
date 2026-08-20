// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Memory manager.
 *
 * Copyright (c) 2026 Seongjae Kim
 */

#include <fynaos/mm.h>
#include <fynaos/symbols.h>
#include <fynaos/kernel.h>
#include <multiboot2.h>
#include "boot.h"

void init_memory(struct multiboot2_mmap_entry *entries, size_t count)
{
    /*
     * Initialize the boot memory manager
     */

    phys_addr_t boot_begin = INVALID_ADDRESS;
    phys_addr_t boot_limit = INVALID_ADDRESS;
    size_t len = 0;

    for (size_t i = 0; i < count; i++)
    {
        if (entries[i].len > len && entries[i].type == 1)
        {
            len = entries[i].len;
            boot_begin = entries[i].addr;
            boot_limit = entries[i].addr + len;
        }
    }

    /* There is no usable memory region! */
    if (len == 0) panic("failed to initialize the boot manager");

    /* Does the block contain the kernel image? */
    if (kernel_image_phys_start() >= boot_begin && kernel_image_phys_end() <= boot_limit)
    {
        boot_begin = ALIGN_UP(kernel_image_phys_end(), PAGE_SIZE);
    }

    init_boot_alloc(boot_begin, boot_limit);

    /*
     * Initialize the physical frames
     */

    phys_addr_t ram_limit = 0;

    for (size_t i = 0; i < count; i++)
    {
        phys_addr_t limit = entries[i].addr + entries[i].len;

        if (limit > ram_limit && entries[i].type == 1)
        {
            ram_limit = limit;
        }
    }

    /* There is no usable memory region! */
    if (ram_limit == 0) panic("failed to initialize the physical frames");

    init_frames(ram_limit & PAGE_MASK);

    for (size_t i = 0; i < count; i++)
    {
        if (entries[i].type == 1)
        {
            unreserve_frame_region(
                entries[i].addr >> PAGE_SHIFT,
                (entries[i].addr + entries[i].len) >> PAGE_SHIFT
                );
        }
    }

    /*
     * Initialize the virtual memory manager
     */

    init_kernel_mm(entries, count);

    /*
     * Initialize the physical frame allocator
     */

    for (size_t i = 0; i < count; i++)
    {
        if (entries[i].type != 1) continue;

        phys_addr_t begin = entries[i].addr;
        phys_addr_t limit = begin + entries[i].len;

        /* Does the block contain the kernel image? */
        if (begin <= kernel_image_phys_start() && limit >= kernel_image_phys_end())
        {
            begin = ALIGN_UP(kernel_image_phys_end(), PAGE_SIZE);
        }

        /* Is the block used by the boot allocator? */
        if (begin <= boot_begin && limit >= boot_limit)
        {
            begin = ALIGN_UP(get_boot_memory_tail(), PAGE_SIZE);
        }

        /* Pass the information to the allocator */
        init_buddy_allocator(begin >> PAGE_SHIFT, limit >> PAGE_SHIFT);
    }
}
