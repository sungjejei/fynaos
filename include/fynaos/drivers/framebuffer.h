// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * framebuffer definitions.
 *
 * Copyright (c) 2026 Seongjae Kim
 */

#ifndef FYNAOS_DRIVERS_FRAMEBUFFER_H
#define FYNAOS_DRIVERS_FRAMEBUFFER_H

#include <fynaos/types.h>
#include <multiboot2.h>

typedef uint32_t framebuffer_color_t; /* 0x00RRGGBB */

#define FRAMEBUFFER_RGB(R, G, B)     (framebuffer_color_t)(((uint8_t)(R) << 16) | ((uint8_t)(G) << 8) | (uint8_t)(B))
#define FRAMEBUFFER_RGB_RED(COLOR)   (uint8_t)(((COLOR) >> 16) & 0xFF)
#define FRAMEBUFFER_RGB_GREEN(COLOR) (uint8_t)(((COLOR) >> 8) & 0xFF)
#define FRAMEBUFFER_RGB_BLUE(COLOR)  (uint8_t)((COLOR) & 0xFF)

int framebuffer_init(struct multiboot2_tag_framebuffer*);
void framebuffer_write(framebuffer_color_t color, unsigned int x, unsigned int y);
void framebuffer_read(framebuffer_color_t *color, unsigned int x, unsigned int y);
void framebuffer_scroll_up(unsigned int scale);

#endif
