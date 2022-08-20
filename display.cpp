#include "display.h"

#include <FastLED.h>

#include <cmath>

namespace bikedisp {
namespace {
constexpr int kWidth = 16;
constexpr int kHeight = 16;

int8_t kXLeft[4000]{};
uint8_t kScale[4000]{};

void MaybeInitLuts() {
  static bool luts_init = false;
  if (luts_init) return;

  for (int i = 0; i < 4000; ++i) {
    const float x = std::sqrt(i / 10.0f);
    float ipart = 0;
    kScale[i] = 255 * std::modf(x, &ipart);
    kXLeft[i] = x;
  }
  luts_init = true;
}

}  // namespace

void Display::Init() {
  MaybeInitLuts();
  FastLED.addLeds<NEOPIXEL, 5>(leds_.data(), leds_.size());
  FastLED.setBrightness(255);
}

void Display::Show() { FastLED.show(); }

void Display::Clear() {
  for (auto& led : leds_) {
    led = CRGB::Black;
  }
}

void Display::SetPixel(int x, int y, CRGB color) {
  if (x < 0 || x >= kWidth || y < 0 || y >= kHeight) return;

  if (y % 2 == 0) {
    leds_[y * 16 + (15 - x)] = color;
  } else {
    leds_[y * 16 + x] = color;
  }
}

void Display::Circle(int x, int y, float r, CRGB color) {
  if (r <= 1) {
    CRGB scaled_a = color, scaled_b = color;
    scaled_a.nscale8(255 * r);
    scaled_b.nscale8(255 * (1.0f - r));
    SetPixel(x + 1, y, scaled_a);
    SetPixel(x - 1, y, scaled_a);
    SetPixel(x, y + 1, scaled_a);
    SetPixel(x, y - 1, scaled_a);
    SetPixel(x, y, scaled_b);
    return;
  }

  if (r > 20) return;

  int y_off = 0;
  int x_off = r;
  while (y_off < x_off) {
    int arg = (r * r - y_off * y_off) * 10.0f;
    x_off = kXLeft[arg];
    uint8_t scale = kScale[arg];
    CRGB scaled_a = color, scaled_b = color;
    scaled_a.nscale8(255 - scale);
    scaled_b.nscale8(scale);
    SetPixel(x + x_off, y + y_off, scaled_a);
    SetPixel(x + 1 + x_off, y + y_off, scaled_b);
    SetPixel(x - x_off, y + y_off, scaled_a);
    SetPixel(x - (1 + x_off), y + y_off, scaled_b);
    SetPixel(x + x_off, y - y_off, scaled_a);
    SetPixel(x + 1 + x_off, y - y_off, scaled_b);
    SetPixel(x - x_off, y - y_off, scaled_a);
    SetPixel(x - (1 + x_off), y - y_off, scaled_b);
    SetPixel(y + y_off, x + x_off, scaled_a);
    SetPixel(y + y_off, x + 1 + x_off, scaled_b);
    SetPixel(y + y_off, x - x_off, scaled_a);
    SetPixel(y + y_off, x - (1 + x_off), scaled_b);
    SetPixel(y - y_off, x + x_off, scaled_a);
    SetPixel(y - y_off, x + 1 + x_off, scaled_b);
    SetPixel(y - y_off, x - x_off, scaled_a);
    SetPixel(y - y_off, x - (1 + x_off), scaled_b);
    y_off++;
  }
}

}  // namespace bikedisp
