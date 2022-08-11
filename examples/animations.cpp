#include <climits>

#pragma SPARK_NO_PREPROCESSOR
#include "application.h"
#include "pixeleds-library.h"

SYSTEM_THREAD(ENABLED);


// all the predefined animations
PixAniFunc *animations[] = {
        &animation_blink,
        &animation_fadeIn,
        &animation_fadeOut,
        &animation_glow,
        &animation_alternating,
        &animation_strobe,
        &animation_sparkle,
        &animation_cycle,
        &animation_fader,
        &animation_random,
        &animation_increment,
        &animation_decrement,
        &animation_bounce,
        &animation_scanner,
        &animation_comet,
        &animation_bars,
        &animation_gradient,
};

// some custom palettes
PixCol colorsRainbowStripe[] = {
        0xFF0000, 0x000000, 0xAB5500, 0x000000, 0xABAB00, 0x000000, 0x00FF00, 0x000000,
        0x00AB55, 0x000000, 0x0000FF, 0x000000, 0x5500AB, 0x000000, 0xAB0055, 0x000000
};
PixCol colorsMix[] = {
        0x5500AB, 0x84007C, 0xB5004B, 0xE5001B, 0xE81700, 0xB84700, 0xAB7700, 0xABAB00,
        0xAB5500, 0xDD2200, 0xF2000E, 0xC2003E, 0x8F0071, 0x5F00A1, 0x2F00D0, 0x0007F9
};
PixCol colorsYCM[] = {
        PixCol::YELLOW, PixCol::CYAN, PixCol::MAGENTA
};
PixCol colors80s[] = {
        PixCol::MAROON, PixCol::OLIVE, PixCol::GREEN, PixCol::NAVY,
        PixCol::TEAL, PixCol::PURPLE, PixCol::GRAY, PixCol::SILVER
};
PixCol colorsOther[] = {
        PixCol::BROWN, PixCol::SIENNA, PixCol::CORAL, PixCol::ORANGE,
        PixCol::GOLD, PixCol::INDIGO, PixCol::VIOLET, PixCol::PINK
};

// all the palettes (predefined and custom)
PixPal palettes[] = {
        paletteBW,
        paletteRGB,
        paletteRYGB,
        paletteRYGBStripe,
        paletteRainbow,
        { 16, colorsRainbowStripe },
        { 16, colorsMix},
        { 3, colorsYCM },
        { 8, colors80s },
        { 8, colorsOther },
};

PixCol warmColors[] = {PixCol::CRIMSON, PixCol::ORANGE_RED, PixCol::DARK_ORANGE, PixCol::ORANGE_RED};
PixPal warmPal = { 4, warmColors };

/*
 * setup/loop
 */

Pixeleds px = Pixeleds(new PixCol[144] {0}, 144, 0, SK6812W, ORDER_GRB);
//Pixeleds px = Pixeleds(new PixCol[660]{0}, 660, 3, SK6812W);
//int currentAnimation = 0;
//int currentPalette = 0;

void fade_from_black(PixAniData* data) {
    PixCol from = PixCol::BLACK;
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
    //  px.rainbow(500, 1000);  // loop rainbow twice in one second at startup
    //  px.rainbow();
    px.startAnimation(&fade_from_black, &warmPal, 10000, 10000);
}

void loop() {
    px.update(millis());  // this call is where all the work is done
}
