#if PLATFORM_ID == 32
#include "pixeleds-photon2.h"
#include "pixeleds-library.h"

/**
 * Encodes a single byte into a 3-byte WS2812B bit pattern.
 *
 * Each bit in the input byte is encoded into a 3-bit pattern:
 * - '1' is encoded as '110'
 * - '0' is encoded as '100'
 *
 * The encoding is done in three bytes:
 * First byte:  Contains patterns for bits 7,6,5 (MSB)
 * Second byte: Contains patterns for bits 4,3,2
 * Third byte:  Contains patterns for bits 2(cont),1,0 (LSB)
 *
 * @param byte    The input byte to encode
 * @param target  Pointer to where the 3 encoded bytes should be written
 *
 * @note This function performs inlined bitwise operations for optimal timing
 *       in LED control via SPI. The target buffer must have space for 3 bytes.
 *
 * Example:
 * Input:  0b10110100
 * Output: 0b11010011  First byte  (bits 7,6,5)
 *         0b01001101  Second byte (bits 4,3,2)
 *         0b00100110  Third byte  (bits 2-cont,1,0)
 */
inline void encodeByteTo3xBits(uint8_t byte, uint8_t* target) {
    // 1 = 110, 0 = 100
    // First byte: bits 7,6,5
    *target++ = 
        ((byte & 0b10000000) ? 0b11000000 : 0b10000000) | // bit 7 -> bits 7,6,5
        ((byte & 0b01000000) ? 0b00011000 : 0b00010000) | // bit 6 -> bits 4,3,2
        ((byte & 0b00100000) ? 0b00000011 : 0b00000010);  // bit 5 -> bits 1,0 (continues)

    // Second byte: bits 4,3,2 (bit 5's continuation is always 0)
    *target++ = 
        ((byte & 0b00010000) ? 0b01100000 : 0b01000000) | // bit 4 -> bits 6,5,4
        ((byte & 0b00001000) ? 0b00001100 : 0b00001000) | // bit 3 -> bits 3,2,1
        ((byte & 0b00000100) ? 0b00000001 : 0b00000001);  // bit 2 -> bit 0 (continues)

    // Third byte: continuation of bit 2, then bits 1,0
    *target =
        ((byte & 0b00000100) ? 0b10000000 : 0b00000000) | // bit 2 continues -> bits 7,6
        ((byte & 0b00000010) ? 0b00110000 : 0b00100000) | // bit 1 -> bits 5,4,3
        ((byte & 0b00000001) ? 0b00000110 : 0b00000100);  // bit 0 -> bits 2,1,0
}

/**
* Initializes SPI configuration for addressable LED control with MOSI-only operation.
* 
* This setup function configures SPI for specialized LED control by:
* 1. Preserving existing SCK and MISO pin configurations
* 2. Initializing SPI in MOSI-only mode to avoid conflicts
* 3. Setting appropriate clock speed for timing
* 4. Restoring SCK and MISO pins to their original states
* 
* The initialization process:
* - Identifies correct SPI pins based on interface (SPI or SPI1)
* - Saves current pin modes and states of SCK/MISO
* - Configures SPI with MOSI-only flag for LED data output
* - Restores SCK/MISO pins to original modes and states
* 
* This allows other pins on the SPI interface to be used for different purposes
* while dedicating MOSI for LED control.
* 
* @note SCK and MISO pins are freed for GPIO use after setup
* @note Clock speed must be set after begin() due to Device OS 5.7.0 requirement
* @note Uses PIN_INVALID to prevent automatic SS pin configuration
*/
void ParticlePixels::setup() {
    pin_t sckPin = SCK;
    pin_t misoPin = MISO;
    if (spi->interface() == HAL_SPI_INTERFACE2) {
        sckPin = SCK1;
        misoPin = MISO1;
    }
    PinMode sckPinMode = getPinMode(sckPin);
    PinMode misoPinMode = getPinMode(misoPin);
    int sckValue = (sckPinMode == OUTPUT) ? digitalRead(sckPin) : 0;
    int misoValue = (misoPinMode == OUTPUT) ? digitalRead(misoPin) : 0;
    
    // implement begin() with MOSI_ONLY flag (no Wiring API yet for this)
    hal_spi_config_t spi_config = {};
    spi_config.size = sizeof(spi_config);
    spi_config.version = HAL_SPI_CONFIG_VERSION;
    spi_config.flags = (uint32_t)HAL_SPI_CONFIG_FLAG_MOSI_ONLY;
    hal_spi_begin_ext(spi->interface(), SPI_MODE_MASTER, PIN_INVALID, &spi_config);
    spi->setClockSpeed(SPI_CLOCK_SPEED); // OS 5.7.0 requires setClockSpeed() to be set after begin()
    
    // allow SCLK and MISO pin to be used as GPIO
    pinMode(sckPin, sckPinMode);
    pinMode(misoPin, misoPinMode);
    if (sckPinMode == OUTPUT) {
        digitalWrite(sckPin, sckValue);
    }
    if (misoPinMode == OUTPUT) {
        digitalWrite(misoPin, misoValue);
    }
}


/**
* Updates LED strip with new pixel data using SPI transmission.
* 
* This function converts RGB(W) pixel data into addressable 
* LED-compatible SPI bit patterns and transmits them to 
* the LED strip. The process involves:
* 
* 1. Bit pattern conversion:
*    - Each LED color bit is encoded as 3 SPI bits
*    - '1' bit encoded as '110'
*    - '0' bit encoded as '100'
*    - RGB pixels: 9 bytes output per pixel (3 colors * 3 bytes each)
*    - RGBW pixels: 12 bytes output per pixel (4 colors * 3 bytes each)
* 
* 2. SPI transmission:
*    - Sends complete buffer including reset periods
*    - Timing is handled by SPI clock speed setting
* 
* @param doRefresh Force update even if no new data (default: false)
* 
* @note Input pixel data is always in RGB(W) order
* @note Output order is determined by rOffset, gOffset, bOffset, wOffset
* @note Buffer includes leading/trailing reset periods 
* @note Returns early if no pixels or no update needed
*/
void ParticlePixels::update(bool forceRefresh) {
    if (!pixels || (!refresh && !forceRefresh)) return;

    // start LED data after reset offset, yay pointer math
    uint8_t* pos = spiArray + resetOffset;
    uint8_t* colorData = (uint8_t*) pixels;

    // Convert RGB(W) pixel data into LED control SPI bit patterns
    // Each LED color bit needs to be expanded into a 3-bit SPI pattern:
    // LED '1' bit -> SPI '110'
    // LED '0' bit -> SPI '100'
    //
    // Example for one RGB pixel:
    // Input: [R:0xA5][G:0x1F][B:0xC0]  (3 bytes, one per color)
    // Output: [R:x3][G:x3][B:x3]       (9 bytes total, each input byte encoded to 3 bytes)
    //
    // For RGBW pixels, adds a 4th white component, producing 12 output bytes per pixel
    // The color order (rOffset,gOffset,bOffset,wOffset) determines final byte arrangement (e.g. ORDER_GRBW)
    // but the incoming colorData is always RGB (3 bytes per pixel) or RGBW (4 bytes per pixel)
    volatile size_t pixelStart = 0;
    for (int i = 0; i < pixelCount; i++) {
        pixelStart = i * PIXEL_BYTES_PER_COLOR;
        encodeByteTo3xBits(colorData[pixelStart], pos+rOffset);  // R
        encodeByteTo3xBits(colorData[pixelStart+1], pos+gOffset);  // G
        encodeByteTo3xBits(colorData[pixelStart+2], pos+bOffset);  // B
        if (wOffset) {
            // if pixel data has a 4th byte, encode it otherwise encode 0
            encodeByteTo3xBits((PIXEL_BYTES_PER_COLOR == 4) ? colorData[pixelStart+3] : 0, pos+wOffset);  // W
            pos += 12; // 4 color bytes * 3 led bits per color bit
        } else {
            pos += 9; // 3 color bytes * 3 led bits per color bit
        }
    }
    
    spi->beginTransaction();
    spi->transfer(spiArray, nullptr, spiArraySize, nullptr);
    spi->endTransaction();

    refresh = false;
}

#endif