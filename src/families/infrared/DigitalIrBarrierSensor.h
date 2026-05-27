#pragma once

#include <Arduino.h>

#include "../../core/DiFindersDiagnostics.h"
#include "../../core/DiFindersGpioInterrupt.h"
#include "../../core/DiFindersTypes.h"

namespace DiFinders {

enum class DigitalIrBarrierProfile : uint8_t {
  E18_D80NK,
  E18_D60NK,
  GenericActiveLow,
  GenericActiveHigh,
};

class DigitalIrBarrierSensor {
 public:
  DigitalIrBarrierSensor() = default;

  DigitalIrBarrierSensor(uint8_t outputPin, DigitalIrBarrierProfile profile = DigitalIrBarrierProfile::E18_D80NK)
      : outputPin_(outputPin), profile_(profile) {
    applyProfileDefaults();
  }

  void begin(uint8_t outputPin, DigitalIrBarrierProfile profile = DigitalIrBarrierProfile::E18_D80NK) {
    outputPin_ = outputPin;
    profile_ = profile;
    applyProfileDefaults();
    pinMode(outputPin_, INPUT);
    initialized_ = true;
    updateReading(0);
  }

  void setActiveLow(bool activeLow) {
    activeLow_ = activeLow;
  }

  void setDebounceMs(uint16_t debounceMs) {
    debounceMs_ = debounceMs;
  }

  bool ready() const {
    return initialized_;
  }

  bool read() {
    if (!initialized_) {
      currentReading_.status = SensorStatus::NotReady;
      return false;
    }

    const uint32_t nowMs = millis();
    const bool raw = digitalRead(outputPin_) == (activeLow_ ? LOW : HIGH);
    if (raw != sampledState_) {
      sampledState_ = raw;
      lastEdgeMs_ = nowMs;
    }

    if ((nowMs - lastEdgeMs_) >= debounceMs_ && raw != stableState_) {
      stableState_ = raw;
    }

    return updateReading(nowMs);
  }

  bool detected() const {
    return currentReading_.detected();
  }

  ProximityReading proximity() const {
    return currentReading_;
  }

  const ProximityReading& lastReading() const {
    return currentReading_;
  }

  bool attachInterruptPin(uint8_t pin, uint8_t mode = DF_IRQ_CHANGE) {
    irqPin_ = pin;
    s_irqTarget_ = this;
    attachGpioInterrupt(irqPin_, DigitalIrBarrierSensor::irqHandler, irqLine_, mode);
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
    info.kind = SensorKind::Proximity;
    info.busType = SensorBusType::GpioDigital;
    info.configured = initialized_;
    info.primaryPin = outputPin_;
    info.supportsInterruptReady = true;
    return info;
  }

  SelfTestReport selfTest() {
    SelfTestReport report;
    report.passed = initialized_;
    report.status = initialized_ ? SensorStatus::Ok : SensorStatus::NotReady;
    report.value = detected() ? 1 : 0;
    return report;
  }

  uint8_t lastBusError() const { return 0; }

  SensorHealth health() const {
    return makeHealth(initialized_, probe().busType, currentReading_.status, 0, currentReading_.rawValue, 0);
  }

 private:
  static void irqHandler() {
    if (s_irqTarget_ != nullptr) {
      gpioInterruptIsr(&s_irqTarget_->irqLine_);
    }
  }

  static DigitalIrBarrierSensor* s_irqTarget_;

  void applyProfileDefaults() {
    switch (profile_) {
      case DigitalIrBarrierProfile::E18_D80NK:
      case DigitalIrBarrierProfile::E18_D60NK:
      case DigitalIrBarrierProfile::GenericActiveLow:
        activeLow_ = true;
        break;
      case DigitalIrBarrierProfile::GenericActiveHigh:
        activeLow_ = false;
        break;
    }
  }

  bool updateReading(uint32_t nowMs) {
    currentReading_.timestampMs = nowMs;
    currentReading_.status = SensorStatus::Ok;
    currentReading_.state = stableState_ ? DetectionState::Active : DetectionState::Inactive;
    currentReading_.strengthPermille = stableState_ ? 1000 : 0;
    currentReading_.rawValue = stableState_ ? 1 : 0;
    return stableState_;
  }

  uint8_t outputPin_ = 0xFF;
  DigitalIrBarrierProfile profile_ = DigitalIrBarrierProfile::E18_D80NK;
  bool activeLow_ = true;
  bool initialized_ = false;
  bool sampledState_ = false;
  bool stableState_ = false;
  uint16_t debounceMs_ = 5;
  uint32_t lastEdgeMs_ = 0;
  ProximityReading currentReading_;
  GpioInterruptLine irqLine_;
  uint8_t irqPin_ = 0xFF;
};

DigitalIrBarrierSensor* DigitalIrBarrierSensor::s_irqTarget_ = nullptr;

}  // namespace DiFinders
