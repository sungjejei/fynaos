// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Framebuffer Console driver.
 *
 * Copyright (c) 2026 Seongjae Kim
 */

#include <fynaos/drivers/fbcon.h>
#include <fynaos/drivers/framebuffer.h>
#include <fynaos/kernel.h>
#include <multiboot2.h>
#include <font8x8_basic.h>

static uint32_t fbcon_x       = 0; /* per text */
static uint32_t fbcon_y       = 0; /* per text */
static uint32_t fbcon_cx      = 0; /* per text */
static uint32_t fbcon_cy      = 0; /* per text */
static uint32_t fbcon_text_cx = 8; /* per pixel */
static uint32_t fbcon_text_cy = 8; /* per pixel */
static uint8_t  fbcon_fg_r    = 255;
static uint8_t  fbcon_fg_g    = 255;
static uint8_t  fbcon_fg_b    = 255;
static uint8_t  fbcon_bg_r    = 0;
static uint8_t  fbcon_bg_g    = 0;
static uint8_t  fbcon_bg_b    = 0;

static void fbcon_draw_ch(char ch, uint32_t x, uint32_t y)
{
    unsigned char uch = (unsigned char)ch;

    for (uint32_t row = 0; row < 8; row++)
    {
        for (uint32_t col = 0; col < 8; col++)
        {
            framebuffer_write(FRAMEBUFFER_RGB(fbcon_bg_r, fbcon_bg_g, fbcon_bg_b), x + col, y + row);
            if (font8x8_basic[uch][row] & (1 << col))
            {
                framebuffer_write(FRAMEBUFFER_RGB(fbcon_fg_r, fbcon_fg_r, fbcon_fg_r), x + col, y + row);
            }
        }
    }
}

int fbcon_write(const char *s)
{
    for (; *s; s++)
    {
        switch (*s)
        {
        case '\n':
            fbcon_x = 0;
            fbcon_y++;
            break;

        case '\t':
            fbcon_write("    ");
            break;

        default:
            fbcon_draw_ch(*s, fbcon_x * fbcon_text_cx, fbcon_y * fbcon_text_cy);
            fbcon_x++;
        }

        if (fbcon_x >= fbcon_cx)
        {
            fbcon_x = 0;
            fbcon_y++;
        }

        if (fbcon_y >= fbcon_cy)
        {
            fbcon_scroll_up();
            fbcon_y--;
        }
    }

    return 0;
}

int fbcon_init(struct multiboot2_tag_framebuffer *fb)
{   
    if (fb->type != 1) return -ENOSYS;

    fbcon_x = 0;
    fbcon_y = 0;
    fbcon_text_cx = 8;
    fbcon_text_cy = 8;
    fbcon_cx = fb->width / fbcon_text_cx;
    fbcon_cy = fb->height / fbcon_text_cy;

    return 0;
}

void fbcon_set_color(uint8_t fg_r, uint8_t fg_g, uint8_t fg_b,
                     uint8_t bg_r, uint8_t bg_g, uint8_t bg_b)
{
    fbcon_fg_r = fg_r;
    fbcon_fg_g = fg_g;
    fbcon_fg_b = fg_b;
    fbcon_bg_r = bg_r;
    fbcon_bg_g = bg_g;
    fbcon_bg_b = bg_b;
}

void fbcon_scroll_up(void)
{
    framebuffer_scroll_up(fbcon_text_cy);
}
