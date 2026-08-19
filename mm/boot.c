// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Boot-phase memory manager.
 *
 * Copyright (c) 2026 Seongjae Kim
 */

#include "boot.h"
#include <fynaos/kernel.h>

phys_addr_t boot_begin;
phys_addr_t boot_tail;
phys_addr_t boot_limit;

void init_boot_alloc(phys_addr_t begin, phys_addr_t limit)
{
    boot_begin = begin;
    boot_tail  = boot_begin;
    boot_limit = limit;
}

void *alloc_boot_memory(size_t len, unsigned int align)
{
    boot_tail = ALIGN_UP(boot_tail, align);

    if (boot_tail + len >= boot_limit)
    {
        panic("failed to allocate boot memory");
    }

    void *addr = (void*)(boot_tail + KERNEL_DIRECT_BASE);

    boot_tail += len;

    return addr;
}

void get_boot_memory_info(phys_addr_t *begin, phys_addr_t *tail)
{
    *begin = boot_begin;
    *tail = boot_tail;
}
