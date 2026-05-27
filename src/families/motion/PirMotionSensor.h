#pragma once

#include <Arduino.h>

#include "../../core/DiFindersGpioInterrupt.h"
#include "../../core/DiFindersTypes.h"

namespace DiFinders {

enum class PirModuleProfile : uint8_t {
  HC_SR501,
  HC_SR505,
  AM312,
  Generic,
};

class PirMotionSensor {
 public:
  PirMotionSensor() = default;

  PirMotionSensor(uint8_t pin, PirModuleProfile profile = PirModuleProfile::Generic, bool activeHigh = true,
                  uint8_t inputMode = INPUT) {
    begin(pin, profile, activeHigh, inputMode);
  }

  void begin(uint8_t pin, PirModuleProfile profile = PirModuleProfile::Generic, bool activeHigh = true, uint8_t inputMode = INPUT) {
    pin_ = pin;
    profile_ = profile;
    activeHigh_ = activeHigh;
    pinMode(pin_, inputMode);
    applyProfileDefaults();
    bool initial = readRawState();
    stableState_ = initial;
    lastStableState_ = initial;
    lastSampleState_ = initial;
    lastSampleChangeMs_ = millis();
    lastChangeMs_ = lastSampleChangeMs_;
    lastActiveMs_ = initial ? lastSampleChangeMs_ : 0;
  }

  void configureTiming(uint16_t debounceMs, uint32_t latchHoldMs) {
    debounceMs_ = debounceMs;
    latchHoldMs_ = latchHoldMs;
  }

  MotionReading update(uint32_t nowMs = millis()) {
    MotionReading reading;
    bool raw = readRawState();

    if (raw != lastSampleState_) {
      lastSampleState_ = raw;
      lastSampleChangeMs_ = nowMs;
    }

    if ((nowMs - lastSampleChangeMs_) >= debounceMs_ && stableState_ != raw) {
      lastStableState_ = stableState_;
      stableState_ = raw;
      lastChangeMs_ = nowMs;
      if (stableState_) {
        lastActiveMs_ = nowMs;
      }
      rose_ = stableState_ && !lastStableState_;
      fell_ = !stableState_ && lastStableState_;
    } else {
      rose_ = false;
      fell_ = false;
    }

    bool latched = stableState_;
    if (!latched && latchHoldMs_ > 0 && (nowMs - lastActiveMs_) <= latchHoldMs_) {
      latched = true;
    }

    reading.status = SensorStatus::Ok;
    reading.state = latched ? DetectionState::Active : DetectionState::Inactive;
    reading.rose = rose_;
    reading.fell = fell_;
    reading.timestampMs = nowMs;
    reading.lastActiveMs = lastActiveMs_;
    lastReading_ = reading;
    return reading;
  }

  MotionReading read(uint32_t nowMs = millis()) {
    return update(nowMs);
  }

  bool readRawState() const {
    bool raw = digitalRead(pin_) == HIGH;
    return activeHigh_ ? raw : !raw;
  }

  bool isActive(uint32_t nowMs = millis()) {
    return update(nowMs).detected();
  }

  bool motion(uint32_t nowMs = millis()) {
    return isActive(nowMs);
  }

  bool rose() const {
    return rose_;
  }

  bool fell() const {
    return fell_;
  }

  uint32_t lastActiveAt() const {
    return lastActiveMs_;
  }

  SensorProbe probe() const {
    SensorProbe info;
    info.kind = SensorKind::Motion;
    info.busType = SensorBusType::GpioDigital;
    info.configured = pin_ != 0xFF;
    info.primaryPin = pin_;
    info.supportsInterruptReady = true;
    return info;
  }

  SelfTestReport selfTest(uint32_t nowMs = millis()) {
    SelfTestReport report;
    MotionReading reading = update(nowMs);
    report.passed = pin_ != 0xFF;
    report.status = report.passed ? reading.status : SensorStatus::NotReady;
    report.value = reading.detected() ? 1 : 0;
    report.detail = static_cast<uint8_t>((reading.rose ? 1 : 0) | (reading.fell ? 2 : 0));
    return report;
  }

  const MotionReading& lastReading() const {
    return lastReading_;
  }

  /** GPIO interrupt on the PIR OUT pin (AVR-safe, one active instance per class). */
  bool attachInterruptPin(uint8_t pin, uint8_t mode = DF_IRQ_RISING) {
    irqPin_ = pin;
    s_irqTarget_ = this;
    attachGpioInterrupt(irqPin_, PirMotionSensor::irqHandler, irqLine_, mode);
    return true;
  }

  void detachInterruptPin() {
    if (irqPin_ != 0xFF) {
      detachGpioInterrupt(irqPin_);
    }
    irqLine_.disarm();
    if (s_irqTarget_ == this) {
      s_irqTarget_ = nullptr;
    }
  }

  bool takeInterruptEvent() {
    return irqLine_.takeEvent();
  }

  bool interruptPending() const {
    return irqLine_.pending();
  }

 private:
  static void irqHandler() {
    if (s_irqTarget_ != nullptr) {
      gpioInterruptIsr(&s_irqTarget_->irqLine_);
    }
  }

  static PirMotionSensor* s_irqTarget_;
  void applyProfileDefaults() {
    switch (profile_) {
      case PirModuleProfile::AM312:
        debounceMs_ = 15;
        latchHoldMs_ = 2500;
        break;
      case PirModuleProfile::HC_SR505:
        debounceMs_ = 20;
        latchHoldMs_ = 8000;
        break;
      case PirModuleProfile::HC_SR501:
        debounceMs_ = 30;
        latchHoldMs_ = 10000;
        break;
      case PirModuleProfile::Generic:
      default:
        debounceMs_ = 20;
        latchHoldMs_ = 5000;
        break;
    }
  }

  uint8_t pin_ = 0xFF;
  PirModuleProfile profile_ = PirModuleProfile::Generic;
  bool activeHigh_ = true;
  bool stableState_ = false;
  bool lastStableState_ = false;
  bool lastSampleState_ = false;
  bool rose_ = false;
  bool fell_ = false;
  uint16_t debounceMs_ = 20;
  uint32_t latchHoldMs_ = 5000;
  uint32_t lastSampleChangeMs_ = 0;
  uint32_t lastChangeMs_ = 0;
  uint32_t lastActiveMs_ = 0;
  MotionReading lastReading_;
  GpioInterruptLine irqLine_;
  uint8_t irqPin_ = 0xFF;
};

PirMotionSensor* PirMotionSensor::s_irqTarget_ = nullptr;

}  // namespace DiFinders
