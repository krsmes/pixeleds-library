/* 
 * Project custom-animation
 * Author: Kevin Smith
 * Date: 2024-12-12
 */
#include "Particle.h"
#include "pixeleds-library.h"
#include "pixeleds-colors.h"

SYSTEM_MODE(SEMI_AUTOMATIC);
SYSTEM_THREAD(ENABLED);

// #define PARTICLE_PHOTON_BUTTON // uncomment to use Photon InternetButton config

#ifdef PARTICLE_PHOTON_BUTTON
#define PARTICLE_PIXEL_PIN 3
#define PARTICLE_PIXEL_COUNT 11
#define PARTICLE_PIXEL_TYPE WS2812B
#define PARTICLE_PIXEL_ORDER ORDER_GRB
#else
// 60 SK6812W LEDs on pin D0
#define PARTICLE_PIXEL_PIN 0
#define PARTICLE_PIXEL_COUNT 60
#define PARTICLE_PIXEL_TYPE SK6812W
#define PARTICLE_PIXEL_ORDER ORDER_GRBW
#endif

PixCol pixels[PARTICLE_PIXEL_COUNT] = {0};
Pixeleds px(pixels, PARTICLE_PIXEL_COUNT, PARTICLE_PIXEL_PIN, PARTICLE_PIXEL_TYPE, PARTICLE_PIXEL_ORDER);

PixPal warmPal = PixPal::create({
    Color::CRIMSON, 
    Color::ORANGE_RED, 
    PixCol::hsv(Hue::ORANGE,1.0,0.3), 
    Color::ORANGE_RED
    });

/**
 * @brief Custom animation function to fade from black
 * 
 * @param data The PixAniData struct containing pixel array, palette, and support functions
 */
void fade_from_black(PixAniData* data) {
    PixCol from = Color::BLACK;
    PixCol target;
    float palpix_ratio = float(data->paletteCount()) / data->pixelCount;
    for (int idx = 0; idx < data->pixelCount; idx++) {
        // pixel target color from palette
        target = data->paletteColor(idx * palpix_ratio);
        data->pixels[idx] = from.interpolate(target, data->cyclePct);
    }
}

void setup() {
    px.setup();
    px.startAnimation(&fade_from_black, &warmPal, 10000, 10000); //run once
}

void loop() {
    px.update(millis()); 
}
