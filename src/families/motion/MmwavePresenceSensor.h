#pragma once

#include <Arduino.h>

#include "../../core/DiFindersGpioInterrupt.h"
#include "../../core/DiFindersTypes.h"

namespace DiFinders {

class MmwavePresenceSensor {
 public:
  MmwavePresenceSensor() = default;

  explicit MmwavePresenceSensor(uint8_t outPin, bool activeHigh = true)
      : outPin_(outPin), activeHigh_(activeHigh) {}

  void begin() {
    pinMode(outPin_, INPUT);
    initialized_ = true;
    const bool state = rawState();
    sampledState_ = state;
    lastState_ = state;
    previousStableState_ = state;
    lastEdgeMs_ = millis();
    lastPresenceMs_ = state ? lastEdgeMs_ : 0;
    currentReading_ = buildReading(state, 0);
  }

  void setDebounceMs(uint16_t debounceMs) {
    debounceMs_ = debounceMs;
  }

  void setPresenceHoldMs(uint16_t holdMs) {
    holdMs_ = holdMs;
  }

  bool read() {
    if (!initialized_) {
      currentReading_ = MotionReading();
      currentReading_.status = SensorStatus::NotReady;
      return false;
    }

    const uint32_t nowMs = millis();
    const bool state = rawState();
    const bool stableElapsed = (nowMs - lastEdgeMs_) >= debounceMs_;

    if (state != sampledState_) {
      sampledState_ = state;
      lastEdgeMs_ = nowMs;
    }

    if (stableElapsed && sampledState_ != lastState_) {
      previousStableState_ = lastState_;
      lastState_ = sampledState_;
      if (lastState_) {
        lastPresenceMs_ = nowMs;
      }
    } else {
      previousStableState_ = lastState_;
    }

    bool present = lastState_;
    if (!present && holdMs_ > 0 && (nowMs - lastPresenceMs_) <= holdMs_) {
      present = true;
    }

    currentReading_ = buildReading(present, nowMs);
    currentReading_.status = SensorStatus::Ok;
    return present;
  }

  bool presence() const {
    return currentReading_.detected();
  }

  bool rose() const {
    return currentReading_.rose;
  }

  bool fell() const {
    return currentReading_.fell;
  }

  MotionReading motion() {
    const MotionReading reading = currentReading_;
    return reading;
  }

  const MotionReading& lastReading() const {
    return currentReading_;
  }

  bool attachInterruptPin(uint8_t pin, uint8_t mode = DF_IRQ_RISING) {
    irqPin_ = pin;
    s_irqTarget_ = this;
    attachGpioInterrupt(irqPin_, MmwavePresenceSensor::irqHandler, irqLine_, mode);
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

  SensorProbe probe() const {
    SensorProbe info;
    info.kind = SensorKind::Motion;
    info.busType = SensorBusType::GpioDigital;
    info.configured = initialized_;
    info.primaryPin = outPin_;
    info.supportsContinuousMode = true;
    info.supportsInterruptReady = true;
    return info;
  }

  SelfTestReport selfTest() const {
    SelfTestReport report;
    report.passed = initialized_;
    report.status = initialized_ ? currentReading_.status : SensorStatus::NotReady;
    report.value = currentReading_.detected() ? 1 : 0;
    report.detail = static_cast<uint8_t>((currentReading_.rose ? 1 : 0) | (currentReading_.fell ? 2 : 0));
    return report;
  }

 private:
  static void irqHandler() {
    if (s_irqTarget_ != nullptr) {
      gpioInterruptIsr(&s_irqTarget_->irqLine_);
    }
  }

  static MmwavePresenceSensor* s_irqTarget_;

  bool rawState() const {
    return digitalRead(outPin_) == (activeHigh_ ? HIGH : LOW);
  }

  MotionReading buildReading(bool detected, uint32_t nowMs) const {
    MotionReading reading;
    reading.state = detected ? DetectionState::Active : DetectionState::Inactive;
    reading.rose = detected && !previousStableState_;
    reading.fell = !detected && previousStableState_;
    reading.timestampMs = nowMs;
    reading.lastActiveMs = detected ? nowMs : lastPresenceMs_;
    reading.status = SensorStatus::Ok;
    return reading;
  }

  uint8_t outPin_ = 0;
  bool activeHigh_ = true;
  bool initialized_ = false;
  bool sampledState_ = false;
  bool lastState_ = false;
  uint16_t debounceMs_ = 80;
  uint16_t holdMs_ = 500;
  uint32_t lastEdgeMs_ = 0;
  uint32_t lastPresenceMs_ = 0;
  bool previousStableState_ = false;
  MotionReading currentReading_;
  GpioInterruptLine irqLine_;
  uint8_t irqPin_ = 0xFF;
};

MmwavePresenceSensor* MmwavePresenceSensor::s_irqTarget_ = nullptr;

}  // namespace DiFinders
