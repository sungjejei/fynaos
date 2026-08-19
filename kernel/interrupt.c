// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Kernel interrupt handling.
 *
 * Copyright (c) 2026 Seongjae Kim
 */

#include <fynaos/kernel.h>
#include <fynaos/cpu.h>
#include <fynaos/mm.h>

#define SET_IDTE(I, ATTR, IST) set_idt_entry((I), isr_table[(I)], ATTR, IST)
void __noreturn interrupt_service(struct trap_frame *frame);
extern void *isr_table[IDT_COUNT];

struct interrupt_handler_info {
    interrupt_handler_t handler;
    struct interrupt_handler_info *next;
};

struct interrupt_handler_info *interrupt_handlers[256] = { NULL };

void __noreturn interrupt_service(struct trap_frame *frame)
{
    struct interrupt_handler_info *head = interrupt_handlers[frame->vector];

    if (frame->vector < 32)
    {
        panic("Unhandled exception: vector=%llu", frame->vector);
    }

    while (head)
    {
        if (!head->handler(frame)) exit_interrupt_service(frame);
        head = head->next;
    }

    panic("Unhandled interrupt: vector=%llu", frame->vector);
}

void init_interrupt(void)
{
    init_idt();

    uint8_t attr = IDT_ATTRIBUTE_PRESENT | IDT_ATTRIBUTE_PRIVILEGE_KERNELONLY |
                   IDT_ATTRIBUTE_TYPE_INTERRUPT_GATE;

    for (unsigned int i = 0; i < IDT_COUNT; i++)
    {
        SET_IDTE(i, attr, 0);
    }

    SET_IDTE(CPU_EXCEPTION_NON_MASKABLE_INTERRUPT, attr, 1);
    SET_IDTE(CPU_EXCEPTION_DOUBLE_FAULT, attr, 1);
    SET_IDTE(CPU_EXCEPTION_MACHINE_CHECK, attr, 1);
}

int connect_interrupt(interrupt_handler_t handler, unsigned int number)
{
    if (number < 32)
    {
        return -EINVAL;
    }

    unsigned long flags = save_and_disable_interrupts();
    struct interrupt_handler_info *new_node = kmalloc(sizeof(struct interrupt_handler_info));

    if (!new_node)
    {
        restore_interrupts(flags);
        return -ENOMEM;
    }

    new_node->handler = handler;
    new_node->next = interrupt_handlers[number];

    interrupt_handlers[number] = new_node;

    restore_interrupts(flags);
    return 0;
}
