#pragma once

#include <Arduino.h>

#include "DiFindersConfig.h"
#include "DiFindersTypes.h"

namespace DiFinders {

/**
 * DMA / high-throughput bus helpers.
 *
 * Arduino Wire/Stream APIs do not expose I2C DMA on all cores. Where the MCU
 * supports it, DiFinders uses hardware SPI burst (ESP32 transferBytes) or large
 * UART RX buffers for sustained sensor rates.
 */

inline bool dmaPlatformSupported() {
#if DI_FINDERS_ENABLE_DMA && (defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_STM32))
  return true;
#else
  return false;
#endif
}

inline bool busSupportsDma(SensorBusType bus) {
  switch (bus) {
    case SensorBusType::Spi:
    case SensorBusType::Uart:
      return dmaPlatformSupported();
    case SensorBusType::I2c:
      return false;
    default:
      return false;
  }
}

inline const char* dmaModeName() {
#if DI_FINDERS_ENABLE_DMA && defined(ARDUINO_ARCH_ESP32)
  return "esp32-spi-burst/uart-rxbuf";
#elif DI_FINDERS_ENABLE_DMA && defined(ARDUINO_ARCH_RP2040)
  return "rp2040-spi-burst";
#elif DI_FINDERS_ENABLE_DMA && defined(ARDUINO_ARCH_STM32)
  return "stm32-spi-burst";
#else
  return "none";
#endif
}

}  // namespace DiFinders
