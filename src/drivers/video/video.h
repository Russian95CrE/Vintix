#pragma once

#include "stdbool.h"
#include "stdint.h"
#include "vintix.h"

extern uint32_t fb_width;
extern uint32_t fb_height;
extern uint32_t fb_pitch;
extern uint8_t  fb_bpp;

// Framebuffer information structure
struct framebuffer_info {
        uint64_t addr;
        uint32_t pitch;
        uint32_t width;
        uint32_t height;
        uint8_t  bpp;
        uint8_t  type;
        uint8_t  red_mask_size;
        uint8_t  red_mask_shift;
        uint8_t  green_mask_size;
        uint8_t  green_mask_shift;
        uint8_t  blue_mask_size;
        uint8_t  blue_mask_shift;
};

void     video_init (struct framebuffer_info* fb_info);
bool     is_video_ready (void);
uint32_t rgb_to_bgr (uint32_t rgb);
uint32_t hexstr_to_color (const char* hex);
uint32_t hexstr_to_bgr (const char* hex);
void     video_put_pixel (uint32_t x, uint32_t y, uint32_t color);
void     video_clear (uint32_t color);

void video_putchar (char c);
void video_puts (const char* s);
void video_draw_circle (int cx, int cy, int radius, uint32_t color);
void video_draw_square (int cx, int cy, int size, uint32_t color);