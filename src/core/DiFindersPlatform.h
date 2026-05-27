#pragma once

#include <Arduino.h>

#include "DiFindersConfig.h"

namespace DiFinders {

inline const char* platformName() {
#if defined(ARDUINO_ARCH_RP2040)
  return "rp2040";
#elif defined(ARDUINO_ARCH_ESP32)
  return "esp32";
#elif defined(ARDUINO_ARCH_STM32)
  return "stm32";
#elif defined(ARDUINO_ARCH_AVR)
  return "avr";
#else
  return "arduino";
#endif
}

inline uint16_t defaultAdcMax() {
#if defined(ARDUINO_ARCH_ESP32)
  return 4095;
#elif defined(ARDUINO_ARCH_STM32)
  return 4095;
#elif defined(ARDUINO_ARCH_RP2040)
  return 4095;
#else
  return DI_FINDERS_DEFAULT_ADC_MAX;
#endif
}

inline uint16_t defaultAdcReferenceMv() {
#if defined(ARDUINO_ARCH_AVR)
  return 5000;
#elif defined(ARDUINO_ARCH_ESP32)
  return 3300;
#elif defined(ARDUINO_ARCH_STM32)
  return 3300;
#elif defined(ARDUINO_ARCH_RP2040)
  return 3300;
#else
  return 3300;
#endif
}

inline bool hasWideAdc() {
  return defaultAdcMax() > 1023;
}

}  // namespace DiFinders