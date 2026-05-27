#pragma once

#include <Arduino.h>

#include "LD2410RadarSensor.h"

namespace DiFinders {

class LD2420RadarSensor : public LD2410RadarSensor {
 public:
  enum class FirmwareGeneration : uint8_t {
    Legacy256000,
    Modern115200,
    Custom,
  };

  enum class TunedProfile : uint8_t {
    RawCompatibility,
    FastPresence,
    BalancedIndoor,
    LongRange,
  };

  static constexpr uint32_t kLegacyDefaultBaudRate = 256000UL;
  static constexpr uint32_t kModernDefaultBaudRate = 115200UL;

  LD2420RadarSensor() = default;
  explicit LD2420RadarSensor(Stream& serial) : LD2410RadarSensor(serial) {}

  void begin(Stream& serial, FirmwareGeneration generation, bool waitForRadar = false) {
    if (generation == FirmwareGeneration::Legacy256000) {
      useLegacyDefaultBaudRate();
    } else if (generation == FirmwareGeneration::Modern115200) {
      useModernDefaultBaudRate();
    }
    LD2410RadarSensor::begin(serial, waitForRadar);
  }

  /** Uses defaultBaudRate / firmware hint (pair with Serial1.begin(baud) in sketches). */
  void begin(Stream& serial, bool waitForRadar = false) {
    begin(serial, firmwareGenerationHint_, waitForRadar);
  }

  uint32_t defaultBaudRate() const {
    return defaultBaudRate_;
  }

  void setDefaultBaudRate(uint32_t baudRate) {
    defaultBaudRate_ = baudRate;
    firmwareGenerationHint_ = FirmwareGeneration::Custom;
  }

  void useLegacyDefaultBaudRate() {
    defaultBaudRate_ = kLegacyDefaultBaudRate;
    firmwareGenerationHint_ = FirmwareGeneration::Legacy256000;
  }

  void useModernDefaultBaudRate() {
    defaultBaudRate_ = kModernDefaultBaudRate;
    firmwareGenerationHint_ = FirmwareGeneration::Modern115200;
  }

  FirmwareGeneration firmwareGenerationHint() const {
    return firmwareGenerationHint_;
  }

  bool isUsingLegacyDefaultBaudRate() const {
    return firmwareGenerationHint_ == FirmwareGeneration::Legacy256000;
  }

  bool isUsingModernDefaultBaudRate() const {
    return firmwareGenerationHint_ == FirmwareGeneration::Modern115200;
  }

  TunedProfile tunedProfile() const {
    return tunedProfile_;
  }

  bool applyFastPresenceProfile() {
    const bool ok =
        setMaxValues(6, 6, 2) && setGateSensitivity(0, 85, 85) && setGateSensitivity(1, 80, 85) &&
        setGateSensitivity(2, 75, 80) && setGateSensitivity(3, 65, 75) && setGateSensitivity(4, 55, 65) &&
        setGateSensitivity(5, 45, 55) && setGateSensitivity(6, 35, 45);
    if (ok) {
      tunedProfile_ = TunedProfile::FastPresence;
    }
    return ok;
  }

  bool applyBalancedIndoorProfile() {
    const bool ok =
        setMaxValues(8, 8, 5) && setGateSensitivity(0, 85, 85) && setGateSensitivity(1, 80, 85) &&
        setGateSensitivity(2, 80, 80) && setGateSensitivity(3, 75, 80) && setGateSensitivity(4, 70, 75) &&
        setGateSensitivity(5, 65, 70) && setGateSensitivity(6, 55, 65) && setGateSensitivity(7, 45, 55) &&
        setGateSensitivity(8, 35, 45);
    if (ok) {
      tunedProfile_ = TunedProfile::BalancedIndoor;
    }
    return ok;
  }

  bool applyLongRangeProfile() {
    const bool ok =
        setMaxValues(8, 8, 8) && setGateSensitivity(0, 70, 75) && setGateSensitivity(1, 70, 75) &&
        setGateSensitivity(2, 75, 80) && setGateSensitivity(3, 80, 85) && setGateSensitivity(4, 85, 85) &&
        setGateSensitivity(5, 85, 80) && setGateSensitivity(6, 80, 70) && setGateSensitivity(7, 70, 55) &&
        setGateSensitivity(8, 60, 45);
    if (ok) {
      tunedProfile_ = TunedProfile::LongRange;
    }
    return ok;
  }

  void clearTunedProfile() {
    tunedProfile_ = TunedProfile::RawCompatibility;
  }

 private:
  uint32_t defaultBaudRate_ = kLegacyDefaultBaudRate;
  FirmwareGeneration firmwareGenerationHint_ = FirmwareGeneration::Legacy256000;
  TunedProfile tunedProfile_ = TunedProfile::RawCompatibility;
};

}  // namespace DiFinders