#pragma once
#if (PLATFORM_ID == 32)

#include "Particle.h"
#include "pixeleds-library.h"

#define SPI_BITS_FACTOR 3
#define SPI_CLOCK_SPEED 3125000



/**
 * @class ParticlePixels
 * @brief A class to manage and control a strip of WS2812B or SK6812W LEDs using SPI on a Particle Photon 2.
 * 
 * @param pixels Pointer to an array of PixCol objects representing the colors of the LEDs.
 * @param pixelCount The number of LEDs in the strip.
 * @param pixelPin The pin used for SPI communication (0 for SPI, 1 for SPI1) -- use the SPI MOSI pin.
 * @param type The type of LED strip (default is WS2812B, can also be SK6812W).
 * @param order The color order of the LEDs (default is ORDER_RGB).
 * 
 * @note Only WS2812B and SK6812W types are supported on the Photon 2.
 * 
 * @warning If an unsupported type is provided, an error will be logged and the constructor will return early.
 */
class ParticlePixels {
public:
    ParticlePixels(PixCol* pixels, int pixelCount, byte pixelPin, byte type = WS2812B, byte order = ORDER_RGB) 
        : pixels(pixels), pixelCount(pixelCount), refresh(true)
    {
        if (type != WS2812B && type != SK6812W) {
            Log.error("Only WS2812B and SK6812W supported on Photon 2");
            return;
        }
        spi = pixelPin == 0 ? &SPI : &SPI1;
        if (spi->interface() >= HAL_PLATFORM_SPI_NUM) {
            Log.error("SPI/SPI1 interface not defined!");
            return; 
        }
        bytesPerLED = (order>>6 & 0b11) ? 4 : 3; // 3 bytes for RGB, 4 bytes for RGBW
        // Extract offsets from order parameter (each 2 bits represents position)
        rOffset = SPI_BITS_FACTOR * (uint8_t)(order & 0b11);    // R offset
        gOffset = SPI_BITS_FACTOR * (uint8_t)(order>>2 & 0b11); // G offset
        bOffset = SPI_BITS_FACTOR * (uint8_t)(order>>4 & 0b11); // B offset
        wOffset = SPI_BITS_FACTOR * (uint8_t)(order>>6 & 0b11); // W offset
        // 0.4 bytes per microsecond .... 3.125mhz / 8 = 0.390625 bytes per microsecond
        // 300us * 0.4 bytes per microsecond = 120 bytes (or 960 bits)
        // 50us * 0.4 bytes per microsecond = 20 bytes (or 160 bits)
        resetOffset = 300 * 4 / 10;
        // bytes per color, spi bits per color bit, plus reset offset (start and end)
        // e.g. 10 pixels * 3 bytes per pixel * 3 spi bits per color bit + 300us reset = (10 * 3 * 3) + 120 + 120 = 540 bytes 
        spiArraySize = (pixelCount * bytesPerLED * SPI_BITS_FACTOR) + resetOffset + resetOffset;
        spiArray = (uint8_t*) malloc(spiArraySize);
        if (spiArray == NULL) { 
            Log.error("Not enough memory available!"); 
            return;
        }
        memset(spiArray, 0, spiArraySize);  // clear the array
    }

    ~ParticlePixels() {
        if (spiArray) {
            free(spiArray);
        }
        if (spi) {
            spi->end();
        }
    }

    void setup();
    void update(bool doRefresh = false);
    void setPixelColor(int pixel, PixCol pixelColor);
    void triggerRefresh();

private:
    // pass in constructor
    PixCol* pixels;
    int pixelCount;
    SPIClass* spi;

    // determines if update() should refresh the pixels
    bool refresh;
    
    // computed at initialization
    uint8_t bytesPerLED;
    uint8_t rOffset, gOffset, bOffset, wOffset; 
    size_t resetOffset;
    size_t spiArraySize;
    uint8_t* spiArray;
};

#endif