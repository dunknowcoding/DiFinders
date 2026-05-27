#pragma once

#include <Arduino.h>
#include <SPI.h>

#include "DiFindersConfig.h"

namespace DiFinders {
namespace detail {

/** VL53L1 / VL53L4 style 16-bit register index over SPI (ST SW model). */
inline bool spiWriteReg8(uint8_t csPin, uint16_t reg, uint8_t value, uint32_t hz = 2000000UL) {
  SPI.beginTransaction(SPISettings(hz, MSBFIRST, SPI_MODE0));
  digitalWrite(csPin, LOW);
  SPI.transfer(static_cast<uint8_t>((reg >> 8) & 0x7FU));
  SPI.transfer(static_cast<uint8_t>(reg & 0xFFU));
  SPI.transfer(value);
  digitalWrite(csPin, HIGH);
  SPI.endTransaction();
  return true;
}

inline bool spiWriteReg16(uint8_t csPin, uint16_t reg, uint16_t value, uint32_t hz = 2000000UL) {
  SPI.beginTransaction(SPISettings(hz, MSBFIRST, SPI_MODE0));
  digitalWrite(csPin, LOW);
  SPI.transfer(static_cast<uint8_t>((reg >> 8) & 0x7FU));
  SPI.transfer(static_cast<uint8_t>(reg & 0xFFU));
  SPI.transfer(static_cast<uint8_t>(value >> 8));
  SPI.transfer(static_cast<uint8_t>(value & 0xFFU));
  digitalWrite(csPin, HIGH);
  SPI.endTransaction();
  return true;
}

inline bool spiReadReg8(uint8_t csPin, uint16_t reg, uint8_t& value, uint32_t hz = 2000000UL) {
  SPI.beginTransaction(SPISettings(hz, MSBFIRST, SPI_MODE0));
  digitalWrite(csPin, LOW);
  SPI.transfer(static_cast<uint8_t>(((reg >> 8) & 0x7FU) | 0x80U));
  SPI.transfer(static_cast<uint8_t>(reg & 0xFFU));
  value = static_cast<uint8_t>(SPI.transfer(0x00));
  digitalWrite(csPin, HIGH);
  SPI.endTransaction();
  return true;
}

inline bool spiReadReg16(uint8_t csPin, uint16_t reg, uint16_t& value, uint32_t hz = 2000000UL) {
  uint8_t hi = 0;
  uint8_t lo = 0;
  SPI.beginTransaction(SPISettings(hz, MSBFIRST, SPI_MODE0));
  digitalWrite(csPin, LOW);
  SPI.transfer(static_cast<uint8_t>(((reg >> 8) & 0x7FU) | 0x80U));
  SPI.transfer(static_cast<uint8_t>(reg & 0xFFU));
  hi = static_cast<uint8_t>(SPI.transfer(0x00));
  lo = static_cast<uint8_t>(SPI.transfer(0x00));
  digitalWrite(csPin, HIGH);
  SPI.endTransaction();
  value = static_cast<uint16_t>((static_cast<uint16_t>(hi) << 8) | lo);
  return true;
}

inline bool spiReadMulti(uint8_t csPin, uint16_t reg, uint8_t* buffer, uint8_t length, uint32_t hz = 2000000UL) {
  SPI.beginTransaction(SPISettings(hz, MSBFIRST, SPI_MODE0));
  digitalWrite(csPin, LOW);
  SPI.transfer(static_cast<uint8_t>(((reg >> 8) & 0x7FU) | 0x80U));
  SPI.transfer(static_cast<uint8_t>(reg & 0xFFU));
#if DI_FINDERS_ENABLE_DMA && defined(ARDUINO_ARCH_ESP32)
  if (length > 0U) {
    SPI.transferBytes(nullptr, buffer, length);
  }
#else
  for (uint8_t i = 0; i < length; ++i) {
    buffer[i] = static_cast<uint8_t>(SPI.transfer(0x00));
  }
#endif
  digitalWrite(csPin, HIGH);
  SPI.endTransaction();
  return true;
}

}  // namespace detail
}  // namespace DiFinders
