// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Runtime library definitions.
 *
 * Copyright (c) 2026 Seongjae Kim
 */

#ifndef FYNAOS_RTL_H
#define FYNAOS_RTL_H

#include <fynaos/types.h>
#include <stdarg.h>

void *memset(void *dst, int val, size_t len);
size_t vsnprintf(char *buffer, size_t length, const char *format, va_list args);
size_t snprintf(char *buffer, size_t len, const char *format, ...);
void *memcpy(void *dst, void *src, size_t c);

#endif
