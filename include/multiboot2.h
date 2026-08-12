// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * GRUB Multiboot2 definitions.
 *
 * Copyright (c) 2026 Seongjae Kim
 */

#ifndef MULTIBOOT2_H
#define MULTIBOOT2_H

#define MULTIBOOT2_HEADER_MAGIC 0xE85250D6
#define MULTIBOOT2_ARCH_I386    0
#define MULTIBOOT2_HEADER_LEN   12
#define MULTIBOOT2_CHKSUM       -(0xE85250D6+12) & 0xFFFFFFFF

#define MULTIBOOT2_LOADER_MAGIC 0x36D76289

#ifdef __ASSEMBLER__
.macro MAKE_MULTIBOOT2_HEADER
    .long MULTIBOOT2_HEADER_MAGIC
    .long MULTIBOOT2_ARCH_I386
    .long MULTIBOOT2_HEADER_LEN
    .long MULTIBOOT2_CHKSUM
.endm
#endif /* __ASSEMBLER__ */

#ifndef __ASSEMBLER__

#include <fynaos/types.h>

struct multiboot2_info {
    uint32_t total_size;
    uint32_t reserved;
};

struct multiboot2_tag_header {
    uint32_t type;
    uint32_t size;
};

struct multiboot2_tag_framebuffer {
    struct multiboot2_tag_header header;
    uint64_t addr;
    uint32_t pitch;
    uint32_t width;
    uint32_t height;
    uint8_t bpp;
    uint8_t type;
    uint16_t reserved;
    struct {
        uint8_t red_position;
        uint8_t red_mask;
        uint8_t green_position;
        uint8_t green_mask;
        uint8_t blue_position;
        uint8_t blue_mask;
    } color_type;
} __attribute__((packed));

struct multiboot2_mmap_entry {
    uint64_t addr;
    uint64_t len;
    uint32_t type;
    uint32_t reserved;
};

struct multiboot2_tag_mmap {
    struct multiboot2_tag_header header;
    uint32_t entry_size;
    uint32_t entry_version;
    struct multiboot2_mmap_entry entries[];
};

#define MULTIBOOT2_FIRST_TAG(pINFO) \
        ((struct multiboot2_tag_header*)pINFO + 1)

#define MULTIBOOT2_NEXT_TAG(PTAG_HEADER) \
        (struct multiboot2_tag_header*)(ALIGN_UP((uintptr_t)PTAG_HEADER + PTAG_HEADER->size, 8))

#define MULTIBOOT2_MMAP_ENTRY_COUNT(MMAP) \
        (size_t)((MMAP)->header.size - sizeof(struct multiboot2_tag_header) / (MMAP)->entry_size)

#endif /* ASSEMBLER */

#define MULTIBOOT2_TAG_END                              0
#define MULTIBOOT2_TAG_BOOT_COMMANDLINE                 1
#define MULTIBOOT2_TAG_BOOT_LOADER_NAME                 2
#define MULTIBOOT2_TAG_MODULES                          3
#define MULTIBOOT2_TAG_BASIC_MEMORY_INFORMATION         4
#define MULTIBOOT2_TAG_BIOS_BOOT_DEVICE                 5
#define MULTIBOOT2_TAG_MEMORY_MAP                       6
#define MULTIBOOT2_TAG_VBE_INFORMATION                  7
#define MULTIBOOT2_TAG_FRAMEBUFFER_INFORMATION          8
#define MULTIBOOT2_TAG_ELF_SYMBOLS                      9
#define MULTIBOOT2_TAG_APM_TABLE                        10
#define MULTIBOOT2_TAG_EFI_SYSTEM_TABLE_32              11
#define MULTIBOOT2_TAG_EFI_SYSTEM_TABLE_64              12
#define MULTIBOOT2_TAG_SMBIOS_TABLES                    13
#define MULTIBOOT2_TAG_ACPI_RSDP_V1                     14
#define MULTIBOOT2_TAG_ACPI_RSDP_V2PLUS                 15
#define MULTIBOOT2_TAG_NETWORKING_INFORMATION           16
#define MULTIBOOT2_TAG_EFI_MEMORY_MAP                   17
#define MULTIBOOT2_TAG_EFI_BOOT_SERVICES_NOT_TERMINATED 18
#define MULTIBOOT2_TAG_EFI_32_IMAGE_HANDLE              19
#define MULTIBOOT2_TAG_EFI_64_IMAGE_HANDLE              20
#define MULTIBOOT2_TAG_IMAGE_LOAD_BASE_ADDRESS          21

#endif /* MULTIBOOT2_H */
