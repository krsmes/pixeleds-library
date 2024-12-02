#include <climits>

#pragma SPARK_NO_PREPROCESSOR
#include "application.h"
#include "pixeleds-library.h"

//Pixeleds px = Pixeleds(new PixCol[11] {0}, 11, 3);  // Photon InternetButton config
Pixeleds px = Pixeleds(new PixCol[210] {0}, 210, 0, SK6812W);  // other LED setups

void __unused setup() {
    px.setup();
    px.rainbow(); // cycle rainbow every 250ms (indefinitely)
}

void __unused loop() {
    px.update(millis());
}
