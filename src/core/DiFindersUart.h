#pragma once

#include <Arduino.h>

#include "DiFindersBoard.h"
#include "DiFindersConfig.h"

namespace DiFinders {

/** Expand UART RX ring for high-baud mmWave / laser modules (ESP32). */
inline void configureSensorUartHighThroughput(HardwareSerial& uart, size_t rxBytes = 4096) {
#if DI_FINDERS_ENABLE_DMA && defined(ARDUINO_ARCH_ESP32)
  uart.setRxBufferSize(rxBytes);
#else
  (void)uart;
  (void)rxBytes;
#endif
}

/**
 * Begin sensor UART at baud on the board's default sensor port.
 * Returns false if no hardware UART (e.g. AVR UNO).
 */
inline bool beginSensorUart(uint32_t baud, HardwareSerial** outPort = nullptr) {
  HardwareSerial* port = sensorUartPort();
  if (port == nullptr) {
    return false;
  }
#if DI_FINDERS_ENABLE_DMA && defined(ARDUINO_ARCH_ESP32)
  configureSensorUartHighThroughput(*port);
#endif
  port->begin(baud);
  if (outPort != nullptr) {
    *outPort = port;
  }
  return true;
}

}  // namespace DiFinders
