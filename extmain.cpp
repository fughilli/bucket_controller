#include "extmain.h"

#include <FastLED.h>
#include <driver/gpio.h>
#include <driver/pcnt.h>
#include <stdint.h>

#include <array>
#include <limits>

#include "encoder.h"

namespace bikedisp {

std::array<CRGB, 256> panel_leds;

template <typename T>
T clamp(T value, T low, T high) {
  return std::min(high, std::max(low, value));
}

Encoder encoder(17, 18, 0);

void ExtMain() {
  Serial.begin(115200);

  encoder.Enable();

  FastLED.addLeds<NEOPIXEL, 5>(panel_leds.data(), panel_leds.size());
  FastLED.setBrightness(10);
}

void UpdateLeds(int64_t count) {
  for (auto& led : panel_leds) {
    led = CRGB::Red;
  }

  int index = (count / 100) % panel_leds.size();
  if (index < 0) {
    index += panel_leds.size();
  }
  if (index < 0 || index >= panel_leds.size()) {
    Serial.print("Failed to normalize index: ");
    Serial.println(index);
    return;
  }
  panel_leds[index] = CRGB::Green;
}

void ExtLoop() {
  int16_t count = 0;
  Serial.print("count: ");
  Serial.println(encoder.Count());

  UpdateLeds(encoder.Count());

  FastLED.show();
}

}  // namespace bikedisp
