#pragma once

#include <Arduino.h>

#include "../../core/DiFindersTypes.h"

namespace DiFinders {

enum class SharpDigitalModel : uint8_t {
  GP2Y0D805Z0F,
  GP2Y0D810Z0F,
  GP2Y0D815Z0F,
};

struct SharpDigitalRange {
  uint16_t minDistanceMm;
  uint16_t maxDistanceMm;
};

class SharpDigitalDistanceSensor {
 public:
  SharpDigitalDistanceSensor() = default;

  SharpDigitalDistanceSensor(uint8_t outputPin, SharpDigitalModel model, bool activeLow = true) {
    begin(outputPin, model, activeLow);
  }

  void begin(uint8_t outputPin, SharpDigitalModel model, bool activeLow = true, uint8_t inputMode = INPUT) {
    outputPin_ = outputPin;
    model_ = model;
    activeLow_ = activeLow;
    pinMode(outputPin_, inputMode);
  }

  void attachEnablePin(uint8_t enablePin, bool activeHigh = true) {
    enablePin_ = enablePin;
    enableActiveHigh_ = activeHigh;
    pinMode(enablePin_, OUTPUT);
    enable();
  }

  void enable() {
    if (enablePin_ != kUnassignedPin) {
      digitalWrite(enablePin_, enableActiveHigh_ ? HIGH : LOW);
    }
    enabled_ = true;
  }

  void disable() {
    if (enablePin_ != kUnassignedPin) {
      digitalWrite(enablePin_, enableActiveHigh_ ? LOW : HIGH);
    }
    enabled_ = false;
  }

  bool enabled() const {
    return enabled_;
  }

  bool detected() const {
    if (!enabled_ || outputPin_ == kUnassignedPin) {
      return false;
    }
    bool raw = digitalRead(outputPin_) == HIGH;
    return activeLow_ ? !raw : raw;
  }

  ProximityReading read() {
    ProximityReading reading;
    reading.timestampMs = millis();
    if (!enabled_) {
      reading.status = SensorStatus::Disabled;
      lastReading_ = reading;
      return reading;
    }
    reading.status = SensorStatus::Ok;
    reading.state = detected() ? DetectionState::Active : DetectionState::Inactive;
    reading.strengthPermille = reading.detected() ? 1000 : 0;
    reading.rawValue = reading.detected() ? 1 : 0;
    lastReading_ = reading;
    return reading;
  }

  SharpDigitalRange range() const {
    switch (model_) {
      case SharpDigitalModel::GP2Y0D805Z0F:
        return {5, 50};
      case SharpDigitalModel::GP2Y0D810Z0F:
        return {20, 100};
      case SharpDigitalModel::GP2Y0D815Z0F:
      default:
        return {5, 150};
    }
  }

  uint16_t minMillimeters() const {
    return range().minDistanceMm;
  }

  uint16_t maxMillimeters() const {
    return range().maxDistanceMm;
  }

  const ProximityReading& lastReading() const {
    return lastReading_;
  }

  SensorProbe probe() const {
    SensorProbe info;
    info.kind = SensorKind::Proximity;
    info.busType = SensorBusType::GpioDigital;
    info.configured = outputPin_ != kUnassignedPin;
    info.powered = enabled_;
    info.primaryPin = outputPin_;
    info.secondaryPin = enablePin_;
    info.supportsThresholdConfig = true;
    return info;
  }

  SelfTestReport selfTest() {
    ProximityReading reading = read();
    SelfTestReport report;
    report.passed = outputPin_ != kUnassignedPin;
    report.status = report.passed ? reading.status : SensorStatus::NotReady;
    report.value = reading.strengthPermille;
    report.detail = reading.detected() ? 1 : 0;
    return report;
  }

 private:
  static constexpr uint8_t kUnassignedPin = 0xFF;

  uint8_t outputPin_ = kUnassignedPin;
  uint8_t enablePin_ = kUnassignedPin;
  SharpDigitalModel model_ = SharpDigitalModel::GP2Y0D810Z0F;
  bool activeLow_ = true;
  bool enableActiveHigh_ = true;
  bool enabled_ = true;
  ProximityReading lastReading_;
};

}  // namespace DiFinders