#include <climits>

#pragma SPARK_NO_PREPROCESSOR
#include "application.h"
#include "pixeleds-library.h"

SYSTEM_THREAD(ENABLED);


PixCol warmColors[] = {PixCol::CRIMSON, PixCol::ORANGE_RED, PixCol::DARK_ORANGE, PixCol::ORANGE_RED};
PixPal warmPal = { 4, warmColors };

/*
 * setup/loop
 */

Pixeleds px = Pixeleds(new PixCol[11] {0}, 11, 3, WS2812B, ORDER_GRB); // Photon InternetButton config
//Pixeleds px = Pixeleds(new PixCol[144] {0}, 144, 0, SK6812W, ORDER_GRB); // other LED setups
//Pixeleds px = Pixeleds(new PixCol[660] {0}, 660, 3, SK6812W); // other LED setups

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
    px.startAnimation(&fade_from_black, &warmPal, 10000, 10000);
}

void loop() {
    px.update(millis());  // this call is where all the work is done
}
