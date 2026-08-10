// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Global Descriptor Table management.
 *
 * Copyright (c) 2026 Seongjae Kim
 */

#include <fynaos/cpu.h>
#include <fynaos/rtl.h>

#define GDTE_ACCESS_KERNEL_CODE 0x9A
#define GDTE_ACCESS_KERNEL_DATA 0x92
#define GDTE_ACCESS_USER_CODE   0xFA
#define GDTE_ACCESS_USER_DATA   0xF2
#define GDTE_ACCESS_TSS         0x89

#define GDTE_FLAG_CODE          0xA
#define GDTE_FLAG_DATA          0xC

#define GDTE_OFFSET_LIMIT_LOW   0
#define GDTE_OFFSET_BASE_LOW    16
#define GDTE_OFFSET_BASE_MIDDLE 32
#define GDTE_OFFSET_ACCESS      40
#define GDTE_OFFSET_LIMIT_HIGH  48
#define GDTE_OFFSET_FLAGS       52
#define GDTE_OFFSET_BASE_HIGH   56

#define MAKE_GDTE(base, limit, access, flags)                                      \
        (((uint64_t)(limit)  & 0x0000FFFFULL) << GDTE_OFFSET_LIMIT_LOW)          | \
        (((uint64_t)(base)   & 0x0000FFFFULL) << GDTE_OFFSET_BASE_LOW)           | \
        (((uint64_t)(base)   & 0x00FF0000ULL) << (GDTE_OFFSET_BASE_MIDDLE - 16)) | \
        (((uint64_t)(access) & 0xFFULL)       << GDTE_OFFSET_ACCESS)             | \
        (((uint64_t)(limit)  & 0x000F0000ULL) << (GDTE_OFFSET_LIMIT_HIGH - 16))  | \
        (((uint64_t)(flags)  & 0xFULL)        << GDTE_OFFSET_FLAGS)              | \
        (((uint64_t)(base)   & 0xFF000000ULL) << (GDTE_OFFSET_BASE_HIGH - 24))

#define MAKE_SYSTEM_GDTE_HIGH(base) (((uint64_t)(base) >> 32) & 0xFFFFFFFFULL)

struct tss {
    uint32_t reserved0;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved1;
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t iomap_base;
} __attribute__((packed));

struct gdtr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

static uint8_t ist1[4096];

static struct tss tss;

static uint64_t gdt[] = {
    0,
    MAKE_GDTE(0, 0xFFFFF, GDTE_ACCESS_KERNEL_CODE, GDTE_FLAG_CODE),
    MAKE_GDTE(0, 0xFFFFF, GDTE_ACCESS_KERNEL_DATA, GDTE_FLAG_DATA),
    MAKE_GDTE(0, 0xFFFFF, GDTE_ACCESS_USER_CODE,   GDTE_FLAG_CODE),
    MAKE_GDTE(0, 0xFFFFF, GDTE_ACCESS_USER_DATA,   GDTE_FLAG_DATA),
    0,
    0
};

void init_gdt(void)
{
    memset(&tss, 0, sizeof(tss));
    tss.rsp0       = 0;
    tss.ist1       = (uint64_t)ist1 + sizeof(ist1);
    tss.iomap_base = 0xFFFF;

    gdt[5] = MAKE_GDTE((uintptr_t)&tss & 0xFFFFFFFF, sizeof(tss) - 1, GDTE_ACCESS_TSS, 0);
    gdt[6] = MAKE_SYSTEM_GDTE_HIGH(&tss);

    struct gdtr gdtr = { sizeof(gdt) - 1, (uint64_t)&gdt };

    __asm__ volatile ("lgdt %0"::"m"(gdtr));

    __asm__ volatile (
        "mov   %0,   %%ax\n\t"
        "mov   %%ax, %%ds\n\t"
        "mov   %%ax, %%ss\n\t"
        "mov   %%ax, %%es\n\t"
        "mov   %%ax, %%fs\n\t"
        "mov   %%ax, %%gs\n\t"
        "pushq %1\n\t"
        "lea   1f(%%rip), %%rax\n\t"
        "pushq %%rax\n\t"
        "lretq\n\t"
        "1:\n\t"
        ::"i"(KERNEL_DATA_SELECTOR),"i"(KERNEL_CODE_SELECTOR)
        : "rax"
    );

    uint16_t tss_sel = MAKE_SEGMENT_SELECTOR(5, 0, 0);

    __asm__ volatile ("ltr %0"::"r"(tss_sel));
}

void set_rsp0(void *rsp0)
{
    tss.rsp0 = (uint64_t)rsp0;
}
