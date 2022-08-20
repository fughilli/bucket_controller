#include "extmain.h"

#include <stdint.h>

#include <array>
#include <cmath>
#include <limits>

#include "HardwareSerial.h"
#include "display.h"
#include "encoder.h"

namespace bikedisp {

Encoder encoder(17, 18, 0);
Display display;

void ExtMain() {
  Serial.begin(115200);

  encoder.Enable();
  display.Init();
}

void ExtLoop() {
  display.Clear();

  display.Circle(8, 8, 5 * (1 + std::sin(encoder.Count() / 256.0f)),
                 CRGB::Red);

  display.Show();
}

}  // namespace bikedisp
