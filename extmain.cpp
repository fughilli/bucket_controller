#include "extmain.h"

//#include <WiFi.h>
//#include <WiFiAP.h>
//#include <WiFiClient.h>
#include <EEPROM.h>
#include <stdint.h>

#include <array>
#include <cmath>
#include <limits>

#include "FastLED.h"
#include "HardwareSerial.h"
#include "display.h"
#include "hsv2rgb.h"

namespace dinodisp {

// WiFiServer server(80);
Display display;

enum class Pattern {
  kRainbow,
  kRainbowMedium,
  kRainbowFast,
  kRedMarching,
  kRedSpinning,
  kRed,
  kDenseLastValue = kRed
};

template <typename E>
E EnumNext(E val) {
  if (val == E::kDenseLastValue) {
    return static_cast<E>(0);
  }
  return static_cast<E>(static_cast<int>(val) + 1);
}

Pattern current_pattern = Pattern::kRed;

constexpr uint32_t kMagic = 0x04d3d3d3;

struct PatternMemory {
  uint32_t magic;
  Pattern pattern;
};

template <typename T>
void read_from_eeprom(size_t offset, T* out) {
  for (size_t i = 0; i < sizeof(T); ++i) {
    uint8_t val = EEPROM.read(offset + i);
    Serial.print("Read value ");
    Serial.println((int)val);
    *(reinterpret_cast<uint8_t*>(out) + i) = val;
  }
}

template <typename T>
void write_to_eeprom(size_t offset, const T& val) {
  for (size_t i = 0; i < sizeof(T); ++i) {
    uint8_t val_to_write = reinterpret_cast<const uint8_t*>(&val)[i];
    Serial.print("Write value ");
    Serial.println((int)val_to_write);
    EEPROM.write(offset + i, val_to_write);
  }
  EEPROM.commit();
}

void update_pattern_from_eeprom() {
  PatternMemory temp;
  read_from_eeprom(0, &temp);
  if (temp.magic != kMagic) {
    Serial.println("Magic did not match.");
    temp.magic = kMagic;
    temp.pattern = current_pattern;
    write_to_eeprom(0, temp);
    Serial.println("Post-write:");
    read_from_eeprom(0, &temp);
  } else {
    Serial.println("Magic matched.");
    current_pattern = temp.pattern;
    Serial.print("Pattern = ");
    Serial.println((int)current_pattern);
    temp.pattern = EnumNext(temp.pattern);
    Serial.print("Next pattern = ");
    Serial.println((int)temp.pattern);
    write_to_eeprom(0, temp);
  }
}

int triangle(int arg, int amplitude) {
  int period_arg = (arg % (amplitude * 2));
  if (period_arg >= amplitude) {
    return 2 * amplitude - period_arg;
  } else {
    return period_arg;
  }
}

void ExtMain() {
  EEPROM.begin(128);
  Serial.begin(115200);

  delay(1000);

  display.Init();
  display.SetBrightness(20);

  // WiFi.softAP("Paintbrush");
  // IPAddress myIP = WiFi.softAPIP();

  // Serial.println();
  // Serial.print("My IP address:");
  // Serial.println(myIP);

  // server.begin();
  //
  update_pattern_from_eeprom();
}

void ExtLoop() {
  static uint64_t i = 0;

  // WiFiClient client = server.available();

  // if (client) {
  //  String request = "";
  //  Serial.println("New client");
  //  while (client.connected()) {
  //    if (client.available()) {
  //      const char c = client.read();
  //      Serial.print(c);
  //      if (c == '\n') {
  //        if (request.length() == 0) {
  //          client.println("HTTP/1.1 200 OK");
  //          client.println("Content-type:text/html");
  //          client.println();
  //          client.print("Hello world!");
  //          client.println();
  //          break;
  //        } else {
  //          request = "";
  //        }
  //      } else if (c != '\r') {
  //        request += c;
  //      }

  //      if (request.endsWith("GET /red")) {
  //        current_pattern = Pattern::kRed;
  //      }
  //      if (request.endsWith("GET /rainbow")) {
  //        current_pattern = Pattern::kRainbow;
  //      }
  //    }
  //  }
  //  client.stop();
  //}

  CRGB color;

  switch (current_pattern) {
    case Pattern::kRainbow:
      for (uint32_t j = 0; j < display.kWidth; ++j) {
        hsv2rgb_rainbow({((j * 255 + i * 10) / display.kWidth), 255, 255},
                        color);
        display.SetPixel(j, 0, color);
      }
      break;
    case Pattern::kRainbowMedium:
      for (uint32_t j = 0; j < display.kWidth; ++j) {
        hsv2rgb_rainbow({((j * 255 + i * 30) / display.kWidth), 255, 255},
                        color);
        display.SetPixel(j, 0, color);
      }
      break;
      ;
    case Pattern::kRainbowFast:
      for (uint32_t j = 0; j < display.kWidth; ++j) {
        hsv2rgb_rainbow({((j * 255 + i * 60) / display.kWidth), 255, 255},
                        color);
        display.SetPixel(j, 0, color);
      }
      break;
    case Pattern::kRedMarching: {
      int length = triangle(i / 150, 6) + 2;
      for (uint32_t j = 0; j < display.kWidth; ++j) {
        if ((j + i / 30) % length < (length / 2)) {
          hsv2rgb_rainbow({0, 255, 255}, color);
          display.SetPixel(j, 0, color);
        } else {
          display.SetPixel(j, 0, {0, 0, 0});
        }
      }
      break;
    }
    case Pattern::kRedSpinning:
      for (uint32_t j = 0; j < display.kWidth; ++j) {
        if ((i / 10 + j) % 8 < 4) {
          hsv2rgb_rainbow({0, 255, 255}, color);
          display.SetPixel(j, 0, color);
        } else {
          display.SetPixel(j, 0, {0, 0, 0});
        }
      }
      break;
    case Pattern::kRed:
      for (uint32_t j = 0; j < display.kWidth; ++j) {
        hsv2rgb_rainbow({0, 255, 100 + (sin(i / 500.0f) + 1) / 2 * 155}, color);
        display.SetPixel(j, 0, color);
      }
      break;
  }

  display.Show();

  i += 1;
}

}  // namespace dinodisp
