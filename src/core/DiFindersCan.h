#pragma once

#include <Arduino.h>

#include "DiFindersTypes.h"

namespace DiFinders {

/** Normalized CAN 2.0 frame for sensor parsers (controller-agnostic). */
struct CanFrame {
  uint32_t id = 0;
  uint8_t dlc = 0;
  uint8_t data[8] = {0};
  bool extended = false;
};

inline bool canFrameFromRaw(CanFrame& out, uint32_t id, const uint8_t* data, uint8_t dlc, bool extended = false) {
  if (data == nullptr || dlc > 8) {
    return false;
  }
  out.id = id;
  out.dlc = dlc;
  out.extended = extended;
  for (uint8_t i = 0; i < dlc; ++i) {
    out.data[i] = data[i];
  }
  for (uint8_t i = dlc; i < 8; ++i) {
    out.data[i] = 0;
  }
  return true;
}

/** Human-readable bus label for probe() / debug. */
inline const char* canControllerLabel() {
#if defined(ARDUINO_ARCH_ESP32)
  return "esp32-twai";
#elif defined(ARDUINO_ARCH_RP2040)
  return "rp2040-can";
#elif defined(ARDUINO_ARCH_SAMD) && defined(ARDUINO_SAMD_MKR1000)
  return "mkr-can";
#else
  return "mcp2515-spi";
#endif
}

}  // namespace DiFinders
