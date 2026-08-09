// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * fbcon definitions.
 *
 * Copyright (c) 2026 Seongjae Kim
 */

#ifndef FYNAOS_DRIVERS_FBCON_H
#define FYNAOS_DRIVERS_FBCON_H

#include <fynaos/types.h>

struct multiboot2_tag_framebuffer;

int fbcon_init(struct multiboot2_tag_framebuffer *fb);
int fbcon_write(const char *s);
void fbcon_set_color(uint8_t fg_r, uint8_t fg_g, uint8_t fg_b,
                     uint8_t bg_r, uint8_t bg_g, uint8_t bg_b);
void fbcon_scroll_up(void);

#endif
