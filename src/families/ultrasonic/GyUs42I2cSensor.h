#pragma once

#include <Arduino.h>
#include <Wire.h>

#include "../../core/DiFindersDiagnostics.h"
#include "../../core/DiFindersI2C.h"
#include "../../core/DiFindersTypes.h"

namespace DiFinders {

class GyUs42I2cSensor {
 public:
  static constexpr uint8_t kDefaultAddress = 0x70;
  static constexpr uint8_t kRangeCommand = 0x51;
  static constexpr uint8_t kChangeAddressCommand1 = 0xAA;
  static constexpr uint8_t kChangeAddressCommand2 = 0xA5;
  static constexpr uint16_t kBlindZoneMm = 200;
  static constexpr uint16_t kMaxDistanceMm = 7200;

  GyUs42I2cSensor() = default;
  explicit GyUs42I2cSensor(uint8_t address) : address_(address) {}

  void begin(uint8_t address = kDefaultAddress, TwoWire* wire = &Wire) {
    address_ = address;
    wire_ = wire;
  }

  bool ready() const {
    return wire_ != nullptr;
  }

  void setMeasureDelayMs(uint16_t delayMs) {
    measureDelayMs_ = delayMs;
  }

  RangeReading read() {
    RangeReading reading;
    reading.timestampMs = millis();
    if (!ready()) {
      reading.status = SensorStatus::NotReady;
      lastReading_ = reading;
      return reading;
    }

    uint8_t status = 4;
    wire_->beginTransmission(address_);
    wire_->write(kRangeCommand);
    status = wire_->endTransmission();
    lastI2cStatus_ = status;
    if (status != 0) {
      reading.status = SensorStatus::Error;
      lastReading_ = reading;
      return reading;
    }

    delay(measureDelayMs_);

    if (wire_->requestFrom(address_, static_cast<uint8_t>(2)) != 2) {
      lastI2cStatus_ = 4;
      detail::discardPendingWireBytes(wire_);
      reading.status = SensorStatus::Timeout;
      lastReading_ = reading;
      return reading;
    }

    const uint8_t high = static_cast<uint8_t>(wire_->read());
    const uint8_t low = static_cast<uint8_t>(wire_->read());

    const uint16_t distanceCm = static_cast<uint16_t>((static_cast<uint16_t>(high) << 8) | low);
    reading.distanceMm = static_cast<uint16_t>(distanceCm * 10U);
    reading.rawValue = distanceCm;
    if (distanceCm == 0) {
      reading.status = SensorStatus::Timeout;
    } else if (reading.distanceMm < kBlindZoneMm) {
      reading.status = SensorStatus::OutOfRange;
    } else if (reading.distanceMm > kMaxDistanceMm) {
      reading.status = SensorStatus::OutOfRange;
    } else {
      reading.status = SensorStatus::Ok;
    }

    lastReading_ = reading;
    return reading;
  }

  uint16_t millimeters() {
    return read().distanceMm;
  }

  bool changeAddress(uint8_t newAddress) {
    if (!ready() || newAddress == 0 || newAddress > 0x7F) {
      return false;
    }
    uint8_t status = 4;
    wire_->beginTransmission(address_);
    wire_->write(kChangeAddressCommand1);
    wire_->write(kChangeAddressCommand2);
    wire_->write(static_cast<uint8_t>(newAddress << 1));
    status = wire_->endTransmission();
    if (status != 0) {
      return false;
    }
    address_ = newAddress;
    return true;
  }

  SensorProbe probe() const {
    SensorProbe info;
    info.kind = SensorKind::Range;
    info.busType = SensorBusType::I2c;
    info.configured = ready();
    info.address = address_;
    info.supportsAddressChange = true;
    return info;
  }

  SelfTestReport selfTest() {
    SelfTestReport report;
    report.passed = ready();
    if (!report.passed) {
      report.status = SensorStatus::NotReady;
      return report;
    }
    const RangeReading sample = read();
    report.status = sample.status;
    report.passed = sample.status == SensorStatus::Ok || sample.status == SensorStatus::OutOfRange;
    report.value = sample.distanceMm;
    return report;
  }

  const RangeReading& lastReading() const {
    return lastReading_;
  }

  uint8_t lastBusError() const {
    return lastI2cStatus_;
  }

  SensorHealth health() const {
    return makeHealth(ready(), SensorBusType::I2c, lastReading_.status, lastI2cStatus_, lastReading_.distanceMm, 0);
  }

 private:
  uint8_t lastI2cStatus_ = 0;
  TwoWire* wire_ = &Wire;
  uint8_t address_ = kDefaultAddress;
  uint16_t measureDelayMs_ = 30;
  RangeReading lastReading_;
};

}  // namespace DiFinders
