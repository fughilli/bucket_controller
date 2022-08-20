#include <stdint.h>

#include "driver/gpio.h"
#include "driver/pcnt.h"

namespace bikedisp {
class Encoder {
 public:
  Encoder(int pin_a, int pin_b, int pcnt_unit);
  void Enable();

  int64_t Count();

 private:
  static void MaybeRegisterIsr();
  static void IsrHandlerStatic(void* arg);
  void IsrHandler();

  gpio_num_t pin_a_, pin_b_;
  pcnt_unit_t pcnt_unit_;
  int64_t count_offset_ = 0;
};
}  // namespace bikedisp
