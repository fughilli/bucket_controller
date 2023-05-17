#include "extmain.h"

#include <stdint.h>

#include <array>
#include <cmath>
#include <limits>

#include "FastLED.h"
#include "HardwareSerial.h"
#include "display.h"
#include "hsv2rgb.h"

namespace dinodisp {

Display display;

void ExtMain() {
  Serial.begin(115200);

  display.Init();
  display.SetBrightness(50);
}

void ExtLoop() {
  static uint64_t i = 0;

  CRGB color;
  for (uint32_t j = 0; j < display.kWidth; ++j) {
    hsv2rgb_rainbow({((j * 255 + i * 10) / display.kWidth), 255, 255}, color);
    display.SetPixel(j, 0, color);
  }

  display.Show();

  i += 1;
}

}  // namespace dinodisp
