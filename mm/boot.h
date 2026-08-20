// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Boot-phase memory manager definitions.
 *
 * Copyright (c) 2026 Seongjae Kim
 */

#ifndef FYNAOS_MM_VMM_H
#define FYNAOS_MM_VMM_H

#include <fynaos/mm.h>

void init_boot_alloc(phys_addr_t begin, phys_addr_t limit);
void *alloc_boot_memory(size_t len, unsigned int align);
phys_addr_t get_boot_memory_tail(void);

#endif
