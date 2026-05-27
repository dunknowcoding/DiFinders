#pragma once

#include <Arduino.h>
#include <Wire.h>

#include "../../core/DiFindersI2C.h"
#include "../../core/DiFindersTypes.h"

namespace DiFinders {

class Gp2Y0E03Sensor {
 public:
  static constexpr uint8_t kDefaultAddress = 0x40;
  static constexpr uint8_t kShiftRegister = 0x35;
  static constexpr uint8_t kDistanceRegister = 0x5E;
  static constexpr uint16_t kMinDistanceMm = 40;
  static constexpr uint16_t kMaxDistanceMm = 5000;

  Gp2Y0E03Sensor() = default;
  explicit Gp2Y0E03Sensor(uint8_t address) : address_(address) {}

  void begin(uint8_t address = kDefaultAddress, TwoWire* wire = &Wire) {
    address_ = address;
    wire_ = wire;
    refreshShiftBit();
  }

  void beginAnalog(uint8_t analogPin, uint16_t adcReferenceMv = 3300, uint16_t adcMax = 1023) {
    analogPin_ = analogPin;
    adcReferenceMv_ = adcReferenceMv;
    adcMax_ = adcMax == 0 ? 1 : adcMax;
    pinMode(analogPin_, INPUT);
    analogReady_ = true;
  }

  bool ready() const {
    return (wire_ != nullptr && shiftLoaded_) || analogReady_;
  }

  bool i2cReady() const {
    return wire_ != nullptr && shiftLoaded_;
  }

  bool analogReady() const {
    return analogReady_;
  }

  void calibrateAnalog(uint16_t voutNearMv, uint16_t voutFarMv, uint16_t distNearMm, uint16_t distFarMm) {
    analogNearMv_ = voutNearMv;
    analogFarMv_ = voutFarMv;
    analogNearMm_ = distNearMm;
    analogFarMm_ = distFarMm;
    analogCalibrated_ = voutNearMv != voutFarMv;
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
    wire_->write(kDistanceRegister);
    status = wire_->endTransmission(false);
    if (status != 0 || wire_->requestFrom(address_, static_cast<uint8_t>(2)) != 2) {
      detail::discardPendingWireBytes(wire_);
      reading.status = SensorStatus::Error;
      lastReading_ = reading;
      return reading;
    }

    const uint8_t high = static_cast<uint8_t>(wire_->read());
    const uint8_t low = static_cast<uint8_t>(wire_->read());

    const uint16_t raw = static_cast<uint16_t>(((static_cast<uint16_t>(high) << 4) | (low & 0x0F)));
    uint32_t distanceCm = raw;
    if (shiftBit_ > 0) {
      distanceCm >>= shiftBit_;
    }
    distanceCm /= 16U;
    reading.distanceMm = static_cast<uint16_t>(distanceCm * 10U);
    reading.rawValue = raw;

    if (distanceCm == 0) {
      reading.status = SensorStatus::OutOfRange;
    } else if (reading.distanceMm < kMinDistanceMm || reading.distanceMm > kMaxDistanceMm) {
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

  RangeReading readAnalog() {
    RangeReading reading;
    reading.timestampMs = millis();
    if (!analogReady_) {
      reading.status = SensorStatus::NotReady;
      lastAnalogReading_ = reading;
      return reading;
    }

    const uint16_t raw = static_cast<uint16_t>(analogRead(analogPin_));
    const uint32_t voutMv = (static_cast<uint32_t>(raw) * adcReferenceMv_) / adcMax_;
    reading.rawValue = static_cast<uint16_t>(voutMv > 65535UL ? 65535UL : voutMv);

    if (!analogCalibrated_) {
      reading.status = SensorStatus::NotReady;
      lastAnalogReading_ = reading;
      return reading;
    }

    int32_t distanceMm = 0;
    if (voutMv <= analogNearMv_) {
      distanceMm = static_cast<int32_t>(analogNearMm_);
    } else if (voutMv >= analogFarMv_) {
      distanceMm = static_cast<int32_t>(analogFarMm_);
    } else {
      const int32_t spanMv = static_cast<int32_t>(analogFarMv_) - static_cast<int32_t>(analogNearMv_);
      const int32_t spanMm = static_cast<int32_t>(analogFarMm_) - static_cast<int32_t>(analogNearMm_);
      distanceMm = analogNearMm_ + ((static_cast<int32_t>(voutMv) - static_cast<int32_t>(analogNearMv_)) * spanMm) / spanMv;
    }

    if (distanceMm < 0) {
      distanceMm = 0;
    }
    reading.distanceMm = static_cast<uint16_t>(distanceMm > 65535 ? 65535 : distanceMm);
    reading.status = SensorStatus::Ok;
    lastAnalogReading_ = reading;
    return reading;
  }

  uint8_t shiftBit() const {
    return shiftBit_;
  }

  SensorProbe probe() const {
    SensorProbe info;
    info.kind = SensorKind::Range;
    info.busType = i2cReady() ? SensorBusType::I2c : SensorBusType::Analog;
    info.configured = ready();
    info.address = address_;
    info.primaryPin = analogPin_;
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
    report.detail = shiftBit_;
    return report;
  }

  const RangeReading& lastReading() const {
    return lastReading_;
  }

  const RangeReading& lastAnalogReading() const {
    return lastAnalogReading_;
  }

 private:
  void refreshShiftBit() {
    uint8_t status = 4;
    wire_->beginTransmission(address_);
    wire_->write(kShiftRegister);
    status = wire_->endTransmission(false);
    if (status == 0 && wire_->requestFrom(address_, static_cast<uint8_t>(1)) == 1) {
      shiftBit_ = static_cast<uint8_t>(wire_->read());
      shiftLoaded_ = true;
      return;
    }
    detail::discardPendingWireBytes(wire_);
    shiftBit_ = 0;
    shiftLoaded_ = false;
  }

  TwoWire* wire_ = &Wire;
  uint8_t address_ = kDefaultAddress;
  uint8_t shiftBit_ = 0;
  bool shiftLoaded_ = false;
  uint8_t analogPin_ = 0xFF;
  uint16_t adcReferenceMv_ = 3300;
  uint16_t adcMax_ = 1023;
  bool analogReady_ = false;
  bool analogCalibrated_ = false;
  uint16_t analogNearMv_ = 480;
  uint16_t analogFarMv_ = 290;
  uint16_t analogNearMm_ = 50;
  uint16_t analogFarMm_ = 300;
  RangeReading lastReading_;
  RangeReading lastAnalogReading_;
};

}  // namespace DiFinders
