// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * kernel definitions.
 *
 * Copyright (c) 2026 Seongjae Kim
 */

#ifndef FYNAOS_KERNEL_H
#define FYNAOS_KERNEL_H

#include <fynaos/types.h>
#include <stdarg.h>

struct trap_frame {
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rbp;
    uint64_t vector;
    uint64_t errcode;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
} __attribute__((packed));

/*
 * The context saves only callee-saved registers.
 */
struct context {
    uint64_t rbx;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
    uint64_t rsp;
    uint64_t rbp;
} __attribute__((packed));

__noreturn void panic(const char *msg, ...);
__noreturn void vpanic(const char *msg, va_list args);
void panic_use_console(void);

void kprintf(const char *fmt, ...);
void vkprintf(const char *fmt, va_list args);

void init_interrupt(void);

struct context *swap_context(struct context *current, struct context *next);

#endif
