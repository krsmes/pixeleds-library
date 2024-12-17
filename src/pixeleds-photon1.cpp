#if (PLATFORM_ID == 6) || (PLATFORM_ID == 8) || (PLATFORM_ID == 10) || (PLATFORM_ID == 88)  // photon, p1, electron

#include "pixeleds-photon1.h"
#include "pinmap_impl.h"

STM32_Pin_Info* BB_PIN_MAP = HAL_Pin_Map();
#define bbPinLO(_pin) (BB_PIN_MAP[_pin].gpio_peripheral->BSRRH = BB_PIN_MAP[_pin].gpio_pin)
#define bbPinHI(_pin) (BB_PIN_MAP[_pin].gpio_peripheral->BSRRL = BB_PIN_MAP[_pin].gpio_pin)


ParticlePixels::ParticlePixels(PixCol *pixels, int pixelCount, byte pin, byte type, byte order) {
    this->pixelCount = pixelCount;
    this->pixels = pixels;
    this->pin = pin;
    this->type = type;
    this->rOfs = order & 3;
    this->gOfs = ((order >> 2) & 3);
    this->bOfs = ((order >> 4) & 3);
    this->refresh = true;
    this->endMicros = 0;
}

ParticlePixels::~ParticlePixels() {
    pinMode(pin, INPUT);
}

void ParticlePixels::setup() {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

void ParticlePixels::update(bool forceRefresh) {
    if (!pixels || (!refresh && !forceRefresh)) return;

    uint32_t wait_micros;
    switch(type) {
        case WS2812B: { wait_micros = 300L; } break;
        case SK6812W: { wait_micros = 80L; } break;
        default:      { wait_micros = 50L; } break;
    }
    while((micros() - endMicros) < wait_micros);

    bool irq = HAL_disable_irq();

    volatile int count = pixelCount;
    volatile PixCol *pPixels = pixels;
    volatile uint32_t color, mask;
    volatile uint8_t bits, r,g,b,w;

    if (type == WS2812B) {
        while (count) {
            count--;
            r = (*pPixels).r;
            g = (*pPixels).g;
            b = (*pPixels++).b;
            color = (uint32_t)r << ((2-rOfs)*8) | (uint32_t)g << ((2-gOfs)*8) | (uint32_t)b << ((2-bOfs)*8);

            mask = 0x800000;
            bits = 0;
            do {
                // base+mov ~50ns, nop ~10ns
                // loop ~100ns (subtracted from LOW)
                // e.g. 500ns HIGH would be (500-50)/10=45 nops
                //      500ns LOW would be (500-150)/10=35 nops

                if (color & mask) {
                    // masked bit is high
                    // WS2812 spec 700ns HIGH (65nop), 600ns LOW (45nop)
                    bbPinHI(pin);
                    asm volatile(
                            "mov r0, r0" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            ::: "r0", "cc", "memory"
                            );
                    bbPinLO(pin);
                    asm volatile(
                            "mov r0, r0" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            :: : "r0", "cc", "memory"
                            );
                } else {
                    // masked bit is low
                    // WS2812 spec 350ns HIGH (30nop), 800ns LOW (65nop)
                    bbPinHI(pin);
                    asm volatile(
                            "mov r0, r0" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            :: : "r0", "cc", "memory"
                            );
                    bbPinLO(pin);
                    asm volatile(
                            "mov r0, r0" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            ::: "r0", "cc", "memory"
                            );
                }
                mask >>= 1;
            } while (++bits < 24); // do all 24 bits
        } // no more pixels
    }
    else if (type == SK6812W) {
        while (count) {
            count--;
            r = (*pPixels).r;
            g = (*pPixels).g;
            b = (*pPixels++).b;
//            w = (*pPixels++).w;
            w = 0x0;
            color = (uint32_t)r << ((3-rOfs)*8) | (uint32_t)g << ((3-gOfs)*8) | (uint32_t)b << ((3-bOfs)*8) | w;

            mask = 0x80000000;
            bits = 0;
            do {
                if (color & mask) {
                    // masked bit is high
                    // SK6812W spec 600ns HIGH (55nop), 600ns LOW (45nop)
                    bbPinHI(pin);
                    asm volatile(
                            "mov r0, r0" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            ::: "r0", "cc", "memory"
                            );
                    bbPinLO(pin);
                    asm volatile(
                            "mov r0, r0" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            ::: "r0", "cc", "memory"
                            );
                }
                else {
                    // masked bit is low
                    // SK6812W spec 300ns HIGH (25nop), 900ns LOW (75nop)
                    bbPinHI(pin);
                    asm volatile(
                            "mov r0, r0" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            ::: "r0", "cc", "memory"
                            );
                    bbPinLO(pin);
                    asm volatile(
                            "mov r0, r0" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                            ::: "r0", "cc", "memory"
                            );
                }
                mask >>= 1;
            } while (++bits < 32 ); // do all 32 bits
        } // no more pixels
    }

    HAL_enable_irq(irq);
    endMicros = micros();
    this->refresh = false;
}

#endif // PLATFORM_ID check