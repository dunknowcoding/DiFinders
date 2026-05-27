#pragma once

#include <Arduino.h>

#include "../../core/DiFindersConfig.h"
#include "../../core/DiFindersTypes.h"

namespace DiFinders {

enum class ReflectiveIrProfile : uint8_t {
  GenericObstacleLm393,
  Tcrt5000,
  WwzmdibIr,
  GenericReflective,
};

class ReflectiveIrSensor {
 public:
  ReflectiveIrSensor() = default;

  ReflectiveIrSensor(uint8_t digitalPin, uint8_t analogPin = kUnassignedPin, uint8_t emitterPin = kUnassignedPin) {
    begin(digitalPin, analogPin, emitterPin);
  }

  void begin(uint8_t digitalPin, uint8_t analogPin = kUnassignedPin, uint8_t emitterPin = kUnassignedPin) {
    digitalPin_ = digitalPin;
    analogPin_ = analogPin;
    emitterPin_ = emitterPin;
    if (digitalPin_ != kUnassignedPin) {
      pinMode(digitalPin_, INPUT);
    }
    if (analogPin_ != kUnassignedPin) {
      pinMode(analogPin_, INPUT);
    }
    if (emitterPin_ != kUnassignedPin) {
      pinMode(emitterPin_, OUTPUT);
      enableEmitter(true);
    }
  }

  bool ready() const {
    return digitalPin_ != kUnassignedPin || analogPin_ != kUnassignedPin;
  }

  void setProfile(ReflectiveIrProfile profile) {
    profile_ = profile;
    if (profile_ == ReflectiveIrProfile::GenericObstacleLm393 || profile_ == ReflectiveIrProfile::WwzmdibIr) {
      digitalActiveLow_ = true;
    }
  }

  void setDigitalActiveLow(bool activeLow) {
    digitalActiveLow_ = activeLow;
  }

  void setCalibrationWindow(uint16_t rawLow, uint16_t rawHigh) {
    calibrationLow_ = rawLow;
    calibrationHigh_ = rawHigh > rawLow ? rawHigh : rawLow + 1;
  }

  void enableEmitter(bool enabled) {
    if (emitterPin_ == kUnassignedPin) {
      emitterEnabled_ = enabled;
      return;
    }
    digitalWrite(emitterPin_, enabled ? HIGH : LOW);
    emitterEnabled_ = enabled;
  }

  bool emitterEnabled() const {
    return emitterEnabled_;
  }

  bool readDigital() {
    if (digitalPin_ == kUnassignedPin) {
      return false;
    }
    bool raw = digitalRead(digitalPin_) == HIGH;
    lastDigitalDetected_ = digitalActiveLow_ ? !raw : raw;
    return lastDigitalDetected_;
  }

  uint16_t readAnalogRaw() {
    if (analogPin_ == kUnassignedPin) {
      lastAnalogRaw_ = 0;
      return 0;
    }
    lastAnalogRaw_ = analogRead(analogPin_);
    return lastAnalogRaw_;
  }

  uint16_t readAmbientCompensatedRaw() {
    if (analogPin_ == kUnassignedPin) {
      lastAmbientCompensatedRaw_ = 0;
      return 0;
    }
    if (emitterPin_ == kUnassignedPin) {
      lastAmbientCompensatedRaw_ = readAnalogRaw();
      return lastAmbientCompensatedRaw_;
    }

    enableEmitter(false);
    delayMicroseconds(50);
    uint16_t ambient = analogRead(analogPin_);
    enableEmitter(true);
    delayMicroseconds(50);
    uint16_t reflected = analogRead(analogPin_);
    lastAnalogRaw_ = reflected;
    lastAmbientCompensatedRaw_ = reflected > ambient ? reflected - ambient : 0;
    return lastAmbientCompensatedRaw_;
  }

  ProximityReading read() {
    ProximityReading reading;
    reading.timestampMs = millis();
    reading.rawValue = analogPin_ == kUnassignedPin ? 0 : readAmbientCompensatedRaw();
    bool digitalDetected = readDigital();

    if (analogPin_ != kUnassignedPin) {
      uint16_t basis = lastAmbientCompensatedRaw_;
      if (basis < calibrationLow_) {
        basis = calibrationLow_;
      }
      int32_t normalized = ((static_cast<int32_t>(basis) - calibrationLow_) * 1000) /
                           (static_cast<int32_t>(calibrationHigh_) - calibrationLow_);
      reading.strengthPermille = clampToPermille(normalized);
      if (digitalPin_ == kUnassignedPin) {
        reading.state = reading.strengthPermille >= analogThresholdPermille_ ? DetectionState::Active : DetectionState::Inactive;
      } else {
        reading.state = digitalDetected ? DetectionState::Active : DetectionState::Inactive;
      }
    } else {
      reading.state = digitalDetected ? DetectionState::Active : DetectionState::Inactive;
      reading.strengthPermille = reading.state == DetectionState::Active ? 1000 : 0;
    }

    reading.status = SensorStatus::Ok;
    lastReading_ = reading;
    return reading;
  }

  bool active() {
    return read().detected();
  }

  uint16_t strength() {
    return read().strengthPermille;
  }

  uint16_t raw() {
    return readAnalogRaw();
  }

  uint16_t compensated() {
    return readAmbientCompensatedRaw();
  }

  void setAnalogThresholdPermille(uint16_t thresholdPermille) {
    analogThresholdPermille_ = clampToPermille(thresholdPermille);
  }

  SensorProbe probe() const {
    SensorProbe info;
    info.kind = SensorKind::Reflective;
    info.busType = analogPin_ != kUnassignedPin ? SensorBusType::Analog : SensorBusType::GpioDigital;
    info.configured = ready();
    info.powered = emitterEnabled_;
    info.supportsThresholdConfig = analogPin_ != kUnassignedPin;
    info.primaryPin = digitalPin_;
    info.secondaryPin = analogPin_;
    return info;
  }

  SelfTestReport selfTest() {
    SelfTestReport report;
    ProximityReading reading = read();
    report.passed = ready();
    report.status = ready() ? reading.status : SensorStatus::NotReady;
    report.value = reading.strengthPermille;
    report.detail = reading.detected() ? 1 : 0;
    return report;
  }

  const ProximityReading& lastReading() const {
    return lastReading_;
  }

 private:
  static constexpr uint8_t kUnassignedPin = 0xFF;

  uint8_t digitalPin_ = kUnassignedPin;
  uint8_t analogPin_ = kUnassignedPin;
  uint8_t emitterPin_ = kUnassignedPin;
  ReflectiveIrProfile profile_ = ReflectiveIrProfile::GenericReflective;
  uint16_t calibrationLow_ = 0;
  uint16_t calibrationHigh_ = DI_FINDERS_DEFAULT_ADC_MAX;
  uint16_t analogThresholdPermille_ = 300;
  bool digitalActiveLow_ = true;
  bool emitterEnabled_ = true;
  bool lastDigitalDetected_ = false;
  uint16_t lastAnalogRaw_ = 0;
  uint16_t lastAmbientCompensatedRaw_ = 0;
  ProximityReading lastReading_;
};

}  // namespace DiFinders
