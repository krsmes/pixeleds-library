#include "Particle.h"
#include "pixeleds-library.h"
#include "pixeleds-colors.h"

Pixeleds px = Pixeleds(new PixCol[11] {0}, 11, 3);  // Photon InternetButton config

void __unused setup() {
    px.setup();
    px.startAnimation(&animation_gradient, &Color::RAINBOW, 250); // cycle rainbow every 250ms (indefinitely)
}

void __unused loop() {
    px.update(millis());
}
