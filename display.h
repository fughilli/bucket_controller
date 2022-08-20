#include <FastLED.h>

#include <array>

namespace bikedisp {

class Display {
 public:
  void Init();
  void Show();
  void Clear();
  void Circle(int x, int y, float r, CRGB color);
  void SetPixel(int x, int y, CRGB color);

 private:
  std::array<CRGB, 256> leds_;
};

}  // namespace bikedisp
