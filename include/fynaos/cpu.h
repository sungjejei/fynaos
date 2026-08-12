// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * CPU definitions.
 *
 * Copyright (c) 2026 Seongjae Kim
 */

#ifndef FYNAOS_CPU_H
#define FYNAOS_CPU_H

#include <fynaos/types.h>

#define MAKE_SEGMENT_SELECTOR(index, ti, rpl) \
        (((index) & 0x1FFF) << 3 | ((ti) & 1) << 2 | ((rpl) & 3))

#define KERNEL_CODE_SELECTOR MAKE_SEGMENT_SELECTOR(1, 0, 0)
#define KERNEL_DATA_SELECTOR MAKE_SEGMENT_SELECTOR(2, 0, 0)
#define USER_CODE_SELECTOR   MAKE_SEGMENT_SELECTOR(3, 0, 3)
#define USER_DATA_SELECTOR   MAKE_SEGMENT_SELECTOR(4, 0, 3)

#define IDT_COUNT                            256U
#define IDT_ATTRIBUTE_PRESENT                0x80U
#define IDT_ATTRIBUTE_PRIVILEGE_KERNELONLY   0x00U
#define IDT_ATTRIBUTE_PRIVILEGE_USERCALLABLE 0x60U
#define IDT_ATTRIBUTE_TYPE_INTERRUPT_GATE    0x0EU
#define IDT_ATTRIBUTE_TYPE_TRAP_GATE         0x0FU
#define IDT_ATTRIBUTE_TYPE_TASK_GATE         0x05U

#define CPU_EXCEPTION_DIVIDE_ERROR           0
#define CPU_EXCEPTION_DEBUG                  1
#define CPU_EXCEPTION_NON_MASKABLE_INTERRUPT 2
#define CPU_EXCEPTION_DEBUG_BREAK            3
#define CPU_EXCEPTION_OVERFLOW               4
#define CPU_EXCEPTION_BOUND_RANGE            5
#define CPU_EXCEPTION_INVALID_OPCODE         6
#define CPU_EXCEPTION_DEVICE_NOT_AVAILABLE   7
#define CPU_EXCEPTION_DOUBLE_FAULT           8
#define CPU_EXCEPTION_INVALID_TSS            10
#define CPU_EXCEPTION_SEGMENT_NOT_PRESENT    11
#define CPU_EXCEPTION_STACK_SEGMENT_FAULT    12
#define CPU_EXCEPTION_GENERAL_PROTECTION     13
#define CPU_EXCEPTION_PAGE_FAULT             14
#define CPU_EXCEPTION_X87_FP                 16
#define CPU_EXCEPTION_ALIGNMENT_CHECK        17
#define CPU_EXCEPTION_MACHINE_CHECK          18
#define CPU_EXCEPTION_SIMD_FP                19

#ifndef __ASSEMBLER__

struct idte {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  ist;
    uint8_t  attributes;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t reserved;
} __attribute__((packed));

struct idtr {
    uint16_t limit;
    uint64_t addr;
} __attribute__((packed));

static inline __forceinline void disable_interrupts(void)
{
    __asm__ volatile ("cli");
}

static inline __forceinline void enable_interrupts(void)
{
    __asm__ volatile ("sti");
}

static inline __forceinline void halt_cpu(void)
{
    __asm__ volatile ("hlt");
}

static inline __forceinline __noreturn void halt_cpu_forever(void)
{
    disable_interrupts();
    for (;;) halt_cpu();
}

static inline __forceinline void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"(val), "Nd"(port)
        : "memory"
    );
}

static inline __forceinline void outw(uint16_t port, uint16_t val)
{
    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"(val), "Nd"(port)
        : "memory"
    );
}

static inline __forceinline uint8_t inb(uint16_t port)
{
    uint8_t val;
    __asm__ volatile (
        "inb %1, %0"
        : "=a"(val)
        : "Nd"(port)
        : "memory"
    );
    return val;
}

static inline __forceinline uint16_t inw(uint16_t port)
{
    uint16_t val;
    __asm__ volatile (
        "inb %1, %0"
        : "=a"(val)
        : "Nd"(port)
        : "memory"
    );
    return val;
}

static inline __forceinline uintptr_t read_cr2(void)
{
    uintptr_t p;

    __asm__ volatile (
        "movq %%cr2, %0"
        :"=r"(p)
        :
        :"memory"
    );

    return p;
}

static inline __forceinline unsigned long save_and_disable_interrupts(void)
{
    unsigned long flags;

    __asm__ volatile (
        "pushfq\n\t"
        "pop %0\n\t"
        "cli\n\t"
        :"=r"(flags)
    );

    return flags;
}

static inline __forceinline void restore_interrupts(unsigned long flags)
{
    __asm__ volatile (
        "push %0\n\t"
        "popfq"
        ::"r"(flags)
    );
}

static inline __forceinline void write_cr3(uintptr_t pml4)
{
    __asm__ volatile ("mov %0, %%cr3"::"r"(pml4));
}

void init_gdt(void);
void set_rsp0(void *stack);

void init_idt(void);
void set_idt_entry(unsigned int index, void *isr_addr, uint8_t attributes, uint8_t ist);

// void init_pic(void);
// void mask_pic_irq(uint8_t irq, boolean_t allow);
// void send_eoi(uint8_t irq);

#endif /* __ASSEMBLER__ */

#endif
