/* rand.c */

#include "stdint.h"

static uint32_t pseudo_rand_state = 0x12345678;

uint32_t rand32 () {
    uint32_t x = pseudo_rand_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    pseudo_rand_state = x;
    return x;
}