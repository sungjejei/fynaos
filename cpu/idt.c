// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Interrupt Descriptor Table management.
 *
 * Copyright (c) 2026 Seongjae Kim
 */

#include <fynaos/cpu.h>

struct idte idt[IDT_COUNT];

void set_idt_entry(unsigned int index, void *isr_addr, uint8_t attributes, uint8_t ist)
{
    struct idte *pidt = &idt[index];

    pidt->attributes  = attributes;
    pidt->ist         = ist;
    pidt->selector    = KERNEL_CODE_SELECTOR;
    pidt->offset_low  = (uintptr_t)isr_addr & 0xFFFF;
    pidt->offset_mid  = ((uintptr_t)isr_addr >> 16) & 0xFFFF;
    pidt->offset_high = ((uintptr_t)isr_addr >> 32) & 0xFFFFFFFF;
    pidt->reserved    = 0;
}

void init_idt(void)
{
    for (unsigned int i = 0; i < IDT_COUNT; i++)
    {
        idt[i].attributes &= ~(uint8_t)IDT_ATTRIBUTE_PRESENT;
    }

    struct idtr idtr = { sizeof(idt) - 1, (uint64_t)idt };

    __asm__ volatile ("lidt %0"::"m"(idtr));
}
