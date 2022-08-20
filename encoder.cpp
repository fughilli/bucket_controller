#include "encoder.h"

#include <initializer_list>

#include "driver/gpio.h"
#include "driver/pcnt.h"

namespace bikedisp {

constexpr static int16_t kUpperLimit = 255;
constexpr static int16_t kLowerLimit = -256;

Encoder* encoder_table[PCNT_UNIT_MAX]{};

void Encoder::IsrHandlerStatic(void* arg) {
  const uint32_t interrupt_status = PCNT.int_st.val;
  for (int i = 0; i < PCNT_UNIT_MAX; ++i) {
    if (interrupt_status & (1 << i)) {
      if (encoder_table[i] != nullptr) {
        encoder_table[i]->IsrHandler();
      }
    }
  }
  PCNT.int_clr.val = interrupt_status;
}

void Encoder::MaybeRegisterIsr() {
  static bool registered = false;

  if (registered) return;

  pcnt_isr_register(Encoder::IsrHandlerStatic, nullptr, 0, nullptr);
  registered = true;
}

Encoder::Encoder(int pin_a, int pin_b, int pcnt_unit)
    : pin_a_(static_cast<gpio_num_t>(pin_a)),
      pin_b_(static_cast<gpio_num_t>(pin_b)),
      pcnt_unit_(static_cast<pcnt_unit_t>(pcnt_unit)) {
  encoder_table[pcnt_unit] = this;
}

void Encoder::IsrHandler() {
  if (PCNT.status_unit[pcnt_unit_].h_lim_lat) {
    count_offset_ += kUpperLimit;
  }
  if (PCNT.status_unit[pcnt_unit_].l_lim_lat) {
    count_offset_ += kLowerLimit;
  }
}

void Encoder::Enable() {
  MaybeRegisterIsr();

  for (gpio_num_t pin : {pin_a_, pin_b_}) {
    gpio_pad_select_gpio(pin);
    gpio_set_direction(pin, GPIO_MODE_INPUT);
    gpio_pullup_en(pin);
  }

  pcnt_config_t encoder_config = {
      .pulse_gpio_num = pin_a_,
      .ctrl_gpio_num = pin_b_,
      .lctrl_mode = PCNT_MODE_KEEP,
      .hctrl_mode = PCNT_MODE_REVERSE,
      .pos_mode = PCNT_COUNT_DEC,
      .neg_mode = PCNT_COUNT_INC,
      .counter_h_lim = kUpperLimit,
      .counter_l_lim = kLowerLimit,
      .unit = pcnt_unit_,
      .channel = PCNT_CHANNEL_0,
  };
  pcnt_unit_config(&encoder_config);
  encoder_config.pulse_gpio_num = pin_b_;
  encoder_config.ctrl_gpio_num = pin_a_;
  encoder_config.channel = PCNT_CHANNEL_1;
  encoder_config.lctrl_mode = PCNT_MODE_REVERSE;
  encoder_config.hctrl_mode = PCNT_MODE_KEEP;
  pcnt_unit_config(&encoder_config);

  pcnt_set_filter_value(pcnt_unit_, 250);
  pcnt_filter_enable(pcnt_unit_);

  pcnt_counter_pause(pcnt_unit_);
  pcnt_counter_clear(pcnt_unit_);

  pcnt_event_enable(pcnt_unit_, PCNT_EVT_H_LIM);
  pcnt_event_enable(pcnt_unit_, PCNT_EVT_L_LIM);

  pcnt_intr_enable(pcnt_unit_);
  pcnt_counter_resume(pcnt_unit_);
}

int64_t Encoder::Count() {
  int16_t count = 0;
  pcnt_get_counter_value(pcnt_unit_, &count);
  return count_offset_ + count;
}

}  // namespace bikedisp
