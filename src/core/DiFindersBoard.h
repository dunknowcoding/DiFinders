#pragma once

#include <Arduino.h>
#include <Wire.h>

namespace DiFinders {

/** Detected MCU family for wiring and capability hints (not a specific SKU). */
enum class BoardFamily : uint8_t {
  Unknown = 0,
  AvrUno,
  AvrNano,
  AvrMega,
  RenesasUnoR4,
  Esp32,
  Esp32S2,
  Esp32S3,
  Esp32C3,
  Esp32C6,
  Esp32H2,
  Esp32P4,
  Rp2040,
  Rp2350,
  SamdMkr,
  SamdNano33IoT,
  Nrf52840,
  Stm32,
  Mbed,
};

inline BoardFamily detectBoardFamily() {
#if defined(ARDUINO_UNOR4_WIFI) || defined(ARDUINO_UNOR4_MINIMA)
  return BoardFamily::RenesasUnoR4;
#elif defined(ARDUINO_AVR_NANO) || defined(ARDUINO_AVR_NANO_EVERY)
  return BoardFamily::AvrNano;
#elif defined(ARDUINO_AVR_MEGA2560) || defined(ARDUINO_AVR_MEGA)
  return BoardFamily::AvrMega;
#elif defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_MINI)
  return BoardFamily::AvrUno;
#elif defined(ARDUINO_ARCH_ESP32)
#if defined(CONFIG_IDF_TARGET_ESP32P4) || defined(ARDUINO_ESP32P4_DEV)
  return BoardFamily::Esp32P4;
#elif defined(CONFIG_IDF_TARGET_ESP32H2) || defined(ARDUINO_ESP32H2_DEV)
  return BoardFamily::Esp32H2;
#elif defined(CONFIG_IDF_TARGET_ESP32C6)
  return BoardFamily::Esp32C6;
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
  return BoardFamily::Esp32C3;
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
  return BoardFamily::Esp32S2;
#elif defined(CONFIG_IDF_TARGET_ESP32S3) || defined(ARDUINO_ESP32S3_DEV)
  return BoardFamily::Esp32S3;
#else
  return BoardFamily::Esp32;
#endif
#elif defined(ARDUINO_ARCH_RP2040)
#if defined(ARDUINO_ARCH_RP2350) || defined(PICO_RP2350)
  return BoardFamily::Rp2350;
#else
  return BoardFamily::Rp2040;
#endif
#elif defined(ARDUINO_ARCH_NRF52840) || defined(ARDUINO_NRF52_ADAFRUIT_FEATHER) || defined(ARDUINO_NANO33BLE)
  return BoardFamily::Nrf52840;
#elif defined(ARDUINO_ARCH_SAMD)
#if defined(ARDUINO_SAMD_NANO_33_IOT)
  return BoardFamily::SamdNano33IoT;
#else
  return BoardFamily::SamdMkr;
#endif
#elif defined(ARDUINO_ARCH_STM32)
  return BoardFamily::Stm32;
#elif defined(ARDUINO_ARCH_MBED)
  return BoardFamily::Mbed;
#else
  return BoardFamily::Unknown;
#endif
}

inline const char* boardFamilyName(BoardFamily family) {
  switch (family) {
    case BoardFamily::AvrUno:
      return "avr-uno";
    case BoardFamily::AvrNano:
      return "avr-nano";
    case BoardFamily::AvrMega:
      return "avr-mega";
    case BoardFamily::RenesasUnoR4:
      return "renesas-uno-r4";
    case BoardFamily::Esp32:
      return "esp32";
    case BoardFamily::Esp32S2:
      return "esp32-s2";
    case BoardFamily::Esp32S3:
      return "esp32-s3";
    case BoardFamily::Esp32C3:
      return "esp32-c3";
    case BoardFamily::Esp32C6:
      return "esp32-c6";
    case BoardFamily::Esp32H2:
      return "esp32-h2";
    case BoardFamily::Esp32P4:
      return "esp32-p4";
    case BoardFamily::Rp2040:
      return "rp2040";
    case BoardFamily::Rp2350:
      return "rp2350";
    case BoardFamily::SamdMkr:
      return "samd-mkr";
    case BoardFamily::SamdNano33IoT:
      return "samd-nano33-iot";
    case BoardFamily::Nrf52840:
      return "nrf52840";
    case BoardFamily::Stm32:
      return "stm32";
    case BoardFamily::Mbed:
      return "mbed";
    default:
      return "unknown";
  }
}

inline const char* boardFamilyName() {
  return boardFamilyName(detectBoardFamily());
}

/** 3.3 V logic for I2C/UART sensor IO (level-shift when false on 5 V AVR). */
inline bool boardIs3v3Logic() {
  switch (detectBoardFamily()) {
    case BoardFamily::AvrUno:
    case BoardFamily::AvrMega:
      return false;
    default:
      return true;
  }
}

/** Classic AVR UNO/Nano: no hardware Serial1. */
inline bool hasHardwareSerial1() {
#if defined(HAVE_HWSERIAL1) || defined(SERIAL_PORT_HARDWARE1)
  return true;
#else
  return false;
#endif
}

inline bool hasHardwareSerial2() {
#if defined(HAVE_HWSERIAL2) || defined(SERIAL_PORT_HARDWARE2)
  return true;
#else
  return false;
#endif
}

/**
 * Preferred hardware UART for sensor modules (not USB Serial).
 * Returns nullptr if this board has no spare hardware UART.
 */
inline HardwareSerial* sensorUartPort() {
#if defined(HAVE_HWSERIAL1) || defined(SERIAL_PORT_HARDWARE1)
  return &Serial1;
#elif defined(HAVE_HWSERIAL2) || defined(SERIAL_PORT_HARDWARE2)
  return &Serial2;
#else
  return nullptr;
#endif
}

/** Suggested I2C clock (Hz) for ToF breakouts on this MCU. */
inline uint32_t defaultI2cClockHz() {
#if defined(ARDUINO_ARCH_AVR)
  return 100000UL;
#elif defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_NRF52840)
  return 400000UL;
#else
  return 400000UL;
#endif
}

inline void applyDefaultI2cClock(TwoWire& bus = Wire) {
  bus.setClock(defaultI2cClockHz());
}

/** Print one-line board + capability summary on USB Serial (bring-up helper). */
inline void printBoardInfo(Print& out = Serial) {
  out.print(F("board="));
  out.print(boardFamilyName());
  out.print(F(" serial1="));
  out.print(hasHardwareSerial1() ? 1 : 0);
  out.print(F(" logic3v3="));
  out.print(boardIs3v3Logic() ? 1 : 0);
  out.print(F(" i2c_hz="));
  out.println(defaultI2cClockHz());
}

}  // namespace DiFinders
