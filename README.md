# Pixeleds Library

A feature-rich C++ library for controlling addressable LED strips on Particle devices. The library provides a high-level interface for LED animations and color management, with particular support for WS2812B and SK6812W LED types.

## Features

- Support for WS2812B and SK6812W addressable LED strips
- Rich color management system with HSV/HSL color spaces
- Predefined color palettes and color sets
- Extensive animation framework with customizable effects
- Platform-specific optimizations for Particle Photon 1 and 2
- Built-in animations for common effects

**There is no use of the delay() function in Pixeleds.**

## Installation

The library can be included in your Particle project. Include the necessary headers:

```cpp
#include "pixeleds-library.h"
#include "pixeleds-colors.h"
```

## Basic Usage

Here's a simple example of a rainbow animation:

```cpp
#include "Particle.h"
#include "pixeleds-library.h"
#include "pixeleds-colors.h"

// Configuration for SK6812W LED strip
#define PARTICLE_PIXEL_PIN 0
#define PARTICLE_PIXEL_COUNT 60
#define PARTICLE_PIXEL_TYPE SK6812W
#define PARTICLE_PIXEL_ORDER ORDER_GRBW

Pixeleds px = Pixeleds(PARTICLE_PIXEL_COUNT, PARTICLE_PIXEL_PIN, 
                       PARTICLE_PIXEL_TYPE, PARTICLE_PIXEL_ORDER);

void setup() {
    px.setup();
    // Start a rainbow animation that cycles every 500ms
    px.startAnimation(&animation_gradient, &Color::RAINBOW, 500);
}

void loop() {
    px.update(millis());
}
```

## Color Management

### Using Predefined Colors

The library provides extensive predefined colors in the `Color` namespace:

```cpp
// Basic colors
px.setPixels(Color::RED);
px.setPixels(Color::BLUE);
px.setPixel(0, Color::GREEN);

// Shades and variants
px.setPixels(Color::DARK_RED);
px.setPixels(Color::LIGHT_CORAL);
px.setPixels(Color::DEEP_SKY_BLUE);
```

### Creating Custom Colors

Colors can be created using RGB, HSV, or HSL:

```cpp
// RGB constructor (0-255)
PixCol orange(255, 128, 0);

// Using hex values
PixCol purple(0x800080);

// Using HSV (normalized 0.0-1.0)
PixCol hsvColor = PixCol::hsv(Hue::RED, 1.0, 1.0);

// Using HSL (normalized 0.0-1.0)
PixCol hslColor = PixCol::hsl(0.333, 1.0, 0.5);

// Color manipulation
PixCol dimmed = color.scale(0.5);  // 50% brightness
PixCol saturated = color.saturate(0.8);  // 80% saturation
```

### Color Palettes

The library includes several predefined color palettes in the `Color` namespace:

```cpp
// Basic palettes
Color::BW       // Black and white
Color::RGB      // Red, green, blue
Color::RYGB     // Red, yellow, green, blue
Color::RAINBOW  // Full rainbow spectrum
Color::CYM      // Cyan, yellow, magenta

// Color family palettes
Color::REDS     // Various red shades
Color::BLUES    // Various blue shades
Color::GREENS   // Various green shades
Color::CYANS    // Various cyan shades
Color::PURPLES  // Various purple shades
```

Creating custom palettes:

```cpp
PixPal customPalette = PixPal::create({
    Color::CRIMSON,
    Color::ORANGE_RED,
    PixCol::hsv(Hue::ORANGE, 1.0, 0.3),
    Color::GOLD
});
```

## Creating Custom Animations

The animation system uses the `PixAniData` structure and `PixAniFunc` function type.

### Understanding PixAniData

The `PixAniData` structure provides animation control:

```cpp
struct PixAniData {
    // Basic strip information
    int pixelCount;          // Number of LEDs
    PixCol *pixels;          // LED color array
    PixPal *palette;         // Current color palette
    
    // Timing information
    long cycleDuration;      // Cycle length in ms
    long start;              // Start time
    long stop;               // End time
    system_tick_t updated;   // Current time
    
    // Animation progress
    long cycleMillis;        // Ms into cycle
    long cycleCount;         // Completed cycles
    float cyclePct;          // Cycle progress (0.0-1.0)
    int data;                // Custom data
};
```

### Helper Functions

PixAniData provides various helper functions:

```cpp
// Cycle progress
int step(int steps);              // Integer steps
float step(float steps);          // Float steps
double sineWave(float periods=1); // Sine wave
double triangleWave(float periods=1);
double sawtoothWave(float periods=1);
double squareWave(float periods=1);
double arctanWave(float periods=1);

// Color helpers
int paletteCount();
PixCol paletteColor(int idx);
PixCol paletteColor(float idx);
PixCol randomColor();
PixCol pixelColor(int idx);
void setPixels(PixCol color);
```

### Example Custom Animations

Here are some example animations:

```cpp
// Fade from black animation
void fade_from_black(PixAniData* data) {
    PixCol from = Color::BLACK;
    PixCol target;
    float ratio = float(data->paletteCount()) / data->pixelCount;
    
    for (int i = 0; i < data->pixelCount; i++) {
        target = data->paletteColor(i * ratio);
        data->pixels[i] = from.interpolate(target, data->cyclePct);
    }
}

// Moving gradient animation
void moving_gradient(PixAniData* data) {
    for (int i = 0; i < data->pixelCount; i++) {
        float pos = float(i + data->cycleMillis) / data->pixelCount;
        data->pixels[i] = data->paletteColor(pos * data->paletteCount());
    }
}
```

### Using Custom Animations

Start animations with specified timing:

```cpp
// Run animation with 10-second cycle, once
px.startAnimation(&fade_from_black, &customPalette, 10000, 10000);

// Run indefinitely with 1-second cycle
px.startAnimation(&moving_gradient, &Color::RAINBOW, 1000, -1);
```

## Built-in Animations

The library includes several pre-built animations:

- `animation_blink` - Simple on/off blinking
- `animation_alternating` - Alternating pattern
- `animation_fadeIn/fadeOut` - Smooth fades
- `animation_glow` - Pulsing effect
- `animation_strobe` - Strobe effect
- `animation_sparkle` - Random twinkling
- `animation_fader` - Smooth color fading
- `animation_cycle` - Color cycling
- `animation_random` - Random colors
- `animation_increment/decrement` - Moving pixel
- `animation_bounce` - Bouncing effect
- `animation_scanner` - Scanning effect
- `animation_comet` - Comet trail effect
- `animation_bars` - Color bars
- `animation_gradient` - Smooth gradients

## Platform Support

The library includes optimized implementations for:
- Particle Photon 1 (PLATFORM_ID 6, 8, 10, 88)
- Particle Photon 2 (PLATFORM_ID 32)

## License

Copyright 2024 The Brynwood Team, LLC

Licensed under the Apache License, Version 2.0.