# SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Kernel entry point.
 *
 * Copyright (c) 2026 Seongjae Kim
 */

#include <multiboot2.h>

#define KERNEL_CODE_SELECTOR 0x08 /* GDT[1] RPL0 */
#define KERNEL_DATA_SELECTOR 0x10 /* GDT[2] RPL0 */

.section .init.multiboot2

.balign 8
multiboot2_header:
    MAKE_MULTIBOOT2_HEADER

.balign 8
multiboot2_framebuffer:
    .short 5
    .short 0
    .long  20
    .long  800
    .long  600
    .long  0

.balign 8
multiboot2_end:
    .short 0
    .short 0
    .long  8

.section .init.data

.balign 4096
initial_pml4:
    .quad initial_pdpt + 0x03
    .fill 255, 8, 0
    .quad initial_pdpt + 0x03
    .fill 254, 8, 0
    .quad initial_pdpt + 0x03

.balign 4096
initial_pdpt:
.set i, 0
.rept 510
    .quad i * 0x40000000 + 0x83
    .set i, i + 1
.endr
    .quad 0 + 0x83
    .quad 0x40000000 + 0x83

loader_info:
    .long 0

loader_magic:
    .long 0

initial_gdt:
    .quad 0x0000000000000000
    .quad 0x00af9a000000ffff
    .quad 0x00cf92000000ffff

initial_gdtr:
    .short 24 - 1
    .quad initial_gdt

.section .init.text, "ax", @progbits

.code32
.globl _start
_start:
_start_32:
    # Save multiboot informations.
    mov     %eax, (loader_magic)
    mov     %ebx, (loader_info)

    # Load the initial page table.
    mov     $initial_pml4, %eax
    mov     %eax, %cr3

    # Enable PAE.
    mov     %cr4, %eax
    or      $0x20, %eax
    mov     %eax, %cr4

    # Enable IA32_EFER.LME.
    mov     $0xC0000080, %ecx
    rdmsr
    or      $0x100, %eax
    wrmsr

    # Enable CR0.PG.
    mov     %cr0, %eax
    or      $0x80000000, %eax
    mov     %eax, %cr0

    # Load the 64-bit GDT and update the segment registers.
    lgdt    (initial_gdtr)
    mov     $KERNEL_DATA_SELECTOR, %ax
    mov     %ax, %ds
    mov     %ax, %ss
    mov     %ax, %es
    mov     %ax, %fs
    mov     %ax, %gs

    # Jump to the long mode code.
    ljmp    $KERNEL_CODE_SELECTOR, $_start_64

.code64
_start_64:
    mov     $idle_stack, %rsp
    add     $4096, %rsp
    movl    (loader_magic), %edi
    movl    (loader_info), %esi
    movabs  $kmain, %rax
    jmp     *%rax

.bss
.balign 4096
idle_stack:
    .space 4096
