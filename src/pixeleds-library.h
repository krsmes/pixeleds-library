#pragma once
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

#include "Particle.h"
#include <cmath>

#define M_2XPI 2 * M_PI

#define WS2812B 0x02
#define SK6812W 0x06

// (rOffset | rOffset | rOffset | wOffset) each offset is 2 bits, 0-3
#define ORDER_RGB (0 | (1 << 2) | (2 << 4))  // 0,1,2
#define ORDER_RBG (0 | (2 << 2) | (1 << 4))  // 0,2,1
#define ORDER_GRB (1 | (0 << 2) | (2 << 4))  // 1,0,2
#define ORDER_GBR (2 | (0 << 2) | (1 << 4))  // 2,0,1
#define ORDER_BRG (1 | (2 << 2) | (0 << 4))  // 1,2,0
#define ORDER_BGR (2 | (1 << 2) | (0 << 4))  // 2,1,0
#define ORDER_RGBW (0 | (1 << 2) | (2 << 4) | 3 << 6)  // 0,1,2,3
#define ORDER_GRBW (1 | (0 << 2) | (2 << 4) | 3 << 6)  // 1,0,2,3

#define PIXEL_BYTES_PER_COLOR 3

// forward declarations
class ParticlePixels;


/**
 * @brief A color handling struct for LED programming that stores and manipulates RGB values.
 * 
 * The PixCol (Pixel Color) struct provides a comprehensive set of methods for creating,
 * manipulating, and converting colors in both RGB and HSV/HSL color spaces. It's designed
 * specifically for LED programming, with support for various color operations and
 * transformations.
 * 
 * Features:
 * - RGB color storage (0-255 for each channel)
 * - Direct RGB value construction
 * - 24-bit hex color construction (0xRRGGBB)
 * - Color space conversions (RGB, HSV, HSL)
 * - Color interpolation and scaling
 * - Predefined color constants
 * 
 * Example Usage:
 * @code
 * // Create colors in different ways
 * PixCol red(255, 0, 0);                  // Using RGB values
 * PixCol blue(0x0000FF);                  // Using hex value
 * PixCol green = PixCol::hsv(120, 255, 255); // Using HSV
 * 
 * // Interpolate between colors
 * PixCol purple = red.interpolate(blue, 0.5);  // Mix red and blue
 * 
 * // Scale brightness
 * PixCol dimRed = red.scale(0.5);  // 50% brightness
 * @endcode
 * 
 * Color Space Support:
 * - RGB: Native format, stored as bytes for red, green, blue
 * - HSV: Convert using hsv() static method (hue: 0-360, sat/val: 0-255)
 * - HSL: Convert using hsl() static method (hue: 0-360, sat/light: 0-255)
 * 
 * Performance Considerations:
 * - Basic RGB operations are very fast (direct byte manipulation)
 * - Color space conversions (HSV/HSL) involve floating point math
 * - Interpolation and scaling use floating point calculations
 */
struct PixCol {
    byte r;
    byte g;
    byte b;

    inline PixCol() __attribute__((always_inline)) : r(0), g(0), b(0) { }

    /* create a color with the given red, green, and blue values */
    inline PixCol(byte red, byte green, byte blue)  __attribute__((always_inline))
            : r(red), g(green), b(blue) { }

    /* create a color with the given 0xRRGGBB value */
    inline PixCol(uint32_t rgb)  __attribute__((always_inline)) 
            : r((byte) (rgb >> 16 & 0xFF)), g((byte) (rgb >> 8 & 0xFF)), b((byte) (rgb >> 0 & 0xFF)) { }

    bool operator == (const PixCol &other) const {
        return (this->r == other.r and this->g == other.g and this->b == other.b);
    }

    bool operator != (const PixCol &other) const {
        return (this->r != other.r or this->g != other.g or this->b != other.b);
    }

    /* return the current color as 0xRRGGBB */
    uint32_t rgb() const { return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b; }

    /**
     * @brief Interpolate between this color and another
     * 
     * @param color Target color to interpolate towards
     * @param value Interpolation factor (0.0 = this color, 1.0 = target color)
     * @return PixCol The interpolated color
     */
    PixCol interpolate(PixCol color, float value) const {
        return PixCol((byte) (value * (color.r - r) + r),
                      (byte) (value * (color.g - g) + g),
                      (byte) (value * (color.b - b) + b));
    }

    /**
     * @brief Reverse interpolate between this color and another
     * 
     * @param color Target color to interpolate from
     * @param value Interpolation factor (0.0 = target color, 1.0 = this color)
     * @return PixCol The interpolated color
     */
    PixCol rinterpolate(PixCol color, float value) const {
        return PixCol((byte) (value * (r - color.r) + color.r),
                      (byte) (value * (g - color.g) + color.g),
                      (byte) (value * (b - color.b) + color.b));
    }

    /**
     * @brief Scale the brightness of the color
     * 
     * @param value Scaling factor (0.0 = black, 1.0 = original color)
     * @return PixCol The scaled color
     */
    PixCol scale(float value) const noexcept {
        value = value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);
        return PixCol((byte) min(r * value, 0xFF),
                      (byte) min(g * value, 0xFF),
                      (byte) min(b * value, 0xFF));
    }

    PixCol saturate(float saturation) const noexcept {
        saturation = saturation < 0.0f ? 0.0f : (saturation > 1.0f ? 1.0f : saturation);
        
        // Convert RGB to floats between 0-1
        float fR = r / 255.0f;
        float fG = g / 255.0f;
        float fB = b / 255.0f;
        
        // Find min and max RGB components
        float min = fR < fG ? (fR < fB ? fR : fB) : (fG < fB ? fG : fB);
        float max = fR > fG ? (fR > fB ? fR : fB) : (fG > fB ? fG : fB);
        float delta = max - min;
        
        // Calculate HSV values
        float hue = 0.0f;
        float val = max;
        
        if (delta != 0.0f) {
            if (max == fR) {
                hue = (fG - fB) / delta;
                if (hue < 0.0f) hue += 6.0f;
            }
            else if (max == fG) {
                hue = 2.0f + (fB - fR) / delta;
            }
            else { // max == fB
                hue = 4.0f + (fR - fG) / delta;
            }
            hue *= 60.0f;
        }
        
        // Use new saturation value and convert back to RGB
        return hsv(hue, saturation, val);
    }

    /**
     * @brief Static method to scale a color's brightness
     * 
     * @param color Color to scale
     * @param value Scaling factor (0.0 = black, 1.0 = original color)
     * @return PixCol The scaled color
     */
    static PixCol scale(PixCol color, float value) {
        return color.scale(value);
    }

    /**
     * @brief Create a color from HSV values
     * 
     * @param hue Hue angle in degrees (0-360)
     * @param sat Saturation value (0-255)
     * @param val Value/brightness (0-255)
     * @return PixCol The resulting RGB color
     * 
     * Creates a color using the HSV (Hue, Saturation, Value) color space.
     * - Hue: Color angle (0=red, 120=green, 240=blue)
     * - Saturation: Color purity (0=white/gray, 255=pure color)
     * - Value: Brightness (0=black, 255=full brightness)
     */
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

    /**
     * all parameters are normalized to 0.0-1.0
     */
    static PixCol hsv(double hue, double sat, double val) {
        return hsv(int(hue * 360), int(sat * 255), int(val * 255));
    }

    /**
     * @brief Create a color from HSL values
     * 
     * @param hue Hue angle in degrees (0-360)
     * @param sat Saturation value (0-255)
     * @param light Lightness value (0-255)
     * @return PixCol The resulting RGB color
     * 
     * Creates a color using the HSL (Hue, Saturation, Lightness) color space.
     * - Hue: Color angle (0=red, 120=green, 240=blue)
     * - Saturation: Color intensity (0=gray, 255=pure color)
     * - Lightness: Light amount (0=black, 128=pure color, 255=white)
     */
    static PixCol hsl(int hue, byte sat, byte light) {
        // Normalize inputs to 0-1 range
        float h = (hue < 0 ? 360 + hue % 360 : hue % 360) / 360.0f;
        float s = sat / 255.0f;
        float l = light / 255.0f;
        
        // No saturation means grayscale
        if (s == 0.0f) {
            byte gray = (byte)(l * 255);
            return PixCol(gray, gray, gray);
        }
        
        // Helper function to convert hue to RGB
        auto hue2rgb = [](float p, float q, float t) {
            if (t < 0.0f) t += 1.0f;
            if (t > 1.0f) t -= 1.0f;
            if (t < 1.0f/6.0f) return p + (q - p) * 6.0f * t;
            if (t < 1.0f/2.0f) return q;
            if (t < 2.0f/3.0f) return p + (q - p) * (2.0f/3.0f - t) * 6.0f;
            return p;
        };
        
        float q = l < 0.5f ? l * (1.0f + s) : l + s - l * s;
        float p = 2.0f * l - q;
        
        byte r = (byte)(hue2rgb(p, q, h + 1.0f/3.0f) * 255);
        byte g = (byte)(hue2rgb(p, q, h) * 255);
        byte b = (byte)(hue2rgb(p, q, h - 1.0f/3.0f) * 255);
        
        return PixCol(r, g, b);
    }

    /**
     * all parameters are normalized to 0.0-1.0
     */
    static PixCol hsl(double hue, double sat, double light) {
        return hsl(int(hue * 360), int(sat * 255), int(light * 255));
    }

    /*
    Basic Color Space Comparisons
    COLOR   RGB (0-255)     HSV (360°,100%,100%)    HSL (360°,100%,100%)
    ------- --------------- ----------------------   ----------------------
    BLACK   (0,0,0)         (0°,0%,0%)              (0°,0%,0%)
    WHITE   (255,255,255)   (0°,0%,100%)            (0°,0%,100%)
    RED     (255,0,0)       (0°,100%,100%)          (0°,100%,50%)
    GREEN   (0,255,0)       (120°,100%,100%)        (120°,100%,50%)
    BLUE    (0,0,255)       (240°,100%,100%)        (240°,100%,50%)

    Blue Color Variations Comparison
    COLOR           RGB (0-255)     HSV (360°,100%,100%)    HSL (360°,100%,100%)
    --------------- --------------- ----------------------   ----------------------
    PURE BLUE       (0,0,255)       (240°,100%,100%)        (240°,100%,50%)
    LIGHT BLUE      (127,127,255)   (240°,50%,100%)         (240°,100%,75%)
    DARK BLUE       (0,0,127)       (240°,100%,50%)         (240°,100%,25%)
    PASTEL BLUE     (179,179,255)   (240°,30%,100%)         (240°,100%,85%)
    NAVY BLUE       (0,0,128)       (240°,100%,50%)         (240°,100%,25%)
    GRAYISH BLUE    (107,107,179)   (240°,40%,70%)          (240°,40%,56%)
    */

};


struct PixPal {
    byte count;
    PixCol* colors;

    // Default constructor
    PixPal() : count(0), colors(nullptr) {}

    // Constructor from count and array
    PixPal(byte cnt, const PixCol* cols) : count(cnt) {
        colors = new PixCol[cnt];
        for(byte i = 0; i < cnt; i++) {
            colors[i] = cols[i];
        }
    }

    // Copy constructor
    PixPal(const PixPal& other) : count(other.count) {
        colors = new PixCol[count];
        for(byte i = 0; i < count; i++) {
            colors[i] = other.colors[i];
        }
    }

    // Assignment operator
    PixPal& operator=(const PixPal& other) {
        if (this != &other) {
            delete[] colors;
            count = other.count;
            colors = new PixCol[count];
            for(byte i = 0; i < count; i++) {
                colors[i] = other.colors[i];
            }
        }
        return *this;
    }

    // Destructor
    ~PixPal() {
        delete[] colors;
    }

    // Static creator method
    template<size_t N>
    static PixPal create(const PixCol (&cols)[N]) {
        return PixPal(N, cols);
    }

    PixCol determineColorAt(int index) const {
        return colors[index % count];
    }

    PixCol interpolateColorAt(float index) const {
        int first = (int)index % count;
        int second = (int)(index + 1) % count;
        float interpolationValue = index - (int)index;
        return colors[first].interpolate(colors[second], interpolationValue);
    }

    PixCol randomColor() const {
        return colors[random(count)];
    }
};



/**
 * @struct PixAniData
 * @brief A structure to manage pixel animation data.
 *
 * This structure holds various data required for pixel animations, including pixel and palette information,
 * timing details, and utility functions for wave generation and color manipulation.
 *
 * Members:
 * - Initialization:
 *   - int pixelCount: Number of pixels.
 *   - PixCol *pixels: Array of pixel data to manipulate.
 *   - PixPal *palette: Color palette to work with.
 *   - long cycleDuration: Total duration of one cycle in milliseconds.
 *   - long start: Time (in milliseconds) the animation started.
 *   - long stop: Time (in milliseconds) the animation will stop.
 * - Updated each loop:
 *   - system_tick_t updated: Time (in milliseconds) of the current update.
 *   - long cycleMillis: Milliseconds into the current cycle.
 *   - long cycleCount: Number of cycles performed.
 *   - float cyclePct: Percentage of the way through the current cycle.
 *   - int data: Data to pass to the animation function.
 */
struct PixAniData {
    // set in initialization:
    int pixelCount;                 // number of pixels
    PixCol *pixels;                 // array of pixel data to manipulate
    PixPal *palette;                // color palette to work with
    unsigned long cycleDuration;    // total duration of one cycle in ms (1..)
    unsigned long start;            // time (in ms) the animation started
    unsigned long stop;             // time (in ms) the animation will stop (start + total duration, equal to start for infinite)
    // updated each loop:
    system_tick_t updated;          // time (in ms) of current update
    unsigned long cycleMillis;      // ms into the current cycle (0..cycleDuration)
    unsigned long cycleCount;       // number of cycles performed.  note: this count rolls over when system.millis() value rolls over
    float cyclePct;                 // percent of the way through the current cycle
    int data;                       // data to pass to animation function

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

    inline PixCol paletteStepColor() { return palette->interpolateColorAt(paletteStep()); }
    inline PixCol palettePartialStepColor() { return palette->interpolateColorAt(palettePartialStep()); }

    inline PixCol paletteColor(float index) { return palette->interpolateColorAt(index); }
    inline PixCol paletteColor(int index) { return palette->determineColorAt(index); }

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

typedef void (PixAniFunc)(PixAniData* data);


/**
 * @class Pixeleds
 * @brief A class to manage and control a strip of addressable LEDs.
 *
 * The Pixeleds class provides methods to initialize, update, and animate a strip of addressable LEDs.
 * It supports setting individual pixels, setting all pixels, and starting animations.
 *
 * @note This class is designed to be used with the Particle platform.
 *
 * @param pixels Pointer to an array of PixCol objects representing the pixels.
 * @param pixelCount The number of pixels in the strip.
 * @param pixelPin The pin number to which the pixel strip is connected.
 * @param type The type of LED strip (default is WS2812B).
 * @param order The color order of the LED strip (default is ORDER_RGB).
 */
class Pixeleds {
public:
    Pixeleds(PixCol *pixels, int pixelCount, byte pixelPin, byte type = WS2812B, byte order = ORDER_RGB);

    // initialize all the things, must be called in application's setup()
    void setup();

    // update pixels, call this from the application's loop()
    void update(system_tick_t millis);

    /* pixels */

    // set given pixel (0 based) to given rgb colors, refreshes pixels on next update()
    void setPixel(int pixel, byte r, byte g, byte b);

    // set given pixel (0 based) to given color (0xRRGGBB or Color::X), refreshes pixels on next update()
    void setPixel(int pixel, PixCol color);

    // set all pixels to given color, refreshes pixels on next update()
    void setPixels(PixCol color);

    // spread given colors across all pixels, refreshes pixels on next update()
    void setPixels(PixPal *palette);

    // like set but forces immediate refresh, does not disable animation
    void updatePixel(int pixel, PixCol color);

    // like set but forces immediate refresh, does not disable animation
    void updatePixels(PixCol color);

    // start a pixel animation using the given animation function
    PixAniData* startAnimation(PixAniFunc *animation, PixPal *palette,
                               long cycle = 1000, long duration = -1, int data = 0);

    // set the rate the animation will be executed and refreshed
    void setAnimationRefresh(int refresh = 1000/50);

    // true if an animation is currently running
    bool isAnimationActive() const;

private:
    void updateAnimation(system_tick_t millis);

    ParticlePixels *pixelStrip;
    PixAniFunc *animationFunction {};
    PixAniData animationData = PixAniData();
    int animationRefresh{};
};



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

