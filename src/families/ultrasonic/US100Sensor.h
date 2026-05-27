#pragma once

#include <Arduino.h>
#include <Stream.h>

#include "../../core/DiFindersTypes.h"

namespace DiFinders {

class US100Sensor {
 public:
  US100Sensor() = default;
  explicit US100Sensor(Stream& serial) { begin(serial); }

  void begin(Stream& serial) {
    serial_ = &serial;
  }

  bool ready() const {
    return serial_ != nullptr;
  }

  void setTimeout(uint16_t timeoutMs) {
    timeoutMs_ = timeoutMs;
  }

  RangeReading read() {
    RangeReading reading;
    reading.timestampMs = millis();
    if (!ready()) {
      reading.status = SensorStatus::NotReady;
      lastReading_ = reading;
      return reading;
    }

    flushInput();
    serial_->write(0x55);
    uint32_t start = millis();
    while (serial_->available() < 2) {
      if (expired(start)) {
        reading.status = SensorStatus::Timeout;
        lastReading_ = reading;
        return reading;
      }
    }

    uint16_t high = static_cast<uint8_t>(serial_->read());
    uint16_t low = static_cast<uint8_t>(serial_->read());
    reading.distanceMm = static_cast<uint16_t>((high << 8) | low);
    reading.rawValue = reading.distanceMm;
    reading.status = reading.distanceMm == 0 ? SensorStatus::OutOfRange : SensorStatus::Ok;
    lastReading_ = reading;
    return reading;
  }

  uint16_t millimeters() {
    return read().distanceMm;
  }

  int16_t readTemperatureC() {
    if (!ready()) {
      return kInvalidTemperature;
    }
    flushInput();
    serial_->write(0x50);
    uint32_t start = millis();
    while (serial_->available() < 1) {
      delay(1);
      if (expired(start)) {
        return kInvalidTemperature;
      }
    }
    int16_t raw = static_cast<uint8_t>(serial_->read());
    lastTemperatureC_ = raw - 45;
    return lastTemperatureC_;
  }

  SensorProbe probe() const {
    SensorProbe info;
    info.kind = SensorKind::Range;
    info.busType = SensorBusType::Uart;
    info.configured = ready();
    info.supportsTemperatureReading = true;
    return info;
  }

  SelfTestReport selfTest() {
    SelfTestReport report;
    report.passed = ready();
    report.status = ready() ? SensorStatus::Ok : SensorStatus::NotReady;
    report.value = lastReading_.distanceMm;
    return report;
  }

  const RangeReading& lastReading() const {
    return lastReading_;
  }

  int16_t lastTemperatureC() const {
    return lastTemperatureC_;
  }

 private:
  static constexpr int16_t kInvalidTemperature = -1000;

  void flushInput() {
    while (serial_ != nullptr && serial_->available() > 0) {
      serial_->read();
    }
  }

  bool expired(uint32_t start) const {
    return timeoutMs_ > 0 && (millis() - start) > timeoutMs_;
  }

  Stream* serial_ = nullptr;
  uint16_t timeoutMs_ = 150;
  RangeReading lastReading_;
  int16_t lastTemperatureC_ = kInvalidTemperature;
};

}  // namespace DiFinders
