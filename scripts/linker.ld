/*
 * SPDX-License-Identifier: GPL-3.0-or-later 
 *
 * Linker script for fxos. 
 *
 * Copyright (C) 2026 Seongjae Kim
 */

ENTRY(_start)
OUTPUT_FORMAT("elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)

SECTIONS
{
    _KERNEL_DIRECT_BASE = 0xFFFF800000000000;
    _KERNEL_IMAGE_BASE = 0xFFFFFFFF80000000;
    
    . = 0x100000;

    _KERNEL_IMAGE_PHYS_START = .;

    . = ALIGN(4096);
    .init : {
        KEEP(*(.init.multiboot2))
        *(.init*)
    }

    . += _KERNEL_IMAGE_BASE;

    . = ALIGN(4096);
    .text : AT(ADDR(.text) - 0xFFFFFFFF80000000) {
        *(.text*)
    }

    . = ALIGN(4096);
    .data : AT(ADDR(.data) - 0xFFFFFFFF80000000) {
        *(.data*)
    }

    . = ALIGN(4096);
    .rodata : AT(ADDR(.rodata) - 0xFFFFFFFF80000000) {
        *(.rodata*)
    }

    . = ALIGN(4096);
    .bss : AT(ADDR(.bss) - 0xFFFFFFFF80000000) {
        *(.bss*)
    }

    _KERNEL_IMAGE_PHYS_END = . - _KERNEL_IMAGE_BASE;
}
