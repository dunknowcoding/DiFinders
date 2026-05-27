#pragma once

#include <Arduino.h>

namespace DiFinders {

enum class SensorKind : uint8_t {
  Range,
  Proximity,
  Reflective,
  Motion,
};

enum class SensorStatus : uint8_t {
  Ok,
  Timeout,
  OutOfRange,
  NotReady,
  Disabled,
  Error,
};

enum class DetectionState : uint8_t {
  Inactive,
  Active,
};

enum class SensorBusType : uint8_t {
  Unknown,
  GpioPulse,
  GpioDigital,
  Analog,
  Uart,
  I2c,
  Rs485,
  Spi,
  Can,
};

struct SensorProbe {
  SensorKind kind = SensorKind::Range;
  SensorBusType busType = SensorBusType::Unknown;
  bool configured = false;
  bool powered = true;
  bool supportsAddressChange = false;
  bool supportsContinuousMode = false;
  bool supportsInterruptReady = false;
  bool supportsTemperatureReading = false;
  bool supportsThresholdConfig = false;
  /** Rectangular SPAD ROI (beam window), not independent multizone matrix cells. */
  bool supportsBeamRoi = false;
  /** MCU can use DiFinders DMA/burst path for this bus (SPI/UART on supported cores). */
  bool supportsDmaTransfer = false;
  uint8_t primaryPin = 0xFF;
  uint8_t secondaryPin = 0xFF;
  uint8_t address = 0x00;
  uint8_t deviceId = 0x00;
};

inline const char* sensorBusTypeName(SensorBusType busType) {
  switch (busType) {
    case SensorBusType::GpioPulse:
      return "gpio-pulse";
    case SensorBusType::GpioDigital:
      return "gpio-digital";
    case SensorBusType::Analog:
      return "analog";
    case SensorBusType::Uart:
      return "uart";
    case SensorBusType::I2c:
      return "i2c";
    case SensorBusType::Rs485:
      return "rs485";
    case SensorBusType::Spi:
      return "spi";
    case SensorBusType::Can:
      return "can";
    default:
      return "unknown";
  }
}

struct SelfTestReport {
  bool passed = false;
  SensorStatus status = SensorStatus::NotReady;
  uint16_t value = 0;
  uint8_t detail = 0;
};

struct RangeReading {
  SensorStatus status = SensorStatus::NotReady;
  uint16_t distanceMm = 0;
  uint16_t rawValue = 0;
  uint32_t timestampMs = 0;

  bool valid() const {
    return status == SensorStatus::Ok;
  }
};

struct ProximityReading {
  SensorStatus status = SensorStatus::NotReady;
  DetectionState state = DetectionState::Inactive;
  uint16_t strengthPermille = 0;
  uint16_t rawValue = 0;
  uint32_t timestampMs = 0;

  bool detected() const {
    return state == DetectionState::Active;
  }
};

struct MotionReading {
  SensorStatus status = SensorStatus::NotReady;
  DetectionState state = DetectionState::Inactive;
  bool rose = false;
  bool fell = false;
  uint32_t timestampMs = 0;
  uint32_t lastActiveMs = 0;

  bool detected() const {
    return state == DetectionState::Active;
  }
};

struct CalibrationData {
  int16_t scalePermille = 1000;
  int16_t offsetMm = 0;
};

inline uint16_t clampToPermille(int32_t value) {
  if (value <= 0) {
    return 0;
  }
  if (value >= 1000) {
    return 1000;
  }
  return static_cast<uint16_t>(value);
}

inline uint16_t applyCalibrationMm(uint16_t valueMm, const CalibrationData& calibration) {
  int32_t adjusted = (static_cast<int32_t>(valueMm) * calibration.scalePermille) / 1000;
  adjusted += calibration.offsetMm;
  if (adjusted < 0) {
    adjusted = 0;
  }
  if (adjusted > 65535) {
    adjusted = 65535;
  }
  return static_cast<uint16_t>(adjusted);
}

}  // namespace DiFinders
