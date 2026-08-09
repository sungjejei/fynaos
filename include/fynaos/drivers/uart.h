// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * UART definitions.
 *
 * Copyright (c) 2026 Seongjae Kim
 */

#ifndef FYNAOS_DRIVERS_UART_H
#define FYNAOS_DRIVERS_UART_H

#include <fynaos/types.h>

void uart_init(void);
void uart_write(char ch);
void uart_write_str(const char *s);

#endif
