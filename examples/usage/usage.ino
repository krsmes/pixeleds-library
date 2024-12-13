/**
 * This usage example uses the Particle's Photon InternetButton to show how to create pallates,
 * custom animations, and input from cloud functions to controls the LEDs using the Pixeleds class
 */
#include "Particle.h"
#include "pixeleds-library.h"
#include "pixeleds-colors.h"

SYSTEM_MODE(SEMI_AUTOMATIC);
SYSTEM_THREAD(ENABLED);

// #define PARTICLE_PHOTON_BUTTON // uncomment to use Photon InternetButton config

#ifdef PARTICLE_PHOTON_BUTTON
#define PARTICLE_PIXEL_PIN 3
#define PARTICLE_PIXEL_COUNT 11
#define PARTICLE_PIXEL_TYPE WS2812B
#define PARTICLE_PIXEL_ORDER ORDER_GRB
#else
// 144 SK6812W LEDs on pin D0
#define PARTICLE_PIXEL_PIN 0
#define PARTICLE_PIXEL_COUNT 144
#define PARTICLE_PIXEL_TYPE SK6812W
#define PARTICLE_PIXEL_ORDER ORDER_GRBW
#endif

/*
 * device LED configuration
 */
Pixeleds px = Pixeleds(new PixCol[PARTICLE_PIXEL_COUNT] {0}, PARTICLE_PIXEL_COUN) 
                       PARTICLE_PIXEL_PIN, PARTICLE_PIXEL_TYPE, PARTICLE_PIXEL_ORDE)

/*
 * palettes
 */
PixPal offPal =   PixPal::create({Color::OFF});
PixPal whitePal = PixPal::create({Color::IVORY} };
PixPal dimPal =   PixPal::create({Color::scale(Color::LINEN, 0.1f)} };
PixPal darkPal =  PixPal::create({Color::scale(Color::BEIGE, 0.02f)} };

PixPal hotPal =   PixPal::create({Color::CRIMSON, Color::ORANGE_RED, Color::DARK_ORANGE, Color::ORANGE_RED} };
PixPal warmPal =  PixPal::create({Color::GREEN_YELLOW, Color::ORANGE_RED, Color::DARK_ORANGE, Color::ORANGE_RED} };
PixPal bluePal =  PixPal::create({Color::DARK_TURQUOISE, Color::CYAN, Color::TEAL, Color::CYAN} };

PixPal christmasPal { 16, new PixCol[16] {
        Color::BLACK, Color::GREEN, Color::BLACK, Color::RED,
        Color::BLACK, Color::GREEN, Color::BLACK, Color::RED,
        Color::BLACK, Color::GREEN, Color::BLACK, Color::RED,
        Color::BLACK, Color::GREEN, Color::BLACK, Color::RED} };

PixPal customPal = { 1, new PixCol[1] {Color::WHITE} };

PixPal *pal = &offPal;

/*
 * custom animations
 */

void fade_from_black(PixAniData* data) {
    PixCol from = Color::BLACK;
    PixCol target;
    // ratio of palette size to pixel count
    // e.g. pallet of 4 colors with a pixel count of 20 is .2,
    //      so for every pixel you take 20% of the way between two pallete colors
    float palpix_ratio = float(data->paletteCount()) / (data->pixelCount - 1);
    // loop through each pixel
    for (int idx = 0; idx < data->pixelCount; idx++) {
        // choose computed target color from palette based on current pixel index
        target = data->paletteColor(idx * palpix_ratio);
        // set the pixel to a color between black and the target color based on pct of current cycle
        data->pixels[idx] = from.interpolate(target, data->cyclePct);
    }
}

void fade_alternating(PixAniData* data) {
    Serial.printlnf("fade_alternating: updated=%ld, cycleCount=%ld, cycleMillis=%ld, cyclePct=%f",
                    data->updated, data->cycleCount, data->cycleMillis, data->cyclePct);
    PixCol from = Color::BLACK;
    PixCol target;
    // ratio of palette size to pixel count
    // e.g. pallet of 4 colors with a pixel count of 20 is .2,
    //      so for every pixel you take 20% of the way between two pallete colors
    float palpix_ratio = float(data->paletteCount()) / (data->pixelCount - 1);
    // loop through each pixel
    for (int idx = 0; idx < data->pixelCount; idx++) {
        // choose computed target color from palette based on current pixel index
        target = data->paletteColor(idx * palpix_ratio);
        // set the pixel to a color between black and the target color based on pct of current cycle
        // alternate every other pixel on / off (idx % 2)
        // fade alternates in / out (data->cycleCount % 2, 1.0 - data->cyclePct, data->cyclePct)
        data->pixels[idx] = (idx % 2 == data->cycleCount % 2)
                            ? from.interpolate(target, 1.0 - data->cyclePct)
                            : from.interpolate(target, data->cyclePct);
        Serial.printf("  %6lX", data->pixels[idx].rgb());
    }
    Serial.printlnf("");
}

/*
 * setup/loop
 */

// forward declarations
int setPalette(String command);
//int setCycle(String command);
//int setAnimation(String command);
int setCustom(String command);

void setup() {
    Serial.begin(9600);

    // functions to invoke remotely to modify the LED display
    Particle.function("palette", setPalette);
    // Particle.function("cycle", setCycle);
    // Particle.function("animation", setAnimation);
    Particle.function("custom", setCustom);

    px.setup();
    px.setAnimationRefresh(1000/2); // 2Hz, infrequent updates

    setPalette("white"); // start with a white palette
}

void loop() {
    // this call is where all the work is done
    px.update(millis());  
}


/*
 * cloud functions
 */

int setPalette(String command) {
    if (command == "rainbow") {
        px.rainbow();
        return 1;
    }
    else if (command == "christmas"){ pal = &christmasPal; }
    else if (command == "custom")   { pal = &customPal; }
    else if (command == "off")      { pal = &offPal; }
    else if (command == "white")    { pal = &whitePal; }
    else if (command == "dim")      { pal = &dimPal; }
    else if (command == "dark")     { pal = &darkPal; }
    else if (command == "hot")      { pal = &hotPal; }
    else if (command == "warm")     { pal = &warmPal; }
    else if (command == "blue")     { pal = &bluePal; }
    else return -1;
    px.startAnimation(&fade_from_black, pal, 30000);
    return 0;
}

int setCustom(String command) {
    unsigned int r, g, b;
    sscanf(command, "%x %x %x", &r, &g, &b);  // parse hex color (space delimited)
    customPal.colors[0] = PixCol((byte)r, (byte)g, (byte)b);
    px.startAnimation(&fade_alternating, &customPal, 30000);
    return 0;
}
