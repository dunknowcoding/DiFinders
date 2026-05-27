#pragma once

#include <Arduino.h>

#include "../../core/DiFindersConfig.h"
#include "../../core/DiFindersGpioInterrupt.h"
#include "../../core/DiFindersTypes.h"

namespace DiFinders {

class PulseUltrasonicSensor {
 public:
  PulseUltrasonicSensor() = default;

  PulseUltrasonicSensor(uint8_t triggerPin, uint8_t echoPin, uint8_t echoPinMode = INPUT) {
    begin(triggerPin, echoPin, echoPinMode);
  }

  void begin(uint8_t triggerPin, uint8_t echoPin, uint8_t echoPinMode = INPUT) {
    triggerPin_ = triggerPin;
    echoPin_ = echoPin;
    pinMode(triggerPin_, OUTPUT);
    digitalWrite(triggerPin_, LOW);
    pinMode(echoPin_, echoPinMode);
  }

  bool ready() const {
    return triggerPin_ != kUnassignedPin && echoPin_ != kUnassignedPin;
  }

  void configureTiming(uint32_t timeoutUs, uint16_t triggerPulseUs = 10, uint16_t settleUs = 2) {
    timeoutUs_ = timeoutUs;
    triggerPulseUs_ = triggerPulseUs;
    settleUs_ = settleUs;
  }

  void configureEnvironment(uint32_t soundSpeedMmPerSecond) {
    soundSpeedMmPerSecond_ = soundSpeedMmPerSecond;
  }

  void setMinDistanceMm(uint16_t minDistanceMm) {
    minDistanceMm_ = minDistanceMm;
  }

  RangeReading singleShot() {
    RangeReading reading;
    reading.timestampMs = millis();

    if (triggerPin_ == kUnassignedPin || echoPin_ == kUnassignedPin) {
      reading.status = SensorStatus::NotReady;
      return reading;
    }

    uint32_t pulseUs = measurePulseUs();
    lastPulseUs_ = pulseUs;

    if (pulseUs == 0) {
      reading.status = SensorStatus::Timeout;
      lastReading_ = reading;
      return reading;
    }

    uint32_t distanceMm = (pulseUs * soundSpeedMmPerSecond_) / 2000000UL;
    if (distanceMm < minDistanceMm_) {
      reading.status = SensorStatus::OutOfRange;
      reading.distanceMm = static_cast<uint16_t>(distanceMm);
      reading.rawValue = static_cast<uint16_t>(pulseUs > 65535UL ? 65535UL : pulseUs);
      lastReading_ = reading;
      return reading;
    }

    reading.status = SensorStatus::Ok;
    reading.distanceMm = static_cast<uint16_t>(distanceMm > 65535UL ? 65535UL : distanceMm);
    reading.rawValue = static_cast<uint16_t>(pulseUs > 65535UL ? 65535UL : pulseUs);
    lastReading_ = reading;
    return reading;
  }

  RangeReading read() {
    return singleShot();
  }

  RangeReading singleShotMedian3() {
    RangeReading a = singleShot();
    RangeReading b = singleShot();
    RangeReading c = singleShot();

    if (!a.valid() || !b.valid() || !c.valid()) {
      if (a.valid()) {
        return a;
      }
      if (b.valid()) {
        return b;
      }
      return c;
    }

    uint16_t v0 = a.distanceMm;
    uint16_t v1 = b.distanceMm;
    uint16_t v2 = c.distanceMm;
    if (v0 > v1) {
      uint16_t temp = v0;
      v0 = v1;
      v1 = temp;
    }
    if (v1 > v2) {
      uint16_t temp = v1;
      v1 = v2;
      v2 = temp;
    }
    if (v0 > v1) {
      uint16_t temp = v0;
      v0 = v1;
      v1 = temp;
    }

    b.distanceMm = v1;
    return b;
  }

  uint16_t millimeters() {
    return singleShot().distanceMm;
  }

  uint16_t medianMillimeters() {
    return singleShotMedian3().distanceMm;
  }

  uint16_t centimeters() {
    return static_cast<uint16_t>(millimeters() / 10U);
  }

  uint16_t medianCentimeters() {
    return static_cast<uint16_t>(medianMillimeters() / 10U);
  }

  uint32_t lastPulseUs() const {
    return lastPulseUs_;
  }

  SensorProbe probe() const {
    SensorProbe info;
    info.kind = SensorKind::Range;
    info.busType = SensorBusType::GpioPulse;
    info.configured = ready();
    info.primaryPin = triggerPin_;
    info.secondaryPin = echoPin_;
    info.supportsInterruptReady = true;
    return info;
  }

  SelfTestReport selfTest() {
    SelfTestReport report;
    report.passed = ready();
    report.status = ready() ? SensorStatus::Ok : SensorStatus::NotReady;
    report.value = static_cast<uint16_t>(lastPulseUs_ > 65535UL ? 65535UL : lastPulseUs_);
    return report;
  }

  const RangeReading& lastReading() const {
    return lastReading_;
  }

  /** Non-blocking echo timing: attach echo pin, call startPing(), wait for takeEchoEvent(), then completePing(). */
  bool attachEchoInterrupt(uint8_t mode = DF_IRQ_CHANGE) {
    s_irqTarget_ = this;
    attachGpioInterrupt(echoPin_, PulseUltrasonicSensor::echoIsr, irqLine_, mode);
    return true;
  }

  void detachEchoInterrupt() {
    detachGpioInterrupt(echoPin_);
    irqLine_.disarm();
    if (s_irqTarget_ == this) {
      s_irqTarget_ = nullptr;
    }
  }

  void startPing() {
    echoPending_ = false;
    echoStartUs_ = 0;
    echoEndUs_ = 0;
    digitalWrite(triggerPin_, LOW);
    delayMicroseconds(settleUs_);
    digitalWrite(triggerPin_, HIGH);
    delayMicroseconds(triggerPulseUs_);
    digitalWrite(triggerPin_, LOW);
  }

  bool takeEchoEvent() {
    return irqLine_.takeEvent();
  }

  RangeReading completePing() {
    RangeReading reading;
    reading.timestampMs = millis();
    if (!ready()) {
      reading.status = SensorStatus::NotReady;
      lastReading_ = reading;
      return reading;
    }
    if (echoEndUs_ <= echoStartUs_) {
      reading.status = SensorStatus::Timeout;
      lastReading_ = reading;
      return reading;
    }
    lastPulseUs_ = echoEndUs_ - echoStartUs_;
    reading = readingFromPulse(lastPulseUs_);
    lastReading_ = reading;
    return reading;
  }

#if DI_FINDERS_ENABLE_TEST_API
  void setSimulatedPulseUs(uint32_t pulseUs) {
    simulatedPulseUs_ = pulseUs;
    useSimulatedPulse_ = true;
  }

  void clearSimulatedPulse() {
    useSimulatedPulse_ = false;
  }
#endif

 private:
  static void echoIsr() {
    if (s_irqTarget_ == nullptr) {
      return;
    }
    PulseUltrasonicSensor* self = s_irqTarget_;
    if (digitalRead(self->echoPin_) == HIGH) {
      self->echoStartUs_ = micros();
    } else {
      self->echoEndUs_ = micros();
      gpioInterruptIsr(&self->irqLine_);
    }
  }

  static PulseUltrasonicSensor* s_irqTarget_;

  RangeReading readingFromPulse(uint32_t pulseUs) {
    RangeReading reading;
    reading.timestampMs = millis();
    if (pulseUs == 0) {
      reading.status = SensorStatus::Timeout;
      return reading;
    }
    uint32_t distanceMm32 = (pulseUs * soundSpeedMmPerSecond_) / 2000000UL;
    if (distanceMm32 < minDistanceMm_) {
      reading.status = SensorStatus::OutOfRange;
      reading.distanceMm = 0;
      reading.rawValue = static_cast<uint16_t>(pulseUs > 65535UL ? 65535UL : pulseUs);
      return reading;
    }
    reading.distanceMm = static_cast<uint16_t>(distanceMm32 > 65535UL ? 65535UL : distanceMm32);
    reading.rawValue = static_cast<uint16_t>(pulseUs > 65535UL ? 65535UL : pulseUs);
    reading.status = SensorStatus::Ok;
    return reading;
  }

  static constexpr uint8_t kUnassignedPin = 0xFF;

  uint32_t measurePulseUs() {
#if DI_FINDERS_ENABLE_TEST_API
    if (useSimulatedPulse_) {
      return simulatedPulseUs_;
    }
#endif
    digitalWrite(triggerPin_, LOW);
    delayMicroseconds(settleUs_);
    digitalWrite(triggerPin_, HIGH);
    delayMicroseconds(triggerPulseUs_);
    digitalWrite(triggerPin_, LOW);
    return pulseIn(echoPin_, HIGH, timeoutUs_);
  }

  uint8_t triggerPin_ = kUnassignedPin;
  uint8_t echoPin_ = kUnassignedPin;
  uint32_t timeoutUs_ = 30000UL;
  uint32_t soundSpeedMmPerSecond_ = 343200UL;
  uint16_t triggerPulseUs_ = 10;
  uint16_t settleUs_ = 2;
  uint16_t minDistanceMm_ = 20;
  uint32_t lastPulseUs_ = 0;
  volatile uint32_t echoStartUs_ = 0;
  volatile uint32_t echoEndUs_ = 0;
  volatile bool echoPending_ = false;
  RangeReading lastReading_;
  GpioInterruptLine irqLine_;
#if DI_FINDERS_ENABLE_TEST_API
  bool useSimulatedPulse_ = false;
  uint32_t simulatedPulseUs_ = 0;
#endif
};

PulseUltrasonicSensor* PulseUltrasonicSensor::s_irqTarget_ = nullptr;

}  // namespace DiFinders
