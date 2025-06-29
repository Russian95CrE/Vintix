/* sleep.c */

#include "signal.h"
#include "sleep.h"
#include "stddef.h"
#include "stdint.h"

volatile uint64_t* hpet = (volatile uint64_t*) 0xFED00000;

#define HPET_GEN_CONF        0x10
#define HPET_MAIN_COUNTER    0xF0
#define HPET_CAP_ID          0x0
#define HPET_GEN_CONF_ENABLE (1 << 0)

#define PIT_CHANNEL0 0x40
#define PIT_COMMAND  0x43

void hpet_enable () {
        if ((hpet[HPET_GEN_CONF / 8] & HPET_GEN_CONF_ENABLE) == 0) {
                hpet[HPET_GEN_CONF / 8] |= HPET_GEN_CONF_ENABLE;
        }
}

// PIT busy-wait ~1ms per tick
void pit_wait (int ms) {
        outb(PIT_COMMAND, 0x34);

        uint16_t reload = 1193; // ~1 ms (1193182 Hz / 1000)
        outb(PIT_CHANNEL0, (uint8_t) (reload & 0xFF));
        outb(PIT_CHANNEL0, (uint8_t) ((reload >> 8) & 0xFF));

        for (int i = 0; i < ms; i++) {
                uint8_t prev = 0xFF, curr = 0;
                do {
                        curr = inb(PIT_CHANNEL0);
                        if (curr > prev)
                                break;
                        prev = curr;
                } while (curr != 0);
        }
}

void sleep (int ms) {
        if (hpet != NULL) {
                hpet_enable();

                uint64_t period_fs = hpet[HPET_CAP_ID / 8] >> 32;
                uint64_t start     = hpet[HPET_MAIN_COUNTER / 8];

                uint64_t ticks =
                    (uint64_t) ms * 1000000000ULL / (period_fs / 1000);

                while ((hpet[HPET_MAIN_COUNTER / 8] - start) < ticks) {
                        cpu_relax();
                }
        } else {
                pit_wait(ms);
        }
}
