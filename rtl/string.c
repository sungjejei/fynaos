// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * String functions implements.
 *
 * Copyright (c) 2026 Seongjae Kim
 */

#include <fynaos/rtl.h>

void *memset(void *dst, int val, size_t len)
{
    uint8_t *d = dst;
    while (len--)
    {
        *d++ = (uint8_t)val;
    }
    return dst;
}

void *memcpy(void *dst, void *src, size_t c)
{
    for (size_t i = 0; i < c; i++)
    {
        ((uint8_t*)dst)[i] = ((uint8_t*)src)[i];
    }

    return dst;
}
