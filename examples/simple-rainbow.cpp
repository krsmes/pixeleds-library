/* 
 * Project simple-rainbow
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
// 60 SK6812W LEDs on pin 0
#define PARTICLE_PIXEL_PIN 0
#define PARTICLE_PIXEL_COUNT 60
#define PARTICLE_PIXEL_TYPE SK6812W
#define PARTICLE_PIXEL_ORDER ORDER_GRBW
#endif

Pixeleds px = Pixeleds(PARTICLE_PIXEL_COUNT, PARTICLE_PIXEL_PIN, PARTICLE_PIXEL_TYPE, PARTICLE_PIXEL_ORDER);


void setup() {
    px.setup();
    // cycle rainbow every 500ms (indefinitely)
    px.startAnimation(&animation_gradient, &Color::RAINBOW, 500); 
}

void loop() {
    px.update(millis());
}
