// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Framebuffer provided by GRUB management.
 *
 * Copyright (c) 2026 Seongjae Kim
 */

#include "multiboot2.h"
#include <fynaos/drivers/framebuffer.h>

struct multiboot2_tag_framebuffer framebuffer_info;

int framebuffer_init(struct multiboot2_tag_framebuffer *info)
{
    if (info->type != 1)
    {
        return -ENOSYS;
    }

    framebuffer_info = *info;

    return 0;
}

static uint32_t *pixel(unsigned int x, unsigned int y)
{
    return (uint32_t*)((uintptr_t)framebuffer_info.addr + (y * framebuffer_info.pitch) + (x * 4));
}

void framebuffer_write(framebuffer_color_t color, unsigned int x, unsigned int y)
{
    uint8_t red   = FRAMEBUFFER_RGB_RED(color);
    uint8_t green = FRAMEBUFFER_RGB_GREEN(color);
    uint8_t blue  = FRAMEBUFFER_RGB_BLUE(color);

    uint32_t *address = pixel(x, y);

    *address = (uint32_t)red << framebuffer_info.color_type.red_position |
               (uint32_t)green << framebuffer_info.color_type.green_position |
               (uint32_t)blue << framebuffer_info.color_type.blue_position;
}

void framebuffer_read(framebuffer_color_t *color, unsigned int x, unsigned int y)
{
    uint32_t *address = pixel(x, y);

    uint8_t red = (*address >> framebuffer_info.color_type.red_position) & 0xFF;
    uint8_t green = (*address >> framebuffer_info.color_type.green_position) & 0xFF;
    uint8_t blue = (*address >> framebuffer_info.color_type.blue_position) & 0xFF;
    
    *color = FRAMEBUFFER_RGB(red, green, blue);
}

void framebuffer_scroll_up(unsigned int scale, framebuffer_color_t empty_fill)
{
    for (uint32_t y = 0; y < framebuffer_info.height; y++)
    {
        for (uint32_t x = 0; x < framebuffer_info.width; x++)
        {
            if (y + scale >= framebuffer_info.height)
            {
                framebuffer_write(empty_fill, x, y);
            }
            else
            {
                *pixel(x, y) = *pixel(x, y + scale);
            }
        }
    }
}
