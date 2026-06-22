#pragma once

#include <Arduino.h>

/**
 * Nordic nRF52 / nRF53 family detection for DiFinders board helpers.
 *
 * Covers:
 * - ArduinoNRF board package (ARDUINO_ARCH_NRF52)
 * - Adafruit nRF52 BSP (ARDUINO_ARCH_NRF52 / ARDUINO_NRF52_ADAFRUIT_FEATHER)
 * - Arduino Mbed Nano 33 BLE (ARDUINO_NANO33BLE / ARDUINO_ARCH_NRF52840)
 * - Nordic / Adafruit nRF5340 application cores (ARDUINO_ARCH_NRF53)
 */
#if defined(ARDUINO_ARCH_NRF53) || defined(NRF5340_XXAA) || defined(NRF5340_XXAA_APPLICATION) \
    || defined(ARDUINO_NRF5340_XXAA)
#define DIFINDERS_IS_NRF53 1
#else
#define DIFINDERS_IS_NRF53 0
#endif

#if defined(ARDUINO_ARCH_NRF52) || defined(ARDUINO_ARCH_NRF52840) \
    || defined(ARDUINO_NRF52_ADAFRUIT_FEATHER) || defined(ARDUINO_NANO33BLE) \
    || defined(NRF52840_XXAA)
#define DIFINDERS_IS_NRF52 1
#else
#define DIFINDERS_IS_NRF52 0
#endif

#if DIFINDERS_IS_NRF52 || DIFINDERS_IS_NRF53
#define DIFINDERS_IS_NRF 1
#else
#define DIFINDERS_IS_NRF 0
#endif

namespace DiFinders {

inline bool isNrf53Board() {
#if DIFINDERS_IS_NRF53
  return true;
#else
  return false;
#endif
}

inline bool isNrf52Board() {
#if DIFINDERS_IS_NRF52
  return true;
#else
  return false;
#endif
}

inline bool isNrfBoard() {
  return isNrf52Board() || isNrf53Board();
}

}  // namespace DiFinders
