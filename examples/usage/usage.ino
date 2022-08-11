#include <climits>

#pragma SPARK_NO_PREPROCESSOR
#include "application.h"
#include "Pixeleds.h"

SYSTEM_THREAD(ENABLED);

/**
 * This usage example uses the Particle's Photon InternetButton to show how to create pallates,
 * custom animations, and input from cloud functions to controls the LEDs using the Pixeleds class
 */

/*
 * pallates
 */

PixPal offPal = { 1, new PixCol[1] {PixCol::OFF} };
PixPal whitePal = { 1, new PixCol[1] {PixCol::LINEN} };
PixPal dimPal = { 1, new PixCol[1] {PixCol::scale(PixCol::LINEN, 0.1f)} };
PixPal darkPal = { 1, new PixCol[1] {PixCol::scale(PixCol::BEIGE, 0.02f)} };

PixPal hotPal = { 4, new PixCol[4] {PixCol::CRIMSON, PixCol::ORANGE_RED, PixCol::DARK_ORANGE, PixCol::ORANGE_RED} };
PixPal warmPal = { 4, new PixCol[4] {PixCol::GREEN_YELLOW, PixCol::ORANGE_RED, PixCol::DARK_ORANGE, PixCol::ORANGE_RED} };
PixPal bluePal = { 4, new PixCol[4] {PixCol::DARK_TURQUOISE, PixCol::CYAN, PixCol::TEAL, PixCol::CYAN} };

PixPal christmasPal { 16, new PixCol[16] {
        PixCol::BLACK, PixCol::GREEN, PixCol::BLACK, PixCol::RED,
        PixCol::BLACK, PixCol::GREEN, PixCol::BLACK, PixCol::RED,
        PixCol::BLACK, PixCol::GREEN, PixCol::BLACK, PixCol::RED,
        PixCol::BLACK, PixCol::GREEN, PixCol::BLACK, PixCol::RED} };

PixPal customPal = { 1, new PixCol[1] {PixCol::WHITE} };

PixPal *pal = &offPal;

/*
 * custom animation
 */

void fade_from_black(PixAniData* data) {
    PixCol from = PixCol::BLACK;
    PixCol target;
    // ratio of palette size to pixel count
    // e.g. pallet of 4 colors with a pixel count of 20 is .2,
    //      so for every pixel you take 20% of the way between two pallete colors
    float palpix_ratio = float(data->paletteCount()) / (data->pixelCount - 1);
    // loop through each pixel
    for (int idx = 0; idx < data->pixelCount; idx++) {
        // choose computed target color from palette based on current pixel index
        target = data->paletteColor(idx * palpix_ratio);
        // set the pixel to a color between black and the target color based on pct of current cycle
        data->pixels[idx] = from.interpolate(target, data->cyclePct);
    }
}

void fade_alternating(PixAniData* data) {
    Serial.printlnf("fade_alternating: updated=%ld, cycleCount=%ld, cycleMillis=%ld, cyclePct=%f",
                    data->updated, data->cycleCount, data->cycleMillis, data->cyclePct);
    PixCol from = PixCol::BLACK;
    PixCol target;
    // ratio of palette size to pixel count
    // e.g. pallet of 4 colors with a pixel count of 20 is .2,
    //      so for every pixel you take 20% of the way between two pallete colors
    float palpix_ratio = float(data->paletteCount()) / (data->pixelCount - 1);
    // loop through each pixel
    for (int idx = 0; idx < data->pixelCount; idx++) {
        // choose computed target color from palette based on current pixel index
        target = data->paletteColor(idx * palpix_ratio);
        // set the pixel to a color between black and the target color based on pct of current cycle
        // alternate every other pixel on / off (idx % 2)
        // fade alternates in / out (data->cycleCount % 2, 1.0 - data->cyclePct, data->cyclePct)
        data->pixels[idx] = (idx % 2 == data->cycleCount % 2)
                            ? from.interpolate(target, 1.0 - data->cyclePct)
                            : from.interpolate(target, data->cyclePct);
        Serial.printf("  %6lX", data->pixels[idx].rgb());
    }
    Serial.printlnf("");
}


/*
 * device LED configuration
 */

Pixeleds px = Pixeleds(new PixCol[11] {0}, 11, 3, WS2812B, ORDER_GRB); // Photon InternetButton config
// Pixeleds px = Pixeleds(new PixCol[144] {0}, 144, 0, SK6812W, ORDER_GRB); // other LED setups



/*
 * setup/loop
 */

int setPalette(String command);
//int setCycle(String command);
//int setAnimation(String command);
int setCustom(String command);

void setup() {
    Particle.function("palette", setPalette);
    // Particle.function("cycle", setCycle);
    // Particle.function("animation", setAnimation);
    Particle.function("custom", setCustom);
    Serial.begin(115200);
    px.setup();
    px.setAnimationRefresh(1000/2);

    setPalette("white");
}

void loop() {
    px.update(millis());  // this call is where all the work is done
}


/*
 * cloud functions
 */

int setPalette(String command) {
    if (command == "rainbow") {
        px.rainbow();
        return 1;
    }
    else if (command == "christmas"){ pal = &christmasPal; }
    else if (command == "custom")   { pal = &customPal; }
    else if (command == "off")      { pal = &offPal; }
    else if (command == "white")    { pal = &whitePal; }
    else if (command == "dim")      { pal = &dimPal; }
    else if (command == "dark")     { pal = &darkPal; }
    else if (command == "hot")      { pal = &hotPal; }
    else if (command == "warm")     { pal = &warmPal; }
    else if (command == "blue")     { pal = &bluePal; }
    else return -1;
    px.startAnimation(&fade_alternating, pal, 30000);
    return 0;
}

int setCustom(String command) {
    unsigned int r, g, b;
    sscanf(command, "%x %x %x", &r, &g, &b);
    customPal.colors[0] = PixCol((byte)r, (byte)g, (byte)b);
    px.startAnimation(&fade_alternating, &customPal, 30000);
    return 0;
}