#pragma once

#include <Arduino.h>
#include <Stream.h>
#include <math.h>

#include "../../core/DiFindersTypes.h"
#include "../../core/DiFindersDiagnostics.h"

namespace DiFinders {

class LD2450RadarSensor {
 public:
  struct Target {
    int16_t xMm = 0;
    int16_t yMm = 0;
    int16_t speedMmPerSecond = 0;
    int16_t resolutionMm = 0;
    uint32_t lastSeenMs = 0;

    bool present() const {
      return resolutionMm != 0;
    }

    float angleDegrees() const {
      if (!present()) {
        return NAN;
      }
      return -(atan2(static_cast<float>(yMm), static_cast<float>(xMm)) * (180.0f / PI) - 90.0f);
    }

    uint16_t distanceMm() const {
      if (!present()) {
        return 0;
      }
      const float distance = sqrtf(static_cast<float>(xMm) * static_cast<float>(xMm) +
                                   static_cast<float>(yMm) * static_cast<float>(yMm));
      return static_cast<uint16_t>(distance > 65535.0f ? 65535.0f : distance);
    }
  };

  enum class BaudRateCode : uint8_t {
    Baud9600 = 0x01,
    Baud19200 = 0x02,
    Baud38400 = 0x03,
    Baud57600 = 0x04,
    Baud115200 = 0x05,
    Baud230400 = 0x06,
    Baud256000 = 0x07,
    Baud460800 = 0x08,
  };

  LD2450RadarSensor() = default;
  explicit LD2450RadarSensor(Stream& serial) {
    begin(serial);
  }

  void begin(Stream& serial) {
    serial_ = &serial;
    configFrameLength_ = 0;
    configExpectedPayloadLength_ = 0;
  }

  void setSensorTimeout(uint16_t timeoutMs) {
    sensorTimeoutMs_ = timeoutMs;
  }

  void setCommandTimeout(uint16_t timeoutMs) {
    commandTimeoutMs_ = timeoutMs;
  }

  void setFlipXAxis(bool enabled) {
    flipXAxis_ = enabled;
  }

  void setFastOffDetection(bool enabled) {
    fastOffDetection_ = enabled;
  }

  void setMaxDetectionDistanceMm(int16_t distanceMm) {
    maxDetectionDistanceMm_ = distanceMm;
  }

  void setMaxDistanceMarginMm(int16_t marginMm) {
    maxDistanceMarginMm_ = marginMm;
  }

  void setTiltAngleLimits(float minAngleDegrees, float maxAngleDegrees) {
    minTiltAngleDegrees_ = minAngleDegrees;
    maxTiltAngleDegrees_ = maxAngleDegrees;
  }

  void setTiltAngleMargin(float marginDegrees) {
    tiltAngleMarginDegrees_ = marginDegrees;
  }

  uint32_t defaultBaudRate() const {
    return 256000UL;
  }

  bool read() {
    if (serial_ == nullptr) {
      return false;
    }

    bool processed = false;
    while (serial_->available() >= 4) {
      const int first = serial_->peek();
      if (first < 0) {
        break;
      }

      if (static_cast<uint8_t>(first) == kUpdateHeader[0]) {
        if (!readUpdateFrame()) {
          break;
        }
        processed = true;
      } else if (static_cast<uint8_t>(first) == kConfigHeader[0]) {
        if (!readConfigFrame()) {
          break;
        }
        processed = true;
      } else {
        serial_->read();
      }
    }

    return processed;
  }

  bool isConnected() const {
    return lastMessageReceivedMs_ != 0 && (millis() - lastMessageReceivedMs_) <= sensorTimeoutMs_;
  }

  bool occupied() const {
    return occupied_;
  }

  uint8_t targetCount() const {
    uint8_t count = 0;
    for (const Target& target : targets_) {
      count += target.present() ? 1 : 0;
    }
    return count;
  }

  const Target& target(uint8_t index) const {
    static Target empty;
    return index < 3 ? targets_[index] : empty;
  }

  bool multiTargetModeEnabled() const {
    return multiTargetModeEnabled_;
  }

  bool bluetoothEnabled() const {
    return bluetoothEnabled_;
  }

  uint8_t firmwareMajorVersion() const {
    return firmwareMajorVersion_;
  }

  uint8_t firmwareMinorVersion() const {
    return firmwareMinorVersion_;
  }

  uint32_t firmwarePatchVersion() const {
    return firmwarePatchVersion_;
  }

  bool requestFirmwareVersion() {
    const uint8_t payload[] = {0xA0, 0x00};
    return sendConfigCommand(payload, sizeof(payload));
  }

  bool requestTrackingMode() {
    const uint8_t payload[] = {0x91, 0x00};
    return sendConfigCommand(payload, sizeof(payload));
  }

  bool setTrackingMode(bool multiTargetMode) {
    const uint8_t payload[] = {static_cast<uint8_t>(multiTargetMode ? 0x90 : 0x80), 0x00};
    const bool result = sendConfigCommand(payload, sizeof(payload));
    if (result) {
      multiTargetModeEnabled_ = multiTargetMode;
    }
    return result;
  }

  bool setBluetoothEnabled(bool enabled) {
    const uint8_t payload[] = {0xA4, 0x00, static_cast<uint8_t>(enabled ? 0x01 : 0x00), 0x00};
    const bool result = sendConfigCommand(payload, sizeof(payload));
    if (result) {
      bluetoothEnabled_ = enabled;
    }
    return result;
  }

  bool setBaudRate(BaudRateCode baudRate) {
    const uint8_t payload[] = {0xA1, 0x00, static_cast<uint8_t>(baudRate), 0x00};
    return sendConfigCommand(payload, sizeof(payload));
  }

  bool requestRestart() {
    const uint8_t payload[] = {0xA3, 0x00};
    return sendConfigCommand(payload, sizeof(payload));
  }

  bool requestFactoryReset() {
    const uint8_t payload[] = {0xA2, 0x00};
    return sendConfigCommand(payload, sizeof(payload));
  }

  const MotionReading& lastReading() const {
    return lastMotionReading_;
  }

  MotionReading motion() const {
    return lastMotionReading_;
  }

  SensorProbe probe() const {
    SensorProbe info;
    info.kind = SensorKind::Motion;
    info.busType = SensorBusType::Uart;
    info.configured = serial_ != nullptr;
    info.supportsContinuousMode = true;
    return info;
  }

  SelfTestReport selfTest() {
    SelfTestReport report;
    report.passed = serial_ != nullptr;
    if (!report.passed) {
      report.status = SensorStatus::NotReady;
      return report;
    }
    read();
    report.status = isConnected() ? SensorStatus::Ok : SensorStatus::Timeout;
    report.passed = report.status == SensorStatus::Ok;
    report.value = targetCount();
    report.detail = occupied_ ? 1 : 0;
    return report;
  }

  uint8_t lastBusError() const { return 0; }

  uint32_t faultCount() const { return 0; }

  SensorHealth health() const {
    const SensorStatus st = isConnected() ? SensorStatus::Ok : SensorStatus::Timeout;
    const uint16_t distMm = targets_[0].present() ? targets_[0].distanceMm() : 0;
    return makeHealth(serial_ != nullptr, probe().busType, st, 0, distMm, faultCount());
  }

 private:
  static constexpr uint8_t kUpdateHeader[4] = {0xAA, 0xFF, 0x03, 0x00};
  static constexpr uint8_t kUpdateFooter[2] = {0x55, 0xCC};
  static constexpr uint8_t kConfigHeader[4] = {0xFD, 0xFC, 0xFB, 0xFA};
  static constexpr uint8_t kConfigFooter[4] = {0x04, 0x03, 0x02, 0x01};

  bool readUpdateFrame() {
    if (serial_->available() < 30) {
      return false;
    }

    uint8_t header[4] = {0};
    serial_->readBytes(reinterpret_cast<char*>(header), sizeof(header));
    if (!(header[0] == kUpdateHeader[0] && header[1] == kUpdateHeader[1] && header[2] == kUpdateHeader[2] && header[3] == kUpdateHeader[3])) {
      return false;
    }

    uint8_t msg[26] = {0};
    if (serial_->readBytes(reinterpret_cast<char*>(msg), sizeof(msg)) != sizeof(msg)) {
      return false;
    }
    if (msg[24] != kUpdateFooter[0] || msg[25] != kUpdateFooter[1]) {
      return false;
    }

    processUpdateMessage(msg, 24);
    return true;
  }

  bool readConfigFrame() {
    if (serial_->available() < 10) {
      return false;
    }

    uint8_t header[4] = {0};
    serial_->readBytes(reinterpret_cast<char*>(header), sizeof(header));
    if (!(header[0] == kConfigHeader[0] && header[1] == kConfigHeader[1] && header[2] == kConfigHeader[2] && header[3] == kConfigHeader[3])) {
      return false;
    }

    uint8_t lengthBytes[2] = {0};
    if (serial_->readBytes(reinterpret_cast<char*>(lengthBytes), sizeof(lengthBytes)) != sizeof(lengthBytes)) {
      return false;
    }
    const uint16_t payloadLength = static_cast<uint16_t>(lengthBytes[0] | (lengthBytes[1] << 8));
    if (payloadLength > 32 || serial_->available() < payloadLength + 4) {
      return false;
    }

    uint8_t msg[36] = {0};
    if (serial_->readBytes(reinterpret_cast<char*>(msg), payloadLength + 4) != payloadLength + 4) {
      return false;
    }
    if (msg[payloadLength] != kConfigFooter[0] || msg[payloadLength + 1] != kConfigFooter[1] ||
        msg[payloadLength + 2] != kConfigFooter[2] || msg[payloadLength + 3] != kConfigFooter[3]) {
      return false;
    }

    processConfigMessage(msg, payloadLength);
    return true;
  }

  void processUpdateMessage(const uint8_t* msg, uint8_t len) {
    (void)len;
    lastMessageReceivedMs_ = millis();

    uint8_t count = 0;
    for (uint8_t index = 0; index < 3; ++index) {
      const uint8_t offset = index * 8;
      int16_t x = static_cast<int16_t>(msg[offset + 1] << 8 | msg[offset + 0]);
      if (msg[offset + 1] & 0x80) {
        x = static_cast<int16_t>(-x + 0x8000);
      }
      int16_t y = static_cast<int16_t>(msg[offset + 3] << 8 | msg[offset + 2]);
      if (y != 0) {
        y = static_cast<int16_t>(y - 0x8000);
      }
      int16_t speed = static_cast<int16_t>(msg[offset + 5] << 8 | msg[offset + 4]);
      if (msg[offset + 5] & 0x80) {
        speed = static_cast<int16_t>(-speed + 0x8000);
      }
      int16_t resolution = static_cast<int16_t>(msg[offset + 7] << 8 | msg[offset + 6]);

      if (flipXAxis_) {
        x = static_cast<int16_t>(-x);
      }

      const float angle = -(atan2(static_cast<float>(y), static_cast<float>(x == 0 ? 1 : x)) * (180.0f / PI) - 90.0f);
      const bool withinDistance = y <= maxDetectionDistanceMm_ || (targets_[index].present() && y <= maxDetectionDistanceMm_ + maxDistanceMarginMm_);
      const bool withinAngle = angle <= maxTiltAngleDegrees_ + (targets_[index].present() ? tiltAngleMarginDegrees_ : 0.0f) &&
                               angle >= minTiltAngleDegrees_ - (targets_[index].present() ? tiltAngleMarginDegrees_ : 0.0f);

      if (resolution != 0 && withinDistance && withinAngle) {
        if (fastOffDetection_ && targets_[index].present() &&
            (targets_[index].xMm != x || targets_[index].yMm != y || targets_[index].speedMmPerSecond != speed ||
             targets_[index].resolutionMm != resolution)) {
          targets_[index].lastSeenMs = millis();
        }
        targets_[index].xMm = x;
        targets_[index].yMm = y;
        targets_[index].speedMmPerSecond = speed;
        targets_[index].resolutionMm = resolution;
        targets_[index].lastSeenMs = millis();
        ++count;
      } else {
        targets_[index] = Target();
      }
    }

    occupied_ = count > 0;
    MotionReading reading;
    reading.status = SensorStatus::Ok;
    reading.state = occupied_ ? DetectionState::Active : DetectionState::Inactive;
    reading.rose = occupied_ && !lastMotionReading_.detected();
    reading.fell = !occupied_ && lastMotionReading_.detected();
    reading.timestampMs = lastMessageReceivedMs_;
    reading.lastActiveMs = occupied_ ? lastMessageReceivedMs_ : lastMotionReading_.lastActiveMs;
    lastMotionReading_ = reading;
  }

  void processConfigMessage(const uint8_t* msg, uint8_t len) {
    if (len < 2) {
      return;
    }
    lastMessageReceivedMs_ = millis();
    lastCommandCode_ = msg[0];
    lastCommandSucceeded_ = msg[1] == 0x01;
    if (!lastCommandSucceeded_) {
      return;
    }

    if (msg[0] == 0xA0 && len >= 12) {
      firmwareMajorVersion_ = msg[7];
      firmwareMinorVersion_ = msg[6];
      firmwarePatchVersion_ = static_cast<uint32_t>(msg[8]) |
                              (static_cast<uint32_t>(msg[9]) << 8) |
                              (static_cast<uint32_t>(msg[10]) << 16) |
                              (static_cast<uint32_t>(msg[11]) << 24);
    } else if (msg[0] == 0x91 && len >= 5) {
      multiTargetModeEnabled_ = msg[4] == 0x02;
    } else if (msg[0] == 0xA4 && len >= 10) {
      bluetoothEnabled_ = !(msg[4] == 0x08 && msg[5] == 0x05 && msg[6] == 0x04 && msg[7] == 0x03 && msg[8] == 0x02 && msg[9] == 0x01);
    }
  }

  bool sendConfigCommand(const uint8_t* payload, uint16_t length) {
    if (serial_ == nullptr) {
      return false;
    }

    for (uint8_t value : kConfigHeader) {
      serial_->write(value);
    }
    serial_->write(static_cast<uint8_t>(length & 0xFF));
    serial_->write(static_cast<uint8_t>(length >> 8));
    for (uint16_t index = 0; index < length; ++index) {
      serial_->write(payload[index]);
    }
    for (uint8_t value : kConfigFooter) {
      serial_->write(value);
    }

    const uint8_t awaitedCommand = payload[0];
    const uint32_t startMs = millis();
    while (millis() - startMs < commandTimeoutMs_) {
      read();
      if (lastCommandCode_ == awaitedCommand) {
        return lastCommandSucceeded_;
      }
      yield();
    }
    return false;
  }

  Stream* serial_ = nullptr;
  uint16_t sensorTimeoutMs_ = 4000;
  uint16_t commandTimeoutMs_ = 300;
  bool flipXAxis_ = false;
  bool fastOffDetection_ = false;
  int16_t maxDetectionDistanceMm_ = 6000;
  int16_t maxDistanceMarginMm_ = 250;
  float minTiltAngleDegrees_ = -90.0f;
  float maxTiltAngleDegrees_ = 90.0f;
  float tiltAngleMarginDegrees_ = 5.0f;
  bool occupied_ = false;
  bool multiTargetModeEnabled_ = true;
  bool bluetoothEnabled_ = true;
  uint32_t lastMessageReceivedMs_ = 0;
  uint8_t lastCommandCode_ = 0;
  bool lastCommandSucceeded_ = false;
  uint8_t firmwareMajorVersion_ = 0;
  uint8_t firmwareMinorVersion_ = 0;
  uint32_t firmwarePatchVersion_ = 0;
  uint16_t configFrameLength_ = 0;
  uint16_t configExpectedPayloadLength_ = 0;
  MotionReading lastMotionReading_;
  Target targets_[3];
};

}  // namespace DiFinders
