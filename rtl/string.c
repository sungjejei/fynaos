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
