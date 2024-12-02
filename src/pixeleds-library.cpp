#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-integer-division"
#pragma ide diagnostic ignored "bugprone-narrowing-conversions"
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
#pragma ide diagnostic ignored "cert-err58-cpp"

/*
Copyright 2022 The Brynwood Team, LLC

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "neopixel.h"
#include "pixeleds-library.h"
#include <cmath>


/*
 * constructors/destructors
 */

Pixeleds::Pixeleds(PixCol* pixels, int pixelCount, byte pixelPin, byte type, byte order) {
    pixelStrip = new ParticlePixels(pixels, pixelCount, pixelPin, type, order);
    animationData.pixels = pixels;
    animationData.pixelCount = pixelCount;
    setAnimationRefresh();
}

/*
 * public api
 */

void Pixeleds::setup() {
    pixelStrip->setup();
}

void Pixeleds::update(system_tick_t millis) {
    updateAnimation(millis);
    pixelStrip->update();
}

void __unused Pixeleds::setPixel(int pixel, byte r, byte g, byte b) {
    setPixel(pixel, PixCol(r, g, b));
}

void __unused Pixeleds::setPixel(int pixel, PixCol color) {
    animationFunction = nullptr;
    pixelStrip->setPixelColor(pixel, color);
}

void __unused Pixeleds::setPixels(byte r, byte g, byte b) {
setPixels(PixCol(r, g, b));
}

void __unused Pixeleds::setPixels(PixCol color) {
animationFunction = nullptr;
for (int idx = 0; idx < animationData.pixelCount; idx++) { pixelStrip->setPixelColor(idx, color); }
}

void __unused Pixeleds::setPixels(PixCol* colors, int count) {
animationFunction = nullptr;
for (int idx = 0; idx < animationData.pixelCount; idx++) { pixelStrip->setPixelColor(idx, idx < count ? colors[idx] : 0); }
}

void __unused Pixeleds::updatePixel(int pixel, PixCol color) {
    pixelStrip->setPixelColor(pixel, color);
    pixelStrip->update(true);
}

void __unused Pixeleds::updatePixels(PixCol color) {
for (int idx = 0; idx < animationData.pixelCount; idx++) { pixelStrip->setPixelColor(idx, color); }
pixelStrip->update(true);
}

void __unused Pixeleds::updatePixels(PixCol* colors, int count) {
for (int idx = 0; idx < animationData.pixelCount; idx++) { pixelStrip->setPixelColor(idx, idx < count ? colors[idx] : 0); }
pixelStrip->update(true);
}

PixAniData* __unused Pixeleds::startAnimation(PixAniFunc *animation, PixPal *palette,
long cycle, long duration, int data) {
animationFunction = (duration != 0) ? animation : nullptr; // duration 0, only fire once (no updates)
animationData.palette = palette;
animationData.cycleDuration = cycle > 0 ? cycle : 1; // can't be zero or negative
animationData.start = millis();
animationData.stop = animationData.start + duration;
animationData.data = data;
animationData.updated = animationData.start;  // detect first fire when these are equal
animationData.cycleMillis = 0;
animationData.cycleCount = 0;
animationData.cyclePct = 0.0;
animation(&animationData); // first fire
pixelStrip->triggerRefresh();
return &animationData;
}

void __unused Pixeleds::setAnimationRefresh(int refresh) {
    animationRefresh = refresh;
}

bool __unused Pixeleds::isAnimationActive() {
    return (bool) (*animationFunction);
}

void __unused Pixeleds::rainbow(long cycle, long duration) {
    startAnimation(&animation_gradient, &paletteRainbow, cycle, duration);
}

/*
 * private helpers
 */

void Pixeleds::updateAnimation(system_tick_t millis) {
    if ((*animationFunction) && (millis > animationData.updated + animationRefresh)) {
#ifdef PIXELEDS_SERIAL_DEBUG
        Serial.printlnf("updateAnimation: %ld", millis);
#endif
        if (animationData.stop > animationData.start && millis > animationData.stop) {
            animationFunction = nullptr;
//            setPixels(0);
        }
        else {
            animationData.updated = millis;
            long millisSinceStart = millis - animationData.start;
            animationData.cycleMillis = millisSinceStart % animationData.cycleDuration;
            animationData.cycleCount = millisSinceStart / animationData.cycleDuration;
            animationData.cyclePct = (float)animationData.cycleMillis / (float)animationData.cycleDuration;
#ifdef PIXELEDS_SERIAL_DEBUG
            Serial.printlnf("updateAnimation: millis=%d, count=%d, pct=%f", animationData.cycleMillis, animationData.cycleCount, animationData.cyclePct);
#endif
            animationFunction(&animationData);
            pixelStrip->triggerRefresh();
        }
    }
}



/*
 * constructors/destructors
 */

ParticlePixels::ParticlePixels(PixCol *pixels, int pixelCount, byte pin, byte type, byte order) {
    this->pixels = pixels;
    this->pixelCount = pixelCount;

    // Convert our type to Neopixel type
//    uint8_t pixelType = (type == SK6812W) ? SK6812RGBW : WS2812B;
    strip = new Adafruit_NeoPixel(pixelCount, pin, type);
//    strip = new Adafruit_NeoPixel(pixelCount, pin, SK6812RGBW);
}

ParticlePixels::~ParticlePixels() {
    delete strip;
}

/*
 * public api
 */

void ParticlePixels::setup() {
    strip->begin();
    strip->show(); // Initialize all pixels to 'off'
}

void ParticlePixels::update(bool doRefresh) {
    if (!this->refresh && !doRefresh) return;
    // Convert our PixCol array to Neopixel colors and update strip
    for(int i = 0; i < pixelCount; i++) {
        strip->setPixelColor(i, pixels[i].r, pixels[i].g, pixels[i].b, 0);
    }
    strip->show();
    this->refresh = false;
}

void ParticlePixels::setPixelColor(int pixel, PixCol pixelColor) {
    if (pixel < pixelCount) {
        pixels[pixel] = pixelColor;
        refresh = true;
    }
}

void ParticlePixels::triggerRefresh() {
    refresh = true;
}

/*************************
 * palettes
 */

PixCol colorsBW[] = {PixCol::WHITE, PixCol::BLACK};  // white is first so that blink/fade animations work
PixCol colorsRGB[] = {PixCol::R, PixCol::G, PixCol::B};
PixCol colorsRYGB[] = {PixCol::RED, PixCol::YELLOW, PixCol::GREEN, PixCol::BLUE};
PixCol colorsRYGBStripe[] = {PixCol::RED, 0, PixCol::YELLOW, 0, PixCol::GREEN, 0, PixCol::BLUE, 0 };
PixCol colorsRainbow[] = {PixCol::RED, PixCol::ORANGE, PixCol::YELLOW, PixCol::GREEN, PixCol::BLUE, PixCol::INDIGO, PixCol::VIOLET};

PixCol colorsBasic[] = {PixCol::RED, PixCol::LIME, PixCol::BLUE, PixCol::YELLOW, PixCol::CYAN, PixCol::MAGENTA, PixCol::MAROON, PixCol::OLIVE, PixCol::GREEN, PixCol::PURPLE, PixCol::TEAL, PixCol::NAVY};

PixCol colorsReds[] = {PixCol::LIGHT_SALMON, PixCol::SALMON, PixCol::DARK_SALMON, PixCol::LIGHT_CORAL, PixCol::INDIAN_RED, PixCol::CRIMSON, PixCol::FIREBRICK, PixCol::RED, PixCol::DARK_RED};
PixCol colorsOranges[] = {PixCol::CORAL, PixCol::TOMATO, PixCol::ORANGE_RED, PixCol::GOLD, PixCol::ORANGE, PixCol::DARK_ORANGE};
PixCol colorsYellows[] = {PixCol::LIGHT_YELLOW, PixCol::LEMON_CHIFFON, PixCol::LIGHT_GOLDEN_ROD_YELLOW, PixCol::PAPAYA_WHIP, PixCol::MOCCASIN, PixCol::PEACH_PUFF, PixCol::PALE_GOLDEN_ROD, PixCol::KHAKI, PixCol::DARK_KHAKI, PixCol::YELLOW};
PixCol colorsGreens[] = {PixCol::LAWN_GREEN, PixCol::CHARTREUSE, PixCol::LIME_GREEN, PixCol::LIME, PixCol::FOREST_GREEN, PixCol::GREEN, PixCol::DARK_GREEN, PixCol::GREEN_YELLOW, PixCol::YELLOW_GREEN, PixCol::SPRING_GREEN, PixCol::MEDIUM_SPRING_GREEN, PixCol::LIGHT_GREEN, PixCol::PALE_GREEN, PixCol::DARK_SEA_GREEN, PixCol::MEDIUM_SEA_GREEN, PixCol::SEA_GREEN, PixCol::OLIVE, PixCol::DARK_OLIVE_GREEN, PixCol::OLIVE_DRAB};
PixCol colorsCyans[] = {PixCol::LIGHT_CYAN, PixCol::CYAN, PixCol::AQUA_MARINE, PixCol::MEDIUM_AQUA_MARINE, PixCol::PALE_TURQUOISE, PixCol::TURQUOISE, PixCol::MEDIUM_TURQUOISE, PixCol::DARK_TURQUOISE, PixCol::LIGHT_SEA_GREEN, PixCol::CADET_BLUE, PixCol::DARK_CYAN, PixCol::TEAL};
PixCol colorsBlues[] = {PixCol::POWDER_BLUE, PixCol::LIGHT_BLUE, PixCol::LIGHT_SKY_BLUE, PixCol::SKY_BLUE, PixCol::DEEP_SKY_BLUE, PixCol::LIGHT_STEEL_BLUE, PixCol::DODGER_BLUE, PixCol::CORN_FLOWER_BLUE, PixCol::STEEL_BLUE, PixCol::ROYAL_BLUE, PixCol::BLUE, PixCol::MEDIUM_BLUE, PixCol::DARK_BLUE, PixCol::NAVY, PixCol::MIDNIGHT_BLUE, PixCol::MEDIUM_SLATE_BLUE, PixCol::SLATE_BLUE, PixCol::DARK_SLATE_BLUE};
PixCol colorsPurples[] = {PixCol::LAVENDER, PixCol::THISTLE, PixCol::PLUM, PixCol::VIOLET, PixCol::ORCHID, PixCol::FUSCHSIA, PixCol::MEDIUM_ORCHID, PixCol::MEDIUM_PURPLE, PixCol::BLUE_VIOLET, PixCol::DARK_VIOLET, PixCol::DARK_ORCHID, PixCol::DARK_MAGENTA, PixCol::PURPLE, PixCol::INDIGO};
PixCol colorsPinks[] = {PixCol::PINK, PixCol::LIGHT_PINK, PixCol::HOT_PINK, PixCol::DEEP_PINK, PixCol::PALE_VIOLET_RED, PixCol::MEDIUM_VIOLET_RED};


PixPal paletteBW = {2, colorsBW };
PixPal paletteRGB = {3, colorsRGB };
PixPal paletteRYGB = {4, colorsRYGB };
PixPal paletteRYGBStripe = {8, colorsRYGBStripe };
PixPal paletteRainbow = {7, colorsRainbow };

PixPal  __unused paletteBasic = {sizeof(colorsBasic) / sizeof(colorsBasic[0]), colorsBasic };

PixPal __unused paletteReds = {sizeof(colorsReds) / sizeof(colorsReds[0]), colorsReds };
PixPal __unused paletteOranges = {sizeof(colorsOranges) / sizeof(colorsOranges[0]), colorsOranges };
PixPal __unused paletteYellows = {sizeof(colorsYellows) / sizeof(colorsYellows[0]), colorsYellows };
PixPal __unused paletteGreens = {sizeof(colorsGreens) / sizeof(colorsGreens[0]), colorsGreens };
PixPal __unused paletteCyans = {sizeof(colorsCyans) / sizeof(colorsCyans[0]), colorsCyans };
PixPal __unused paletteBlues = {sizeof(colorsBlues) / sizeof(colorsBlues[0]), colorsBlues };
PixPal __unused palettePurples = {sizeof(colorsPurples) / sizeof(colorsPurples[0]), colorsPurples };
PixPal __unused palettePinks = {sizeof(colorsPinks) / sizeof(colorsPinks[0]), colorsPinks };


/*************************
 * animations
 */

void __unused animation_blink(PixAniData* data) {
data->setPixels(data->paletteColor(0).scale((data->step(2) + 1) % 2));
}

void __unused animation_alternating(PixAniData* data) {
int step = data->step(2);
for (int idx = 0; idx < data->pixelCount; idx++) {
data->pixels[idx] = data->paletteColor(0).scale((step + idx) % 2);
}
}

void __unused animation_fadeIn(PixAniData* data) {
data->setPixels(data->paletteColor(0).scale(data->step(1.0f)));
}

void __unused animation_fadeOut(PixAniData* data) {
data->setPixels(data->paletteColor(0).scale(1.0f - data->step(1.0f)));
}

void __unused animation_glow(PixAniData* data) {
float scale = (-cosf(data->step((float)M_PI*2)) + 1.0f) / 2.0f;
data->setPixels(data->paletteColor(0).scale(scale));
}

void __unused animation_strobe(PixAniData* data) {
int step = data->step(10);  // 1/10th of the cycle
if (step != data->data) {
data->data = step;
data->setPixels(data->randomColor().scale(step==0));
}
}

void __unused animation_sparkle(PixAniData* data) {
if (data->data == 0 && data->start == data->cycleDuration) { data->data = 10; }
int step = (int) (data->cycleDuration / data->data);
for (int idx = 0; idx < data->pixelCount; idx++) {
data->pixels[idx] = (random(step) == 0) ? data->randomColor() : data->pixelColor(idx).scale(0.90);
}
}

void __unused animation_fader(PixAniData* data) {
data->setPixels(data->palettePartialStepColor());
}

void __unused animation_cycle(PixAniData* data) {
data->setPixels(data->paletteStepColor());
}

void __unused animation_random(PixAniData* data) {
int step = data->paletteStep();
data->setPixels((step != data->data) ? data->randomColor() : data->pixelColor(0));
data->data = step;
}

void __unused animation_increment(PixAniData* data) {
int pixStep = data->pixelStep();
PixCol color = data->palettePartialStepColor();
for (int idx = 0; idx < data->pixelCount; idx++) {
data->pixels[idx] = color.scale(idx == pixStep ? 1 : 0);
}
}

void __unused animation_decrement(PixAniData* data) {
int pixStep = data->pixelStep();
PixCol color = data->palettePartialStepColor();
for (int idx = 0; idx < data->pixelCount; idx++) {
data->pixels[idx] = color.scale(idx == (data->pixelCount - 1 - pixStep) ? 1 : 0);
}
}

void __unused animation_bounce(PixAniData* data) {
int pixStep = data->step(2 * data->pixelCount - 2);
PixCol color = data->palettePartialStepColor();
for (int idx = 0; idx < data->pixelCount; idx++) {
int scale = (idx == (-abs(pixStep - data->pixelCount + 1) + data->pixelCount - 1)) ? 1 : 0;
data->pixels[idx] = color.scale(scale);
}
}

void __unused animation_scanner(PixAniData* data) {
float tail = data->pixelCount / 4;
float step = data->step(2 * data->pixelCount + ((tail * 4) - 1));
PixCol color = data->palettePartialStepColor();
for (int idx = 0; idx < data->pixelCount; idx++) {
float scale = constrain(-abs((int) (abs((int) (step - data->pixelCount - 2 * tail)) - idx - tail)) + tail, 0, 1);
data->pixels[idx] = color.scale(scale);
}
}

void __unused animation_comet(PixAniData* data) {
float tail = data->pixelCount / 2;
float pixStep = data->step(2 * data->pixelCount - tail);
PixCol color;
for (int idx = 0; idx < data->pixelCount; idx++) {
float palStep = data->mapf(max(pixStep - idx, 0), 0, (data->pixelCount / 1.75f), 0, data->paletteCount() - 1);
color = data->paletteColor(palStep);
float scale = constrain((((idx - pixStep) / tail + 1.25f)) * (idx - pixStep < 0 ? 1 : 0), 0, 1);
data->pixels[idx] = color.scale(scale);
}
}

void __unused animation_bars(PixAniData* data) {
int step = data->pixelStep();
for (int idx = 0; idx < data->pixelCount; idx++) {
data->pixels[idx] = data->paletteColor((step + idx) * data->paletteCount() / data->pixelCount);
}
}

void __unused animation_gradient(PixAniData* data) {
float step = data->pixelStep();
for (int idx = 0; idx < data->pixelCount; idx++) {
data->pixels[idx] = data->paletteColor((step + idx) * data->paletteCount() / data->pixelCount);
}
}


#pragma clang diagnostic pop
