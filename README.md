# Pixeleds

### Why

Learning how to control addressable LEDs


## Features

* **There is no use of the delay() function in Pixeleds.**

* The LED/Pixel classes support animations.  Write your own or use one of the 17 built in animations.

* The animations support custom color palettes.

* LEDs can be set with separate r, g, b values or with a combined value (like html colors).

* The Accelerometer class supports a callback method which is called when the InternetButton starts
  or stops moving.  The class also provides angle, azimuth, pitch, and roll calculations.  It uses
  a state machine when starting up to also avoid using delay().

All of the above is accomplished by an `update()` method on Pixeleds.  
Just add `update(millis())` to the `loop()` method and Pixeleds classes take care of the rest.

## Usage

...

## Examples

#### [Animations](examples/custom-animation.cpp)
Use the buttons to cycle through a variety of sample animations.

#### [SimpleRainbow](examples/simple-rainbow.cpp)
The simplest Pixeleds example.  Simple gradiant across all the LEDs.

```c
#include "pixeleds-library.h"

Pixeleds pxl = Pixeleds();

void setup() {
    pxl.setup();
    pxl.rainbow(250); // cycle rainbow every 250ms (indefinitely)
}

void loop() {
    pxl.update(millis());
}
```

The `rainbow(...)` function exists as an carryover from Particle's InternetButton library.  All it
does is call `startAnimation(&animation_gradient, &paletteRainbow, cycle, duration);`


## Reference


### Pixels (LEDs)
(in progress)

#### Built in Animations

These animations are just functions implemented at the bottom of [pixeleds-library.cpp](src/pixeleds-library.cpp).
The most complex one in this set is animation_comet and it is only 9 lines of code, so these are not complex
to implement.  Study these functions and the corresponding `PixAniData` struct which has supporting functions
to help you compute things relative to the current step, palette, pixel, etc.

##### use only first color in color palette
* `animation_blink`
* `animation_alternating`
* `animation_fadeIn`
* `animation_fadeOut`
* `animation_glow`

##### use all colors in color palette
* `animation_strobe`
* `animation_sparkle`
* `animation_fader`
* `animation_cycle`
* `animation_random`
* `animation_increment`
* `animation_decrement`
* `animation_bounce`
* `animation_scanner`
* `animation_comet`
* `animation_bars`
* `animation_gradient`

You can write your own animation functions as seen above in `AnimateAccel` and set the pattern
of LEDs based on data (such as the accelerometer).

Another example of a custom 'animation' function is in `PlaySongs` with the `animateCount()`
function which lights a single LED based on the value of the variable indicating the current
selection.

#### Built in Color Palettes

All animations depend on a color palette to function.  These palettes are simply a reference to a
`PixPal` struct which wraps a count and a pointer to an array of colors (`PixCol`).
There are 5 predefined palettes to help you get started.

* `paletteBW` -- 2: White/Black(off)
* `paletteRGB` -- 3: Red/Green/Blue
* `paletteRYGB` -- 4: Red/Yellow/Green/Blue
* `paletteRYGBStripe` -- 8: Red/Black/Yellow/Black/Green/Black/Blue/Black
* `paletteRainbow` 7: Red/Orange/Yellow/Green/Blue/Indego/Violet

Note that many animations compute the color between two colors in a palette. For example if you
use the `animation_gradient` with the `paletteBW` across 11 LEDs (pixels) you will end up with
with a gradient of the brightness of White to Black (off) across the 11 LEDs.  This is the difference
between using functions on `PixAniData` that take and return `float` vs `int`.


## Etc.

...


## License
Copyright 2022 The Brynwood Team, LLC.