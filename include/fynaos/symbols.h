#ifndef FYNAOS_SYMBOLS_H
#define FYNAOS_SYMBOLS_H

#include <fynaos/types.h>

extern unsigned char _KERNEL_IMAGE_PHYS_START;
extern unsigned char _KERNEL_IMAGE_PHYS_END;

static inline __forceinline phys_addr_t kernel_image_phys_start(void)
{
    phys_addr_t addr;
    __asm__ volatile (
        "movabs $_KERNEL_IMAGE_PHYS_START, %0":"=r"(addr)
    );
    return addr;
}

static inline __forceinline phys_addr_t kernel_image_phys_end(void)
{
    phys_addr_t addr;
    __asm__ volatile (
        "movabs $_KERNEL_IMAGE_PHYS_END, %0":"=r"(addr)
    );
    return addr;
}

#endif
