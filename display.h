#include <FastLED.h>

#include <array>

namespace dinodisp {

class Display {
 public:
  constexpr static int kWidth = 32;
  constexpr static int kHeight = 1;

  void Init();
  void Show();
  void Clear();
  void Circle(int x, int y, float r, CRGB color);
  void SetPixel(int x, int y, CRGB color);
  void SetAll(CRGB color);
  void SetBrightness(uint8_t brightness);

 private:
  void CircleSetPix4(int x, int y, int delta_x, int delta_y, CRGB color);
  std::array<CRGB, kWidth * kHeight> leds_;
};

}  // namespace dinodisp
