#pragma once

#include <Arduino.h>

#include "../../core/DiFindersConfig.h"
#include "../../core/DiFindersTypes.h"
#include "../../core/DiFindersDiagnostics.h"

namespace DiFinders {

class MaxBotixAnalogSensor {
 public:
  MaxBotixAnalogSensor() = default;

  MaxBotixAnalogSensor(uint8_t analogPin, uint16_t adcReferenceMv = 3300,
                       uint16_t adcMax = DI_FINDERS_DEFAULT_ADC_MAX, uint16_t sensorSupplyMv = 0) {
    begin(analogPin, adcReferenceMv, adcMax, sensorSupplyMv);
  }

  void begin(uint8_t analogPin, uint16_t adcReferenceMv = 3300,
             uint16_t adcMax = DI_FINDERS_DEFAULT_ADC_MAX, uint16_t sensorSupplyMv = 0) {
    analogPin_ = analogPin;
    adcReferenceMv_ = adcReferenceMv == 0 ? 1 : adcReferenceMv;
    adcMax_ = adcMax == 0 ? 1 : adcMax;
    sensorSupplyMv_ = sensorSupplyMv == 0 ? adcReferenceMv_ : sensorSupplyMv;
    pinMode(analogPin_, INPUT);
  }

  bool ready() const {
    return analogPin_ != kUnassignedPin;
  }

  void configureElectrical(uint16_t adcReferenceMv, uint16_t adcMax, uint16_t sensorSupplyMv = 0) {
    adcReferenceMv_ = adcReferenceMv == 0 ? 1 : adcReferenceMv;
    adcMax_ = adcMax == 0 ? 1 : adcMax;
    sensorSupplyMv_ = sensorSupplyMv == 0 ? adcReferenceMv_ : sensorSupplyMv;
  }

  void configureRange(uint16_t minDistanceMm, uint16_t maxDistanceMm) {
    minDistanceMm_ = minDistanceMm;
    maxDistanceMm_ = maxDistanceMm;
  }

  void attachEnablePin(uint8_t enablePin, bool activeHigh = true) {
    enablePin_ = enablePin;
    enableActiveHigh_ = activeHigh;
    pinMode(enablePin_, OUTPUT);
    enable();
  }

  void setCalibration(const CalibrationData& calibration) {
    calibration_ = calibration;
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

  uint16_t readRaw() {
    lastRaw_ = analogRead(analogPin_);
    return lastRaw_;
  }

  uint16_t readVoltageMv() {
    lastVoltageMv_ = static_cast<uint16_t>((static_cast<uint32_t>(readRaw()) * adcReferenceMv_) / adcMax_);
    return lastVoltageMv_;
  }

  RangeReading read() {
    return readingFromVoltage(readVoltageMv(), millis());
  }

  uint16_t millimeters() {
    return read().distanceMm;
  }

  uint16_t averageMillimeters(uint8_t samples = 3) {
    if (samples == 0) {
      samples = 1;
    }
    uint32_t totalRaw = 0;
    for (uint8_t index = 0; index < samples; ++index) {
      totalRaw += analogRead(analogPin_);
    }
    lastRaw_ = static_cast<uint16_t>(totalRaw / samples);
    lastVoltageMv_ = static_cast<uint16_t>((static_cast<uint32_t>(lastRaw_) * adcReferenceMv_) / adcMax_);
    return readingFromVoltage(lastVoltageMv_, millis()).distanceMm;
  }

  uint16_t raw() const {
    return lastRaw_;
  }

  uint16_t voltage() const {
    return lastVoltageMv_;
  }

  SensorProbe probe() const {
    SensorProbe info;
    info.kind = SensorKind::Range;
    info.busType = SensorBusType::Analog;
    info.configured = ready();
    info.powered = enabled_;
    info.primaryPin = analogPin_;
    info.secondaryPin = enablePin_;
    return info;
  }

  SelfTestReport selfTest() {
    SelfTestReport report;
    if (!ready()) {
      report.status = SensorStatus::NotReady;
      return report;
    }
    RangeReading reading = read();
    report.passed = reading.status == SensorStatus::Ok || reading.status == SensorStatus::OutOfRange;
    report.status = reading.status;
    report.value = reading.distanceMm;
    report.detail = static_cast<uint8_t>(enabled_ ? 1 : 0);
    return report;
  }

  const RangeReading& lastReading() const {
    return lastReading_;
  }

  uint8_t lastBusError() const { return 0; }

  SensorHealth health() const {
    return makeHealth(ready(), probe().busType, lastReading_.status, 0, lastReading_.rawValue, 0);
  }

 private:
  static constexpr uint8_t kUnassignedPin = 0xFF;

  RangeReading readingFromVoltage(uint16_t voltageMv, uint32_t timestampMs) {
    RangeReading reading;
    reading.timestampMs = timestampMs;
    if (!ready()) {
      reading.status = SensorStatus::NotReady;
      lastReading_ = reading;
      return reading;
    }
    if (!enabled_) {
      reading.status = SensorStatus::Disabled;
      lastReading_ = reading;
      return reading;
    }

    reading.rawValue = lastRaw_;
    if (voltageMv == 0 || sensorSupplyMv_ == 0) {
      reading.status = SensorStatus::Timeout;
      lastReading_ = reading;
      return reading;
    }

    uint64_t scaled = static_cast<uint64_t>(voltageMv) * 26010ULL;
    uint32_t estimatedMm = static_cast<uint32_t>(scaled / sensorSupplyMv_);
    reading.distanceMm = applyCalibrationMm(static_cast<uint16_t>(estimatedMm > 65535UL ? 65535UL : estimatedMm), calibration_);
    if (reading.distanceMm < minDistanceMm_ || reading.distanceMm > maxDistanceMm_) {
      reading.status = SensorStatus::OutOfRange;
      lastReading_ = reading;
      return reading;
    }

    reading.status = SensorStatus::Ok;
    lastReading_ = reading;
    return reading;
  }

  uint8_t analogPin_ = kUnassignedPin;
  uint8_t enablePin_ = kUnassignedPin;
  uint16_t adcReferenceMv_ = 3300;
  uint16_t adcMax_ = DI_FINDERS_DEFAULT_ADC_MAX;
  uint16_t sensorSupplyMv_ = 3300;
  uint16_t minDistanceMm_ = 0;
  uint16_t maxDistanceMm_ = 10000;
  uint16_t lastRaw_ = 0;
  uint16_t lastVoltageMv_ = 0;
  bool enabled_ = true;
  bool enableActiveHigh_ = true;
  CalibrationData calibration_;
  RangeReading lastReading_;
};

}  // namespace DiFinders