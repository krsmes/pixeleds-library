#pragma once
#if (PLATFORM_ID == 6) || (PLATFORM_ID == 8) || (PLATFORM_ID == 10) || (PLATFORM_ID == 88)  // photon, p1, electron, readbear-duo

#include "Particle.h"
#include "pixeleds-library.h"

class ParticlePixels {
public:
    ParticlePixels(PixCol *pixels, int pixelCount, byte pin, byte type = WS2812B, byte order = ORDER_GRB);
    ~ParticlePixels();

    void setup();
    void update(bool forceRefresh = false);
    inline void triggerRefresh() { refresh = true; }

    int getPixelCount() { return pixelCount; }
    PixCol* getPixels() { return pixels; }

    void setPixelColor(int pixel, PixCol pixelColor) {
        if (pixel >= pixelCount) return;
        pixels[pixel] = pixelColor;
        triggerRefresh();
    };
    void setPixelColor(int pixel, byte r, byte g, byte b) {
        if (pixel >= pixelCount) return;
        pixels[pixel] = PixCol(r, g, b);
        triggerRefresh();
    };

private:
    byte pin;
    PixCol *pixels;
    int pixelCount;
    byte type;
    byte rOfs,gOfs,bOfs;
    unsigned long endMicros;
    bool refresh;
};

#endif