# SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Assembly routines used by the kernel.
 *
 * Copyright (c) 2026 Seongjae Kim
 */

.altmacro

/*
 * This macro generates an ISR routine.
 */
.macro MAKE_ISR num
isr\num\():
.if \num != 8  && \num != 10 && \num != 11 && \num != 12 && \num != 13 && \
    \num != 14 && \num != 17 && \num != 21 && \num != 29 && \num != 30
    push $0
.endif
    push $\num
    jmp isr_common
.endm

/*
 * This macro generates an address of the ISR.
 */
.macro MAKE_ISR_ADDR num
    .quad isr\num
.endm

/*
 * Generate the 256 ISRs.
 */
.text
.set i, 0
.rept 256
MAKE_ISR %i
.set i, i + 1
.endr

/*
 * Generate the ISR table.
 */
.data
.globl isr_table
.set i, 0
isr_table:
.rept 256
MAKE_ISR_ADDR %i
.set i, i+1
.endr

.text

/*
 * This routine pushes all registers and jumps to the
 * C interrupt routine.
 */
isr_common:
    push    %rbp
    push    %rdi
    push    %rsi
    push    %rdx
    push    %rcx
    push    %rbx
    push    %rax
    push    %r8
    push    %r9
    push    %r10
    push    %r11
    push    %r12
    push    %r13
    push    %r14
    push    %r15
    mov     %rsp, %rdi
    jmp     interrupt_service

/*
 * __noreturn void exit_interrupt_service(struct trap_frame *frame);
 *
 * This routine completes the interrupt service routine
 * and returns to the interrupted routine.
 */
.globl exit_interrupt_service
exit_interrupt_service:
    mov     %rdi, %rsp
    pop     %r15
    pop     %r14
    pop     %r13
    pop     %r12
    pop     %r11
    pop     %r10
    pop     %r9
    pop     %r8
    pop     %rax
    pop     %rbx
    pop     %rcx
    pop     %rdx
    pop     %rsi
    pop     %rdi
    pop     %rbp
    add     $16, %rsp
    iretq
