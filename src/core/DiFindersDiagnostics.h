#pragma once

#include <Arduino.h>

#include "DiFindersTypes.h"

namespace DiFinders {

/** Runtime health snapshot (all sensor families). */
struct SensorHealth {
  bool ready = false;
  bool passed = false;
  SensorBusType bus = SensorBusType::Unknown;
  SensorStatus lastStatus = SensorStatus::NotReady;
  uint8_t busErrorCode = 0;
  uint16_t lastValue = 0;
  uint32_t faultCount = 0;
};

inline SensorHealth makeHealth(bool ready, SensorBusType bus, SensorStatus lastStatus, uint8_t busError = 0,
                               uint16_t lastValue = 0, uint32_t faultCount = 0) {
  SensorHealth h;
  h.ready = ready;
  h.bus = bus;
  h.lastStatus = lastStatus;
  h.busErrorCode = busError;
  h.lastValue = lastValue;
  h.faultCount = faultCount;
  h.passed = ready && (lastStatus == SensorStatus::Ok || lastStatus == SensorStatus::OutOfRange);
  return h;
}

inline void printSensorHealth(Print& out, const SensorHealth& h) {
  out.print(F("ready="));
  out.print(h.ready ? 1 : 0);
  out.print(F(" bus="));
  out.print(sensorBusTypeName(h.bus));
  out.print(F(" status="));
  out.print(static_cast<uint8_t>(h.lastStatus));
  out.print(F(" bus_err="));
  out.print(h.busErrorCode);
  out.print(F(" passed="));
  out.println(h.passed ? 1 : 0);
}

}  // namespace DiFinders
