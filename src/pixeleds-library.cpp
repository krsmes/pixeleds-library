/*
Copyright 2024 The Brynwood Team, LLC

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

#include "pixeleds-library.h"
#include <cmath>

// Include platform-specific implementations
#if (PLATFORM_ID == 6) || (PLATFORM_ID == 8) || (PLATFORM_ID == 10) || (PLATFORM_ID == 88)
    #include "pixeleds-photon1.h"
#elif (PLATFORM_ID == 32)
    #include "pixeleds-photon2.h"
#else
    #error "Platform not supported"
#endif


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

void Pixeleds::setPixel(int pixel, byte r, byte g, byte b) {
    setPixel(pixel, PixCol(r, g, b));
}

void Pixeleds::setPixel(int pixel, PixCol color) {
    animationFunction = nullptr;
    pixelStrip->setPixelColor(pixel, color);
}

void Pixeleds::setPixels(PixCol color) {
    animationFunction = nullptr;
    for (int idx = 0; idx < animationData.pixelCount; idx++) { pixelStrip->setPixelColor(idx, color); }
}

void Pixeleds::setPixels(PixPal *palette) {
    animationFunction = nullptr;
    for (int idx = 0; idx < animationData.pixelCount; idx++) { pixelStrip->setPixelColor(idx, palette->determineColorAt(idx)); }
}

void Pixeleds::updatePixel(int pixel, PixCol color) {
    pixelStrip->setPixelColor(pixel, color);
    pixelStrip->update(true);
}

void Pixeleds::updatePixels(PixCol color) {
    for (int idx = 0; idx < animationData.pixelCount; idx++) { pixelStrip->setPixelColor(idx, color); }
    pixelStrip->update(true);
}

PixAniData* Pixeleds::startAnimation(PixAniFunc *animation, PixPal *palette,
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

void Pixeleds::setAnimationRefresh(int refresh) {
    animationRefresh = refresh;
}

bool Pixeleds::isAnimationActive() const {
    return (bool) (*animationFunction);
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
