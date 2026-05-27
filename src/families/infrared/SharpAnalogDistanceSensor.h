#pragma once

#include <Arduino.h>

#include "../../core/DiFindersConfig.h"
#include "../../core/DiFindersTypes.h"
#include "../../core/DiFindersDiagnostics.h"

namespace DiFinders {

enum class SharpAnalogModel : uint8_t {
  GP2Y0A21YK0F,
  GP2Y0A41SK0F,
  GP2Y0A51SK0F,
  GP2Y0A02YK0F,
  GP2Y0A60SZLF,
};

struct SharpAnalogModelData {
  uint16_t minDistanceMm;
  uint16_t maxDistanceMm;
  uint16_t minVoltageMv;
  uint16_t maxVoltageMv;
  uint32_t numerator;
  uint16_t denominatorOffsetMv;
};

class SharpAnalogDistanceSensor {
 public:
  SharpAnalogDistanceSensor() = default;

  SharpAnalogDistanceSensor(uint8_t analogPin, SharpAnalogModel model, uint16_t adcReferenceMv = 3300,
                            uint16_t adcMax = DI_FINDERS_DEFAULT_ADC_MAX) {
    begin(analogPin, model, adcReferenceMv, adcMax);
  }

  void begin(uint8_t analogPin, SharpAnalogModel model, uint16_t adcReferenceMv = 3300, uint16_t adcMax = DI_FINDERS_DEFAULT_ADC_MAX) {
    analogPin_ = analogPin;
    model_ = model;
    adcReferenceMv_ = adcReferenceMv;
    adcMax_ = adcMax == 0 ? 1 : adcMax;
    pinMode(analogPin_, INPUT);
  }

  bool ready() const {
    return analogPin_ != kUnassignedPin;
  }

  void configureAdc(uint16_t adcReferenceMv, uint16_t adcMax) {
    adcReferenceMv_ = adcReferenceMv;
    adcMax_ = adcMax == 0 ? 1 : adcMax;
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
    if (enablePin_ == kUnassignedPin) {
      enabled_ = true;
      return;
    }
    digitalWrite(enablePin_, enableActiveHigh_ ? HIGH : LOW);
    enabled_ = true;
  }

  void disable() {
    if (enablePin_ == kUnassignedPin) {
      enabled_ = false;
      return;
    }
    digitalWrite(enablePin_, enableActiveHigh_ ? LOW : HIGH);
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
    uint32_t raw = readRaw();
    lastVoltageMv_ = static_cast<uint16_t>((raw * adcReferenceMv_) / adcMax_);
    return lastVoltageMv_;
  }

  RangeReading readDistance() {
    RangeReading reading;
    reading.timestampMs = millis();
    if (!enabled_) {
      reading.status = SensorStatus::Disabled;
      return reading;
    }
    uint16_t voltageMv = readVoltageMv();
    const SharpAnalogModelData& data = modelData(model_);
    reading.rawValue = lastRaw_;

    if (voltageMv == 0 || voltageMv < data.minVoltageMv / 4) {
      reading.status = SensorStatus::Timeout;
      lastReading_ = reading;
      return reading;
    }

    uint16_t effectiveMv = voltageMv > data.denominatorOffsetMv ? voltageMv - data.denominatorOffsetMv : 1;
    uint32_t estimatedMm = data.numerator / effectiveMv;
    if (estimatedMm < data.minDistanceMm || estimatedMm > data.maxDistanceMm) {
      reading.status = SensorStatus::OutOfRange;
      reading.distanceMm = static_cast<uint16_t>(estimatedMm > 65535UL ? 65535UL : estimatedMm);
      lastReading_ = reading;
      return reading;
    }

    reading.status = SensorStatus::Ok;
    reading.distanceMm = applyCalibrationMm(static_cast<uint16_t>(estimatedMm), calibration_);
    lastReading_ = reading;
    return reading;
  }

  RangeReading read() {
    return readDistance();
  }

  RangeReading readDistanceAveraged(uint8_t samples) {
    if (samples == 0) {
      samples = 1;
    }

    uint32_t totalRaw = 0;
    for (uint8_t index = 0; index < samples; ++index) {
      totalRaw += analogRead(analogPin_);
    }
    lastRaw_ = static_cast<uint16_t>(totalRaw / samples);
    lastVoltageMv_ = static_cast<uint16_t>((static_cast<uint32_t>(lastRaw_) * adcReferenceMv_) / adcMax_);
    return distanceFromVoltage(lastVoltageMv_);
  }

  RangeReading readDistanceMedian3() {
    uint16_t a = analogRead(analogPin_);
    uint16_t b = analogRead(analogPin_);
    uint16_t c = analogRead(analogPin_);
    if (a > b) {
      uint16_t temp = a;
      a = b;
      b = temp;
    }
    if (b > c) {
      uint16_t temp = b;
      b = c;
      c = temp;
    }
    if (a > b) {
      uint16_t temp = a;
      a = b;
      b = temp;
    }
    lastRaw_ = b;
    lastVoltageMv_ = static_cast<uint16_t>((static_cast<uint32_t>(lastRaw_) * adcReferenceMv_) / adcMax_);
    return distanceFromVoltage(lastVoltageMv_);
  }

  uint16_t millimeters() {
    return readDistance().distanceMm;
  }

  uint16_t averageMillimeters(uint8_t samples = 3) {
    return readDistanceAveraged(samples).distanceMm;
  }

  uint16_t medianMillimeters() {
    return readDistanceMedian3().distanceMm;
  }

  uint16_t raw() {
    return readRaw();
  }

  uint16_t voltage() {
    return readVoltageMv();
  }

  bool detectWithin(uint16_t minDistanceMm, uint16_t maxDistanceMm) {
    RangeReading reading = readDistance();
    return reading.valid() && reading.distanceMm >= minDistanceMm && reading.distanceMm <= maxDistanceMm;
  }

  uint16_t lastRaw() const {
    return lastRaw_;
  }

  uint16_t lastVoltageMv() const {
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

  SelfTestReport selfTest(uint8_t samples = 3) {
    SelfTestReport report;
    if (!ready()) {
      report.status = SensorStatus::NotReady;
      return report;
    }
    RangeReading reading = readDistanceAveraged(samples);
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

  const SharpAnalogModelData& modelData(SharpAnalogModel model) const {
    static const SharpAnalogModelData models[] = {
        {100, 800, 350, 3000, 285000UL, 120},
        {40, 300, 350, 3100, 120000UL, 110},
        {20, 150, 350, 3000, 60000UL, 110},
        {200, 1500, 280, 2600, 480000UL, 100},
        {100, 1500, 250, 3300, 530000UL, 80},
    };
    return models[static_cast<uint8_t>(model)];
  }

  RangeReading distanceFromVoltage(uint16_t voltageMv) {
    RangeReading reading;
    reading.timestampMs = millis();
    reading.rawValue = lastRaw_;
    if (!enabled_) {
      reading.status = SensorStatus::Disabled;
      return reading;
    }
    const SharpAnalogModelData& data = modelData(model_);
    if (voltageMv == 0 || voltageMv < data.minVoltageMv / 4) {
      reading.status = SensorStatus::Timeout;
      lastReading_ = reading;
      return reading;
    }

    uint16_t effectiveMv = voltageMv > data.denominatorOffsetMv ? voltageMv - data.denominatorOffsetMv : 1;
    uint32_t estimatedMm = data.numerator / effectiveMv;
    if (estimatedMm < data.minDistanceMm || estimatedMm > data.maxDistanceMm) {
      reading.status = SensorStatus::OutOfRange;
      reading.distanceMm = static_cast<uint16_t>(estimatedMm > 65535UL ? 65535UL : estimatedMm);
      lastReading_ = reading;
      return reading;
    }

    reading.status = SensorStatus::Ok;
    reading.distanceMm = applyCalibrationMm(static_cast<uint16_t>(estimatedMm), calibration_);
    lastReading_ = reading;
    return reading;
  }

  uint8_t analogPin_ = A0;
  uint8_t enablePin_ = kUnassignedPin;
  SharpAnalogModel model_ = SharpAnalogModel::GP2Y0A21YK0F;
  uint16_t adcReferenceMv_ = 3300;
  uint16_t adcMax_ = DI_FINDERS_DEFAULT_ADC_MAX;
  CalibrationData calibration_;
  bool enableActiveHigh_ = true;
  bool enabled_ = true;
  uint16_t lastRaw_ = 0;
  uint16_t lastVoltageMv_ = 0;
  RangeReading lastReading_;
};

}  // namespace DiFinders
