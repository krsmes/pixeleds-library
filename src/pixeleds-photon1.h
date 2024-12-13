#pragma once
#if (PLATFORM_ID == 6) || (PLATFORM_ID == 8) || (PLATFORM_ID == 10) || (PLATFORM_ID == 88)

#include "Particle.h"
#include "pixeleds-library.h"

class ParticlePixels {
public:
    ParticlePixels(PixCol *pixels, int pixelCount, byte pin, byte type = WS2812B, byte order = ORDER_GRB);
    ~ParticlePixels();

    void setup();
    void update(bool doRefresh = false);
    void triggerRefresh();
    void setPixelColor(int pixel, PixCol pixelColor);

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