# Pixeleds Library

A feature-rich C++ library for controlling addressable LED strips on Particle devices. The library provides a high-level interface for LED animations and color management, with particular support for WS2812B and SK6812W LED types.

## Features

- Support for WS2812B and SK6812W addressable LED strips
- Rich color management system with predefined color palettes
- Extensive animation framework with customizable effects
- Built-in animations for common effects

**There is no use of the delay() function in Pixeleds.**

## Installation

The library can be included in your Particle project. Make sure to include both the header and implementation files:

```cpp
#include "pixeleds-library.h"
```

## Basic Usage

Here's a simple example to get started:

```cpp
#include "pixeleds-library.h"

// Create a strip of 11 LEDs connected to pin 3
Pixeleds px = Pixeleds(new PixCol[11] {0}, 11, 3);

void setup() {
    px.setup();
    // Start a rainbow animation that cycles every second
    px.rainbow(1000);
}

void loop() {
    px.update(millis());
}
```

## Color Management

### Using Predefined Colors

```cpp
// Using predefined colors
px.setPixels(PixCol::RED);
px.setPixels(PixCol::BLUE);
px.setPixel(0, PixCol::GREEN);
```

### Creating Custom Colors

```cpp
// RGB values (0-255)
PixCol customColor(255, 128, 0);  // Orange

// Using HSV
PixCol hsvColor = PixCol::hsv(Hue::RED, 1.0, 0.5);  // 50% bright red
```

### Color Palettes

The library includes several predefined color palettes:

- `paletteBW` - Black and white
- `paletteRGB` - Red, green, blue
- `paletteRYGB` - Red, yellow, green, blue
- `paletteRainbow` - Full rainbow spectrum
- `paletteBasic` - Basic color set
- Color family palettes: `paletteReds`, `paletteBlues`, `paletteGreens`, etc.

## Creating Custom Animations

The animation system in Pixeleds is built around the `PixAniData` structure and `PixAniFunc` function type. Understanding these is key to creating custom animations.

### Understanding PixAniData

The `PixAniData` structure provides everything needed to create animations:

```cpp
struct PixAniData {
    // Basic strip information
    int pixelCount;       // Number of LEDs in the strip
    PixCol *pixels;       // Array of pixel colors to modify
    PixPal *palette;      // Current color palette
    
    // Timing information
    long cycleDuration;   // Total milliseconds for one cycle
    long start;          // Start time in milliseconds
    long stop;           // End time (start + duration)
    system_tick_t updated; // Current update time
    
    // Animation progress
    long cycleMillis;    // Milliseconds into current cycle
    long cycleCount;     // Number of completed cycles
    float cyclePct;      // Progress through current cycle (0.0 to 1.0)
    int data;            // Custom data parameter
};
```

### Helper Functions in PixAniData

PixAniData provides several helper functions for common animation tasks:

```cpp
// Get integer steps through a cycle
int step(int steps);           // Returns 0 to steps-1 based on cycle progress
float step(float steps);       // Returns 0.0 to steps based on cycle progress

// Working with palettes
int paletteCount();           // Number of colors in palette
PixCol paletteColor(int idx); // Get specific palette color
PixCol paletteColor(float idx); // Get interpolated palette color
PixCol randomColor();         // Get random palette color

// Accessing pixels
PixCol pixelColor(int idx);   // Get current color of a pixel
void setPixels(PixCol color); // Set all pixels to a color

// Wave functions for smooth animations
double sineWave(float periodsPerCycle = 1.0);     // Sine wave pattern
double triangleWave(float periodsPerCycle = 1.0);  // Triangle wave pattern
double sawtoothWave(float periodsPerCycle = 1.0);  // Sawtooth wave pattern
```

### Creating Animation Functions

Here are some examples of custom animations showing different techniques:

```cpp
// Simple color cycling animation
void color_cycle(PixAniData* data) {
    // Use cyclePct to select a color from the palette
    PixCol color = data->paletteColor(data->cyclePct * data->paletteCount());
    data->setPixels(color);
}

// Moving dot animation
void moving_dot(PixAniData* data) {
    // Calculate dot position
    int position = data->step(data->pixelCount);
    
    // Set all pixels to black first
    data->setPixels(PixCol::BLACK);
    
    // Set the dot position to the first palette color
    data->pixels[position] = data->paletteColor(0);
}

// Pulsing animation with sine wave
void pulse_animation(PixAniData* data) {
    // Get brightness from sine wave (0.0 to 1.0)
    float brightness = data->sineWave();
    
    // Apply brightness to palette color
    PixCol color = data->paletteColor(0).scale(brightness);
    data->setPixels(color);
}

// Complex animation with multiple elements
void complex_animation(PixAniData* data) {
    // Use custom data parameter for state
    int& state = data->data;
    
    // Multiple patterns based on cycle count
    if (data->cycleCount % 2 == 0) {
        // Even cycles: moving gradient
        for (int i = 0; i < data->pixelCount; i++) {
            float colorPos = (float)(i + data->cycleMillis) / data->pixelCount;
            data->pixels[i] = data->paletteColor(colorPos * data->paletteCount());
        }
    } else {
        // Odd cycles: sparkle effect
        for (int i = 0; i < data->pixelCount; i++) {
            if (random(10) == 0) {
                data->pixels[i] = data->randomColor();
            } else {
                data->pixels[i] = data->pixels[i].scale(0.8);
            }
        }
    }
}
```

### Using Custom Animations

To use a custom animation:

```cpp
// Start animation with a 1 second cycle, running indefinitely
px.startAnimation(&color_cycle, &paletteRainbow, 1000, -1);

// Start animation with 500ms cycle, run for 5 seconds, pass custom data
px.startAnimation(&complex_animation, &paletteRGB, 500, 5000, initialState);
```

## Built-in Animations

The library includes several pre-built animations:

- `animation_blink` - Simple on/off blinking
- `animation_fadeIn` / `animation_fadeOut` - Smooth fade effects
- `animation_glow` - Pulsing glow effect
- `animation_sparkle` - Random twinkling effect
- `animation_gradient` - Smooth color transitions
- `animation_comet` - Moving light with tail effect
- `animation_scanner` - Knight Rider style scanning effect
- `animation_bounce` - Bouncing light effect

## License

Copyright 2024 The Brynwood Team, LLC

Licensed under the Apache License, Version 2.0. See LICENSE file for details.
