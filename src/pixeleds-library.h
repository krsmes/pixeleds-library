#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-narrowing-conversions"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
#pragma ide diagnostic ignored "hicpp-explicit-conversions"
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

#ifndef PIXELEDS_H
#define PIXELEDS_H
#define PIXELEDS_V "1.0.0"

#include "Particle.h"
#include <application.h>
#include <cmath>

#define M_2XPI 2 * M_PI

#define WS2812B 0x02
#define SK6812W 0x06
//                 R    G          B
#define ORDER_RGB (0 | (1 << 2) | (2 << 4))
#define ORDER_RBG (0 | (2 << 2) | (1 << 4))
#define ORDER_GRB (1 | (0 << 2) | (2 << 4))
#define ORDER_GBR (2 | (0 << 2) | (1 << 4))
#define ORDER_BRG (1 | (2 << 2) | (0 << 4))
#define ORDER_BGR (2 | (1 << 2) | (0 << 4))


/* holds a color, some helper functions for manipulating the color */
struct PixCol {
    byte r;
    byte g;
    byte b;

    inline PixCol() __attribute__((always_inline)) : r(0), g(0), b(0) { }

    /* create a color with the given red, green, and blue values */
    inline PixCol(byte red, byte green, byte blue)  __attribute__((always_inline))
            : r(red), g(green), b(blue) { }

    /* create a color with the given 0xRRGGBB value */
    inline PixCol(uint32_t rgb)  __attribute__((always_inline)) // NOLINT(google-explicit-constructor)
            : r((byte) (rgb >> 16 & 0xFF)), g((byte) (rgb >> 8 & 0xFF)), b((byte) (rgb >> 0 & 0xFF)) { }

    bool operator == (const PixCol &other) const {
        return (this->r == other.r and this->g == other.g and this->b == other.b);
    }

    bool operator != (const PixCol &other) const {
        return (this->r != other.r or this->g != other.g or this->b != other.b);
    }

    /* return the current color as 0xRRGGBB */
    uint32_t rgb() { return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b; }

    /* compute a new color between the current and given.  value 1=current, .5=half between, 0=given */
    PixCol interpolate(PixCol color, float value) {
        return PixCol((byte) (value * (color.r - r) + r),
                      (byte) (value * (color.g - g) + g),
                      (byte) (value * (color.b - b) + b));
    }

    /* compute a new color between the current and given.  value 1=given, .5=half between, 0=current */
    PixCol rinterpolate(PixCol color, float value) {
        return PixCol((byte) (value * (r - color.r) + color.r),
                      (byte) (value * (g - color.g) + color.g),
                      (byte) (value * (b - color.b) + color.b));
    }

    /* multiply the current color by the given scale value */
    PixCol scale(float value) {
        return PixCol((byte) min(r * value, 0xFF),
                      (byte) min(g * value, 0xFF),
                      (byte) min(b * value, 0xFF));
    }

    static PixCol scale(PixCol color, float value) {
        return color.scale(value);
    }

    static PixCol hsv(int hue, byte sat, byte val) {
        byte r = val, g = val, b = val;
        if (sat > 0) {
            hue = hue < 0 ? 360+hue%360 : hue%360; // allow negative and continuous
            int base = ((255-sat) * val) >> 8;
            switch (hue/60) {
                case 0:  r = val;  g = (((val-base)*hue)/60)+base;  b = base;  break;
                case 1:  r = (((val-base)*(60-(hue%60)))/60)+base;  g = val;  b = base;  break;
                case 2:  r = base;  g = val;  b = (((val-base)*(hue%60))/60)+base;  break;
                case 3:  r = base;  g = (((val-base)*(60-(hue%60)))/60)+base;  b = val;  break;
                case 4:  r = (((val-base)*(hue%60))/60)+base;  g = base;  b = val;  break;
                case 5:  r = val;  g = base;  b = (((val-base)*(60-(hue%60)))/60)+base;  break;
            }
        }
        return {r, g, b};
    }

    /*
function hsl2rgb_wiki(h,s,l) {
  let c = (1-Math.abs(2*l-1))*s;
  let k = h/60;
  let x = c*(1 - Math.abs(k%2-1));

  let r1 = g1 = b1 = 0;

  if(k>=0 && k<=1) { r1=c; g1=x }
  if(k>1 && k<=2)  { r1=x; g1=c }
  if(k>2 && k<=3)  { g1=c; b1=x }
  if(k>3 && k<=4)  { g1=x; b1=c }
  if(k>4 && k<=5)  { r1=x; b1=c }
  if(k>5 && k<=6)  { r1=c; b1=x }

  let m = l - c/2;

  return [r1+m,g1+m,b1+m]
}
     */

    static PixCol hsv(double hue, double sat, double val) {
        return hsv(int(hue * 360), int(sat * 255), int(val * 255));
    }

    enum Colors: uint32_t {
        OFF     = 0,
        BLACK   = OFF,
        WHITE   = 0xFFFFFF,

        R       = 0xFF0000,
        G       = 0x00FF00,
        B       = 0x0000FF,

        // BROWN COLORS
        CORN_SILK = 0xFFF8DC,
        BLANCHED_ALMOND = 0xFFEBCD,
        BISQUE = 0xFFE4C4,
        NAVAJO_WHITE = 0xFFDEAD,
        WHEAT = 0xF5DEB3,
        BURLY_WOOD = 0xDEB887,
        TAN = 0xD2B48C,
        ROSY_BROWN = 0xBC8F8F,
        SANDY_BROWN = 0xF4A460,
        GOLDEN_ROD = 0xDAA520,
        DARK_GOLDEN_ROD = 0xB8860B,
        PERU = 0xCD853F,
        CHOCOLATE = 0xD2691E,
        SADDLE_BROWN = 0x8B4513,
        SIENNA = 0xA0522D,
        BROWN = 0xA52A2A,
        MAROON = 0x800000,

        // RED COLORS
        LIGHT_SALMON = 0xFFA07A,
        SALMON = 0xFA8072,
        DARK_SALMON = 0xE9967A,
        LIGHT_CORAL = 0xF08080,
        INDIAN_RED = 0xCD5C5C,
        CRIMSON = 0xDC143C,
        FIREBRICK = 0xB22222,
        RED = 0xFF0000,
        DARK_RED = 0x8B0000,

        // ORANGE COLORS
        CORAL = 0xFF7F50,
        TOMATO = 0xFF6347,
        ORANGE_RED = 0xFF4500,
        GOLD = 0xFFD700,
        ORANGE = 0xFFA500,
        DARK_ORANGE = 0xFF8C00,

        // YELLOW COLORS
        LIGHT_YELLOW = 0xFFFFE0,
        LEMON_CHIFFON = 0xFFFACD,
        LIGHT_GOLDEN_ROD_YELLOW = 0xFAFAD2,
        PAPAYA_WHIP = 0xFFEFD5,
        MOCCASIN = 0xFFE4B5,
        PEACH_PUFF = 0xFFDAB9,
        PALE_GOLDEN_ROD = 0xEEE8AA,
        KHAKI = 0xF0E68C,
        DARK_KHAKI = 0xBDB76B,
        YELLOW = 0xFFFF00,

        // GREEN COLORS
        LAWN_GREEN = 0x7CFC00,
        CHARTREUSE = 0x7FFF00,
        LIME_GREEN = 0x32CD32,
        LIME = 0x00FF00,
        FOREST_GREEN = 0x228B22,
        GREEN = 0x008000,
        DARK_GREEN = 0x006400,
        GREEN_YELLOW = 0xADFF2F,
        YELLOW_GREEN = 0x9ACD32,
        SPRING_GREEN = 0x00FF7F,
        MEDIUM_SPRING_GREEN = 0x00FA9A,
        LIGHT_GREEN = 0x90EE90,
        PALE_GREEN = 0x98FB98,
        DARK_SEA_GREEN = 0x8FBC8F,
        MEDIUM_SEA_GREEN = 0x3CB371,
        SEA_GREEN = 0x2E8B57,
        OLIVE = 0x808000,
        DARK_OLIVE_GREEN = 0x556B2F,
        OLIVE_DRAB = 0x6B8E23,

        // CYAN COLORS
        LIGHT_CYAN = 0xE0FFFF,
        CYAN = 0x00FFFF,
        AQUA = CYAN,
        AQUA_MARINE = 0x7FFFD4,
        MEDIUM_AQUA_MARINE = 0x66CDAA,
        PALE_TURQUOISE = 0xAFEEEE,
        TURQUOISE = 0x40E0D0,
        MEDIUM_TURQUOISE = 0x48D1CC,
        DARK_TURQUOISE = 0x00CED1,
        LIGHT_SEA_GREEN = 0x20B2AA,
        CADET_BLUE = 0x5F9EA0,
        DARK_CYAN = 0x008B8B,
        TEAL = 0x008080,

        // BLUE COLORS
        POWDER_BLUE = 0xB0E0E6,
        LIGHT_BLUE = 0xADD8E6,
        LIGHT_SKY_BLUE = 0x87CEFA,
        SKY_BLUE = 0x87CEEB,
        DEEP_SKY_BLUE = 0x00BFFF,
        LIGHT_STEEL_BLUE = 0xB0C4DE,
        DODGER_BLUE = 0x1E90FF,
        CORN_FLOWER_BLUE = 0x6495ED,
        STEEL_BLUE = 0x4682B4,
        ROYAL_BLUE = 0x4169E1,
        BLUE = 0x0000FF,
        MEDIUM_BLUE = 0x0000CD,
        DARK_BLUE = 0x00008B,
        NAVY = 0x000080,
        MIDNIGHT_BLUE = 0x191970,
        MEDIUM_SLATE_BLUE = 0x7B68EE,
        SLATE_BLUE = 0x6A5ACD,
        DARK_SLATE_BLUE = 0x483D8B,

        // PURPLE COLORS
        LAVENDER = 0xE6E6FA,
        THISTLE = 0xD8BFD8,
        PLUM = 0xDDA0DD,
        VIOLET = 0xEE82EE,
        ORCHID = 0xDA70D6,
        MAGENTA = 0xFF00FF,
        FUSCHSIA = MAGENTA,
        MEDIUM_ORCHID = 0xBA55D3,
        MEDIUM_PURPLE = 0x9370DB,
        BLUE_VIOLET = 0x8A2BE2,
        DARK_VIOLET = 0x9400D3,
        DARK_ORCHID = 0x9932CC,
        DARK_MAGENTA = 0x8B008B,
        PURPLE = 0x800080,
        INDIGO = 0x4B0082,

        // PINK COLORS
        PINK = 0xFFC0CB,
        LIGHT_PINK = 0xFFB6C1,
        HOT_PINK = 0xFF69B4,
        DEEP_PINK = 0xFF1493,
        PALE_VIOLET_RED = 0xDB7093,
        MEDIUM_VIOLET_RED = 0xC71585,

        // WHITE COLORS
        SNOW = 0xFFFAFA,
        HONEYDEW = 0xF0FFF0,
        MINT_CREAM = 0xF5FFFA,
        AZURE = 0xF0FFFF,
        ALICE_BLUE = 0xF0F8FF,
        GHOST_WHITE = 0xF8F8FF,
        WHITE_SMOKE = 0xF5F5F5,
        SEA_SHELL = 0xFFF5EE,
        BEIGE = 0xF5F5DC,
        OLD_LACE = 0xFDF5E6,
        FLORAL_WHITE = 0xFFFAF0,
        IVORY = 0xFFFFF0,
        ANTIQUE_WHITE = 0xFAEBD7,
        LINEN = 0xFAF0E6,
        LAVENDER_BLUSH = 0xFFF0F5,
        MISTY_ROSE = 0xFFE4E1,

        // GRAY COLORS
        GAINSBORO = 0xDCDCDC,
        LIGHT_GRAY = 0xD3D3D3,
        SILVER = 0xC0C0C0,
        DARK_GRAY = 0xA9A9A9,
        GRAY = 0x808080,
        DIM_GRAY = 0x696969,
        LIGHT_SLATE_GRAY = 0x778899,
        SLATE_GRAY = 0x708090,
        DARK_SLATE_GRAY = 0x2F4F4F,

    };
};

namespace Hue {
    constexpr double RED          = 0.0;     // 0°
    constexpr double ORANGE       = 0.083;   // 30°
    constexpr double YELLOW       = 0.167;   // 60°
    constexpr double YELLOW_GREEN = 0.25;    // 90°
    constexpr double GREEN        = 0.333;   // 120°
    constexpr double BLUE_GREEN   = 0.417;   // 150°
    constexpr double CYAN         = 0.5;     // 180°
    constexpr double AZURE        = 0.583;   // 210°
    constexpr double BLUE         = 0.667;   // 240°
    constexpr double VIOLET       = 0.75;    // 270°
    constexpr double MAGENTA      = 0.833;   // 300°
    constexpr double PINK         = 0.917;   // 330°
    constexpr double RED_MAX      = 1.0;     // 360°
}


/* holds a set of colors, helper functions for selecting or computing a color from the palette */
struct PixPal {
    byte count;
    PixCol *colors;

    /* return a color at the given index, if the index is fractional compute the proper color between the two indices */
    PixCol computeColorAt(float index) {
        int first = (int) index % (count);
        int second = (int) (index + 1) % (count);
        float interpolationValue = index - (int) index;
        return colors[first].interpolate(colors[second], interpolationValue);
    }

    /* return one of the palette colors randomly */
    PixCol randomColor() {
        return colors[random(count)];
    }

    bool operator == (const PixPal &other) const {
        if (this->count != other.count) return false;
        for (int i=0; i < count; i++) {
            if (this->colors[i] != other.colors[i]) return false;
        }
        return true;
    }
};

extern PixPal paletteBW;
extern PixPal paletteRGB;
extern PixPal paletteRYGB;
extern PixPal paletteRYGBStripe;
extern PixPal paletteRainbow;
extern PixPal  __unused paletteBasic;

extern PixPal paletteReds;
extern PixPal paletteOranges;
extern PixPal paletteYellows;
extern PixPal paletteGreens;
extern PixPal paletteCyans;
extern PixPal paletteBlues;
extern PixPal palettePurples;
extern PixPal palettePinks;

/* holds the data and functions given to a PixAniFunc function */
struct PixAniData {
    // set in initialization:
    int pixelCount;       // number of pixels
    PixCol *pixels;       // array of pixel data to manipulate
    PixPal *palette;      // color palette to work with
    long cycleDuration;   // total duration of one cycle in ms (1..)
    long start;           // time (in ms) the animation started
    long stop;            // time (in ms) the animation will stop (start + total duration, equal to start for infinite)
    // updated each loop:
    system_tick_t updated;// time (in ms) of current update
    long cycleMillis;     // ms into the current cycle (0..cycleDuration)
    long cycleCount;      // number of cycles performed.  note: this count rolls over when system.millis() value rolls over
    float cyclePct;       // percent of the way through the current cycle
    int data;             // data to pass to animation function

    /* return the current step, given the number of steps, based on time and cycle time */
    int step(int steps) { return (int) (cyclePct * steps); }

    /* return the current fractional step, given the number of steps, based on time and cycle time */
    float step(float steps) { return cyclePct * steps; }

    /* return the size of the color palatte */
    inline int paletteCount() { return palette->count; }

    /* return the current step as pixel index */
    inline int pixelStep() { return step(pixelCount); }  // cycle / pixels = 1 step

    /* return the current step as palatte index */
    inline int paletteStep() { return step(palette->count); }  // cycle / palette-colors = 1 step

    /* return the current step as fractional palatte index */
    inline float palettePartialStep() { return step((float)palette->count); }

    inline PixCol paletteStepColor() { return palette->computeColorAt(paletteStep()); }
    inline PixCol palettePartialStepColor() { return palette->computeColorAt(palettePartialStep()); }

    inline PixCol paletteColor(float index) { return palette->computeColorAt(index); }
    inline PixCol paletteColor(int index) { return palette->colors[index % palette->count]; }

    inline PixCol randomColor() { return palette->randomColor(); }

    inline PixCol pixelColor(int index) { return pixels[index % pixelCount]; }

    void setPixels(PixCol color) { for (int i = 0; i < pixelCount; ++i) { pixels[i] = color; } }

    /* https://www.desmos.com/calculator/3modf4w7wj */

    /* returns y 0.0 to 1.0 sine wave for x in period.  y=0.5 at .25 & .75 y=1 at .5; y=0 at 0 & 1 */
    static double sineWave(double period, double x) { return (1.0 + sin((x/period) * M_2XPI - M_PI_2)) / 2.0; }
    /* returns y 0.0 or 1.0 square wave for x in period.   y = 1 if x < half of period */
    static double squareWave(double period, double x) { return fmod(x, period) < period/2.0 ? 1.0 : 0.0; }
    /* returns y 0.0 to 1.0 triangle wave peaking (1.0) mid period (0 at x 0 and period) */
    static double triangleWave(double period, double x) { double t = period/2.0; return (1.0/t) * (t - fabs(fmod(x,period) - t)); }
    /* returns y 0.0 to 1.0 sawtooth wave for x in period.  y=0 at 0, .25 at .25, .5 at .5, 1 at 1 */
    static double sawtoothWave(double period, double x) { return fmod(x/period,1.0); }
    /* returns y 0.0 to 1.0 arctan wave for x in period. y=0 at 0, .167 at .25, .5 at .5, .833 at .77 and 1 at 1 (curvy sawtooth) */
    static double arctanWave(double period, double x) { return clampd((1.0 + atan(fmod(x,period)/period * M_PI - M_PI_2)) / 2.0, 0.0, 1.0); }

    double sineWave(float periodsPerCycle = 1.0) { return sineWave(cycleDuration / periodsPerCycle, cycleMillis); }
    double squareWave(float periodsPerCycle = 1.0) { return squareWave(cycleDuration / periodsPerCycle, cycleMillis); }
    double triangleWave(float periodsPerCycle = 1.0) { return triangleWave(cycleDuration / periodsPerCycle, cycleMillis); }
    double sawtoothWave(float periodsPerCycle = 1.0) { return sawtoothWave(cycleDuration / periodsPerCycle, cycleMillis); }
    double arctanWave(float periodsPerCycle = 1.0) { return arctanWave(cycleDuration / periodsPerCycle, cycleMillis); }

    /* linear interpolation, map fromVal in the range of fromMin..fromMax to the range toMin..toMax (avoiding div by zero) */
    static float mapf(float fromVal, float fromMin, float fromMax, float toMin, float toMax) {
        return fromMax == fromMin ? fromVal : (fromVal - fromMin) * (toMax - toMin) / (fromMax - fromMin) + toMin;
    }

    /* min & max function all in one */
    static float clampf(float v, float lo, float hi) { return (v < lo) ? lo : (hi < v) ? hi : v; }
    static double clampd(double v, double lo, double hi) { return (v < lo) ? lo : (hi < v) ? hi : v; }

};

/* function definition for creating animations */
typedef void (PixAniFunc)(PixAniData* data);

/* palette color[0] only */
extern PixAniFunc animation_blink;
extern PixAniFunc animation_alternating;
extern PixAniFunc animation_fadeIn;
extern PixAniFunc animation_fadeOut;
extern PixAniFunc animation_glow;

/* all palette colors */
extern PixAniFunc animation_strobe;
extern PixAniFunc animation_sparkle;
extern PixAniFunc animation_fader;
extern PixAniFunc animation_cycle;
extern PixAniFunc animation_random;
extern PixAniFunc animation_increment;
extern PixAniFunc animation_decrement;
extern PixAniFunc animation_bounce;
extern PixAniFunc animation_scanner;
extern PixAniFunc animation_comet;
extern PixAniFunc animation_bars;
extern PixAniFunc animation_gradient;




// LED driver (now using neopixel)
class Adafruit_NeoPixel;

class ParticlePixels {
public:
    ParticlePixels(PixCol *pixels, int pixelCount, byte pin, byte type = WS2812B, byte order = ORDER_GRB);

    ~ParticlePixels();

    void setup();

    void update(bool doRefresh = false) __attribute__((optimize("Ofast")));

    void triggerRefresh();

    void setPixelColor(int pixel, PixCol pixelColor);

private:
    PixCol *pixels;
    int pixelCount;
    Adafruit_NeoPixel *strip;
    bool refresh = true;
};




/**********************************************************************************************************************/


class Pixeleds {
public:
    Pixeleds(PixCol *pixels, int pixelCount, byte pixelPin, byte type = WS2812B, byte order = ORDER_RGB);

    // initialize all the things, must be called in application's setup()
    void setup();

    // update pixels, call this from the application's loop()
    void update(system_tick_t millis);


    /* pixels */

    // set given pixel (0 based) to given rgb colors, refreshes pixels on next update()
    void __unused setPixel(int pixel, byte r, byte g, byte b);

    // set given pixel (0 based) to given color (0xRRGGBB or PixCol::X), refreshes pixels on next update()
    void __unused setPixel(int pixel, PixCol color);

    // set all pixels to given color, refreshes pixels on next update()
    void __unused setPixels(byte r, byte g, byte b);

    // set all pixels to given color, refreshes pixels on next update()
    void __unused setPixels(PixCol color);

    // set all pixels to given colors array, refreshes pixels on next update()
    void __unused setPixels(PixCol* colors, int count);

    // like set but forces immediate refresh, does not disable animation
    void __unused updatePixel(int pixel, PixCol color);

    // like set but forces immediate refresh, does not disable animation
    void __unused updatePixels(PixCol color);

    // like set but forces immediate refresh, does not disable animation
    void __unused updatePixels(PixCol* colors, int count);


    /* animation */

    // start a pixel animation using the given animation function
    PixAniData* startAnimation(PixAniFunc *animation, PixPal *palette = &paletteRainbow,
                               long cycle = 1000, long duration = -1, int data = 0);

    // set the rate the animation will be executed and refreshed
    void setAnimationRefresh(int refresh = 1000/30);

    // true if an animation is currently running
    bool isAnimationActive();

    // display the &animation_gradient and &palette_rainbow with the given cycle times and duration, -1=indefinite
    void rainbow(long cycle = 1000, long duration = -1);


private:
    void updateAnimation(system_tick_t millis);

    ParticlePixels *pixelStrip;
    PixAniFunc *animationFunction {};
    PixAniData animationData = PixAniData();
    int animationRefresh{};
};

/**********************************************************************************************************************/

#endif //PIXELEDS_H

#pragma clang diagnostic pop