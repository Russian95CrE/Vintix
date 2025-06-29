/* video.c */

#include "font/font.h"
#include "stdbool.h"
#include "stddef.h"
#include "string.h"
#include "video.h"

// Forward declarations
extern void puts (const char* s);
extern void puthex (uint64_t value);
extern void putdec (uint32_t value);

static volatile uint32_t* framebuffer = NULL;
uint32_t                  fb_width    = 0;
uint32_t                  fb_height   = 0;
uint32_t                  fb_pitch    = 0;
uint8_t                   fb_bpp      = 0;

static uint32_t cursor_x = 0;
static uint32_t cursor_y = 0;

// Font dimensions are 8x8
#define FONT_WIDTH 8
#define FONT_HEIGHT 8

bool is_video_ready (void) {
    return framebuffer != NULL;
}

// Convert RGB888 to framebuffer format
static uint32_t rgb_to_fb_color (uint32_t rgb) {
    uint8_t r = (rgb >> 16) & 0xFF;
    uint8_t g = (rgb >> 8) & 0xFF;
    uint8_t b = rgb & 0xFF;

    // The format is 0x00RRGGBB, but the framebuffer expects 0x00BBGGRR
    // or something similar. For simplicity, we'll assume a standard
    // 32-bpp 0xAARRGGBB format, but it's common for framebuffers to
    // use BGR. Let's try BGR.
    return (b << 16) | (g << 8) | r;
}

static bool is_hex_char (char c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
           (c >= 'A' && c <= 'F');
}

uint32_t rgb_to_bgr (uint32_t rgb) {
    uint8_t r = (rgb >> 16) & 0xFF;
    uint8_t g = (rgb >> 8) & 0xFF;
    uint8_t b = rgb & 0xFF;
    return (b << 16) | (g << 8) | r;
}

uint32_t hexstr_to_color (const char* hex) {
    if (hex[0] == '#')
        hex++;
    if (hex[0] == '0' && (hex[1] == 'x' || hex[1] == 'X'))
        hex += 2;

    uint32_t value = 0;
    for (int i = 0; i < 6 && is_hex_char(hex[i]); i++) {
        char c = hex[i];
        value <<= 4;
        if (c >= '0' && c <= '9')
            value |= (uint32_t) (c - '0');
        else if (c >= 'a' && c <= 'f')
            value |= (uint32_t) (c - 'a' + 10);
        else if (c >= 'A' && c <= 'F')
            value |= (uint32_t) (c - 'A' + 10);
    }

    return value;
}

uint32_t hexstr_to_bgr (const char* hex) {
    return rgb_to_bgr(hexstr_to_color(hex));
}

void video_init (struct framebuffer_info* fb_info) {
    if (fb_info == NULL || fb_info->addr == 0) {
        return;
    }

    framebuffer = (volatile uint32_t*) fb_info->addr;
    fb_width    = fb_info->width;
    fb_height   = fb_info->height;
    fb_pitch    = fb_info->pitch;
    fb_bpp      = fb_info->bpp;

    cursor_x = 0;
    cursor_y = 0;
}

void video_put_pixel (uint32_t x, uint32_t y, uint32_t color) {
    if (framebuffer == NULL || x >= fb_width || y >= fb_height) {
        return;
    }

    if (fb_bpp == 16) {
        uint16_t* row = (uint16_t*) ((uint8_t*) framebuffer + y * fb_pitch);
        row[x]        = (uint16_t) rgb_to_fb_color(color);
    } else {
        uint32_t* row = (uint32_t*) ((uint8_t*) framebuffer + y * fb_pitch);
        row[x]        = rgb_to_fb_color(color);
    }
}

void video_clear (uint32_t color) {
    if (framebuffer == NULL) {
        puts("video_clear: framebuffer not initialized");
        return;
    }

    if (fb_width == 0 || fb_height == 0 || fb_pitch == 0) {
        puts("video_clear: invalid framebuffer dimensions");
        return;
    }

    uint32_t fb_color = rgb_to_fb_color(color);

    if (fb_bpp == 16) {
        uint16_t color16 = (uint16_t) fb_color;
        // Clear the first line
        uint16_t* first_row = (uint16_t*) ((uint8_t*) framebuffer);
        for (uint32_t x = 0; x < fb_width; x++) {
            first_row[x] = color16;
        }
        // Copy first line to subsequent lines
        for (uint32_t y = 1; y < fb_height; y++) {
            uint16_t* row = (uint16_t*) ((uint8_t*) framebuffer + y * fb_pitch);
            memcpy(row, first_row, fb_width * sizeof(uint16_t));
        }
    } else if (fb_bpp == 32) {
        // Clear the first line
        uint32_t* first_row = (uint32_t*) ((uint8_t*) framebuffer);
        for (uint32_t x = 0; x < fb_width; x++) {
            first_row[x] = fb_color;
        }
        // Copy first line to subsequent lines
        for (uint32_t y = 1; y < fb_height; y++) {
            uint32_t* row = (uint32_t*) ((uint8_t*) framebuffer + y * fb_pitch);
            memcpy(row, first_row, fb_width * sizeof(uint32_t));
        }
    } else {
        puts("video_clear: unsupported bpp");
        return;
    }
    cursor_x = 0;
    cursor_y = 0;
}

static void
video_draw_glyph_at (char c, uint32_t x, uint32_t y, uint32_t color) {
    if (framebuffer == NULL || (unsigned int) c >= 256) {
        return;
    }

    const unsigned char* glyph = font[(unsigned int) c];

    for (uint32_t row = 0; row < FONT_HEIGHT; row++) {
        unsigned char row_data = glyph[row];
        for (uint32_t col = 0; col < FONT_WIDTH; col++) {
            if (row_data & (0x80 >> col)) {
                video_put_pixel(x + col, y + row, color);
            }
        }
    }
}

void video_putchar (char c) {
    if (c == '\b') {
        if (cursor_x >= FONT_WIDTH) {
            cursor_x -= FONT_WIDTH;
        } else if (cursor_y >= FONT_HEIGHT) {
            cursor_y -= FONT_HEIGHT;
            cursor_x = fb_width - FONT_WIDTH;
        }
        // Erase the character at the current position
        for (uint32_t row = 0; row < FONT_HEIGHT; row++) {
            for (uint32_t col = 0; col < FONT_WIDTH; col++) {
                video_put_pixel(cursor_x + col,
                                cursor_y + row,
                                0x000000); // Black
            }
        }
        return;
    }

    if (c == '\n') {
        cursor_x = 0;
        cursor_y += FONT_HEIGHT;
    } else if (c == '\r') {
        cursor_x = 0;
    } else {
        video_draw_glyph_at(c, cursor_x, cursor_y, 0xFFFFFF); // White
        cursor_x += FONT_WIDTH;
    }

    if (cursor_x + FONT_WIDTH > fb_width) {
        cursor_x = 0;
        cursor_y += FONT_HEIGHT;
    }

    if (cursor_y + FONT_HEIGHT > fb_height) {
        video_clear(0x000000); // For now, just clear screen on scroll
    }
}

void video_draw_circle (int cx, int cy, int radius, uint32_t color) {
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x * x + y * y <= radius * radius) {
                int px = cx + x;
                int py = cy + y;
                if (px >= 0 && py >= 0) {
                    video_put_pixel((uint32_t) px, (uint32_t) py, color);
                }
            }
        }
    }
}

void video_draw_square (int cx, int cy, int size, uint32_t color) {
    int half = size / 2;

    for (int y = cy - half; y < cy + half; y++) {
        for (int x = cx - half; x < cx + half; x++) {
            if (x >= 0 && y >= 0 && x < (int) fb_width && y < (int) fb_height) {
                video_put_pixel((uint32_t) x, (uint32_t) y, color);
            }
        }
    }
}

void video_puts (const char* s) {
    while (*s) {
        video_putchar(*s++);
    }
}