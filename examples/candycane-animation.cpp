/* 
 * Project candycane-animation
 * Author: Kevin Smith
 * Date: 2024-12-12
 */
#include "Particle.h"
#include "pixeleds-library.h"
#include "pixeleds-colors.h"

SYSTEM_MODE(SEMI_AUTOMATIC);
SYSTEM_THREAD(ENABLED);

#define PARTICLE_PIXEL_PIN 0
#define PARTICLE_PIXEL_COUNT 60
#define PARTICLE_PIXEL_TYPE SK6812W
#define PARTICLE_PIXEL_ORDER ORDER_GRBW

Pixeleds px = Pixeleds(new PixCol[PARTICLE_PIXEL_COUNT] {0}, PARTICLE_PIXEL_COUNT, 
                       PARTICLE_PIXEL_PIN, PARTICLE_PIXEL_TYPE, PARTICLE_PIXEL_ORDER);

// Define our candy cane colors
PixPal candyPalette = PixPal::create({Color::RED.scale(0.5), Color::WHITE.scale(0.25)});

// Configuration structure to hold all animation parameters
struct CandyCaneConfig {
    // Base animation settings
    int baseSpeed = 50;           // Base milliseconds per pixel movement
    int baseSegmentSize = 5;      // Base size of each color segment in pixels
    bool powerSaveMode = false;   // When true, only uses every other LED
    
    // Direction change settings
    long minDirectionTime = 10000; // Minimum time before direction change (ms)
    long maxDirectionTime = 30000; // Maximum time before direction change (ms)
    
    // Speed variation settings
    long minSpeedTime = 6000;     // Minimum time before speed change (ms)
    long maxSpeedTime = 12000;    // Maximum time before speed change (ms)
    float minSpeedMultiplier = 0.8;
    float maxSpeedMultiplier = 4.0;
    
    // Stretch settings
    long minStretchDirectionTime = 8000;  // Minimum time before changing stretch direction
    long maxStretchDirectionTime = 16000;  // Maximum time before changing stretch direction
    float minStretchMultiplier = 0.25;  // Minimum size (quarter original)
    float maxStretchMultiplier = 2.0;  // Maximum size (double original)
    float stretchStepSize = 0.05;      // How much to change size each step
    long stretchStepInterval = 500;    // Milliseconds between size changes
    
    // State tracking
    bool movingRight = true;      // Current direction
    float speedMultiplier = 1.0;  // Current speed multiplier
    float currentStretch = 1.0;   // Current stretch value
    bool stretchingOut = true;    // Whether we're currently growing or shrinking
    
    // Timing tracking
    long lastDirectionChange = 0;
    long nextDirectionChange = 0;
    long lastSpeedChange = 0;
    long nextSpeedChange = 0;
    long lastStretchStep = 0;     // Time of last stretch increment/decrement
    long nextStretchDirectionChange = 0;  // When to change stretch direction
};

// Helper function to schedule next random event
long scheduleNextEvent(long currentTime, long minInterval, long maxInterval) {
    return currentTime + random(minInterval, maxInterval);
}

// Update movement direction if it's time
void updateDirection(CandyCaneConfig* config, long currentTime) {
    if (currentTime >= config->nextDirectionChange) {
        config->movingRight = !config->movingRight;
        config->lastDirectionChange = currentTime;
        config->nextDirectionChange = scheduleNextEvent(
            currentTime, 
            config->minDirectionTime, 
            config->maxDirectionTime
        );
    }
}

// Update scrolling speed if it's time
void updateSpeed(CandyCaneConfig* config, long currentTime) {
    if (currentTime >= config->nextSpeedChange) {
        config->speedMultiplier = random(
            config->minSpeedMultiplier * 100,
            config->maxSpeedMultiplier * 100
        ) / 100.0;
        config->lastSpeedChange = currentTime;
        config->nextSpeedChange = scheduleNextEvent(
            currentTime,
            config->minSpeedTime,
            config->maxSpeedTime
        );
    }
}

// Update segment stretch using small incremental steps
void updateStretch(CandyCaneConfig* config, long currentTime) {
    // Check if it's time to change stretch direction
    if (currentTime >= config->nextStretchDirectionChange) {
        config->stretchingOut = !config->stretchingOut;
        config->nextStretchDirectionChange = currentTime + random(
            config->minStretchDirectionTime,
            config->maxStretchDirectionTime
        );
    }
    
    // Check if it's time for next stretch step
    if (currentTime >= config->lastStretchStep + config->stretchStepInterval) {
        // Update the stretch value in small increments
        if (config->stretchingOut) {
            // Growing the segments
            if (config->currentStretch < config->maxStretchMultiplier) {
                config->currentStretch += config->stretchStepSize;
            }
        } else {
            // Shrinking the segments
            if (config->currentStretch > config->minStretchMultiplier) {
                config->currentStretch -= config->stretchStepSize;
            }
        }
        
        // Ensure we stay within bounds
        config->currentStretch = constrain(
            config->currentStretch,
            config->minStretchMultiplier,
            config->maxStretchMultiplier
        );
        
        config->lastStretchStep = currentTime;
    }
}

// Calculate the current position accounting for direction and wrapping
int calculatePosition(int basePosition, int totalPixels, bool movingRight) {
    if (movingRight) {
        return basePosition % totalPixels;
    } else {
        return (totalPixels - (basePosition % totalPixels)) % totalPixels;
    }
}

// Determines if a position should be red or white, accounting for stretch
bool isRedSegment(int position, int baseSegmentSize, float stretchMultiplier) {
    // Calculate the total pattern length (one red + one white segment)
    int patternLength = baseSegmentSize * 2;
    
    // Find position within the current pattern
    int posInPattern = position % patternLength;
    
    // Calculate the center of the red segment in this pattern
    int redCenter = baseSegmentSize / 2;
    
    // Calculate the current red segment size
    float currentRedSize = baseSegmentSize * stretchMultiplier;
    
    // Calculate the distance from the nearest red segment center
    int distanceFromCenter = abs(posInPattern - redCenter);
    if (distanceFromCenter > baseSegmentSize) {
        distanceFromCenter = abs(posInPattern - (redCenter + patternLength));
    }
    
    // Return true if we're within the stretched/shrunk red segment
    return distanceFromCenter < (currentRedSize / 2);
}

// Main animation function
void candy_cane_animation(PixAniData* data) {
    CandyCaneConfig* config = (CandyCaneConfig*)data->data;
    long currentTime = data->updated;
    
    // Update animation states
    updateDirection(config, currentTime);
    updateSpeed(config, currentTime);
    updateStretch(config, currentTime);
    
    // Calculate current position based on time and speed
    int effectiveSpeed = config->baseSpeed * config->speedMultiplier;
    int basePosition = currentTime / effectiveSpeed;
    int position = calculatePosition(basePosition, data->pixelCount, config->movingRight);
    
    // Update all pixels
    for (int i = 0; i < data->pixelCount; i++) {
        // In power save mode, turn off every other LED
        if (config->powerSaveMode && (i % 2 == 1)) {
            data->pixels[i] = Color::BLACK;
            continue;
        }
        
        // Calculate position in the pattern
        int adjustedPos = (i + position) % data->pixelCount;
        if (config->powerSaveMode) {
            adjustedPos = adjustedPos / 2;
        }
        
        // Determine color based on position and current stretch value
        bool isRed = isRedSegment(adjustedPos, config->baseSegmentSize, config->currentStretch);
        data->pixels[i] = data->paletteColor(isRed ? 0 : 1);
    }
}

// Initialize the candy cane animation
void setup_candy_cane(CandyCaneConfig* config, long currentTime) {
    // Schedule initial events
    config->nextDirectionChange = scheduleNextEvent(
        currentTime, 
        config->minDirectionTime, 
        config->maxDirectionTime
    );
    config->nextSpeedChange = scheduleNextEvent(
        currentTime,
        config->minSpeedTime,
        config->maxSpeedTime
    );
    config->nextStretchDirectionChange = scheduleNextEvent(
        currentTime,
        config->minStretchDirectionTime,
        config->maxStretchDirectionTime
    );
    config->lastStretchStep = currentTime;
}


CandyCaneConfig config = CandyCaneConfig();

void setup() {
    px.setup();
    px.setPixels(Color::BLACK);

    // Start the animation
    setup_candy_cane(&config, millis());
    px.startAnimation(&candy_cane_animation, &candyPalette, 25, -1, (int)&config);
}

void loop() {
    px.update(millis());
}