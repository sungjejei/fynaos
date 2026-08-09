// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Kernel interrupt handling.
 *
 * Copyright (c) 2026 Seongjae Kim
 */

#include <fynaos/kernel.h>
#include <fynaos/cpu.h>

extern void *isr_table[IDT_COUNT];

__noreturn void interrupt_service(struct trap_frame *frame);
__noreturn void exit_interrupt_service(struct trap_frame *frame);
__noreturn void debug_break_exception(struct trap_frame *frame);
__noreturn void nmi_exception(struct trap_frame *frame);

__noreturn void interrupt_service(struct trap_frame *frame)
{
    switch (frame->vector)
    {
    case 3:
        debug_break_exception(frame);

    case 2:
        nmi_exception(frame);

    default:
        panic("unhandled exception: %llu", frame->vector);

    }
}

static void print_frame(struct trap_frame *frame)
{
    kprintf(
        "    rip=0x%016X\n"
        "    rax=0x%016X rbx=0x%016X rcx=0x%016X rdx=0x%016X\n"
        "    r8=0x%016X  r9=0x%016X  r10=0x%016X r11=0x%016X\n"
        "    r12=0x%016X r13=0x%016X r14=0x%016X r15=0x%016X\n"
        "    rsi=0x%016X rdi=0x%016X rsp=0x%016X rbp=0x%016X\n",
        frame->rip,
        frame->rax, frame->rbx, frame->rcx, frame->rdx,
        frame->r8, frame->r9, frame->r10, frame->r11,
        frame->r12, frame->r13, frame->r14, frame->r15,
        frame->rsi, frame->rdi, (uintptr_t)frame - sizeof(struct trap_frame), frame->rbp
        );

}

__noreturn void debug_break_exception(struct trap_frame *frame)
{
    kprintf("debug break occured:\n");
    print_frame(frame);

    exit_interrupt_service(frame);
}

__noreturn void nmi_exception(struct trap_frame *frame)
{
    kprintf("non maskable interrupt occured:\n");
    print_frame(frame);

    uint8_t reason = inb(0x61);

    if (reason & 0x80)
    {
        kprintf("RAM Parity Check Error!\n");
        panic("nmi: ram parity check error");
    }
    else if (reason & 0x40)
    {
        kprintf("I/O Channel Error!\n");
        panic("nmi: I/O channel error");
    }
    else
    {
        kprintf("NMI occured but no error detected.\nTrying to continue.\n");
        exit_interrupt_service(frame);
    }
}

void init_interrupt(void)
{
    init_idt();

    for (unsigned int i = 0; i < IDT_COUNT; i++)
    {
        set_idt_entry(
            i,
            isr_table[i],
            IDT_ATTRIBUTE_PRESENT |
            IDT_ATTRIBUTE_PRIVILEGE_KERNELONLY |
            IDT_ATTRIBUTE_TYPE_INTERRUPT_GATE,
            0
            );
    }

    set_idt_entry(
        3,
        isr_table[3],
        IDT_ATTRIBUTE_PRESENT |
        IDT_ATTRIBUTE_PRIVILEGE_KERNELONLY |
        IDT_ATTRIBUTE_TYPE_INTERRUPT_GATE,
        1
    );
}
