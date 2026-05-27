#pragma once

#include <Arduino.h>
#include <Stream.h>
#include <math.h>

#include "../../core/DiFindersDiagnostics.h"
#include "../../core/DiFindersTypes.h"

namespace DiFinders {

class LD2461RadarSensor {
 public:
  struct Target {
    int16_t xMm = 0;
    int16_t yMm = 0;
    uint32_t lastSeenMs = 0;

    bool present() const {
      return xMm != 0 || yMm != 0;
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

  enum class ReportMode : uint8_t {
    Coordinates = 0x01,
    TargetCount = 0x02,
    CoordinatesAndCount = 0x03,
  };

  enum class BaudRateCode : uint8_t {
    Baud9600 = 0x01,
    Baud19200 = 0x02,
    Baud38400 = 0x03,
    Baud57600 = 0x04,
    Baud115200 = 0x05,
    Baud256000 = 0x06,
  };

  static constexpr uint32_t kConservativeDefaultBaudRate = 9600UL;
  static constexpr uint32_t kFastDefaultBaudRate = 115200UL;

  LD2461RadarSensor() = default;
  explicit LD2461RadarSensor(Stream& serial) {
    begin(serial);
  }

  void begin(Stream& serial) {
    serial_ = &serial;
    frameLength_ = 0;
    lastCommandCode_ = 0;
    lastPayloadLength_ = 0;
    lastTargetUpdateMs_ = 0;
    targetCount_ = 0;
    clearTargets();
  }

  void setSensorTimeout(uint16_t timeoutMs) {
    sensorTimeoutMs_ = timeoutMs;
  }

  void setCommandTimeout(uint16_t timeoutMs) {
    commandTimeoutMs_ = timeoutMs;
  }

  void setTargetFreshnessTimeout(uint16_t timeoutMs) {
    targetFreshnessTimeoutMs_ = timeoutMs;
  }

  uint16_t targetFreshnessTimeout() const {
    return targetFreshnessTimeoutMs_;
  }

  void setDefaultBaudRate(uint32_t baudRate) {
    defaultBaudRate_ = baudRate;
  }

  uint32_t defaultBaudRate() const {
    return defaultBaudRate_;
  }

  bool read() {
    if (serial_ == nullptr) {
      return false;
    }

    bool processed = false;
    while (serial_->available() > 0) {
      processed = true;
      consumeByte(static_cast<uint8_t>(serial_->read()));
    }
    return processed;
  }

  bool isConnected() const {
    return lastMessageReceivedMs_ != 0 && (millis() - lastMessageReceivedMs_) <= sensorTimeoutMs_;
  }

  bool occupied() const {
    return freshTargetCount() > 0;
  }

  uint8_t targetCount() const {
    return targetCount_;
  }

  uint8_t freshTargetCount() const {
    uint8_t count = 0;
    for (uint8_t index = 0; index < 5; ++index) {
      count += targetFresh(index) ? 1 : 0;
    }
    return count;
  }

  const Target& target(uint8_t index) const {
    static Target empty;
    return index < 5 ? targets_[index] : empty;
  }

  bool targetFresh(uint8_t index) const {
    return index < 5 ? isTargetFresh(targets_[index], millis()) : false;
  }

  uint32_t lastTargetUpdateMs() const {
    return lastTargetUpdateMs_;
  }

  bool trackingDataFresh() const {
    return lastTargetUpdateMs_ != 0 && (millis() - lastTargetUpdateMs_) <= targetFreshnessTimeoutMs_;
  }

  uint16_t nearestTargetDistanceMm() const {
    const uint32_t now = millis();
    uint16_t nearest = 0;
    for (uint8_t index = 0; index < 5; ++index) {
      if (!isTargetFresh(targets_[index], now)) {
        continue;
      }
      const uint16_t distance = targets_[index].distanceMm();
      if (distance == 0) {
        continue;
      }
      if (nearest == 0 || distance < nearest) {
        nearest = distance;
      }
    }
    return nearest;
  }

  ReportMode reportMode() const {
    return reportMode_;
  }

  bool regionsEnabled() const {
    return regionsEnabled_;
  }

  uint8_t lastCommandCode() const {
    return lastCommandCode_;
  }

  uint8_t lastPayloadLength() const {
    return lastPayloadLength_;
  }

  const uint8_t* lastPayload() const {
    return lastPayload_;
  }

  const MotionReading& lastReading() const {
    return lastMotionReading_;
  }

  MotionReading motion() const {
    return lastMotionReading_;
  }

  bool setReportMode(ReportMode mode) {
    const uint8_t payload[] = {static_cast<uint8_t>(mode)};
    const bool ok = sendCommand(0x02, payload, sizeof(payload));
    if (ok) {
      reportMode_ = mode;
    }
    return ok;
  }

  bool requestReportMode() {
    return sendCommand(0x03, nullptr, 0);
  }

  bool requestCoordinates() {
    return sendCommand(0x07, nullptr, 0);
  }

  bool requestTargetCount() {
    return sendCommand(0x08, nullptr, 0);
  }

  bool requestTrackingSnapshot() {
    return requestTargetCount() && requestCoordinates();
  }

  bool requestFirmwareVersion() {
    return sendCommand(0x09, nullptr, 0);
  }

  bool requestFactoryReset() {
    return sendCommand(0x0A, nullptr, 0);
  }

  bool setBaudRate(BaudRateCode baudRate) {
    const uint8_t payload[] = {static_cast<uint8_t>(baudRate)};
    const bool ok = sendCommand(0x01, payload, sizeof(payload));
    if (ok) {
      defaultBaudRate_ = baudRateValue(baudRate);
    }
    return ok;
  }

  bool setRegionsEnabled(bool enabled) {
    const bool ok = sendCommand(enabled ? 0x04 : 0x05, nullptr, 0);
    if (ok) {
      regionsEnabled_ = enabled;
    }
    return ok;
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
    report.value = targetCount_;
    report.detail = lastCommandCode_;
    return report;
  }


  /* --- DiFinders diagnostics API (unified naming) --- */
  uint8_t lastBusError() const { return 0; }
  bool ready() const {
    return isConnected();
  }

  SensorHealth health() const {
    return makeHealth(ready(), probe().busType, SensorStatus::Ok, 0, 0, 0);
  }

 private:
  static constexpr uint8_t kHeader[3] = {0xFF, 0xEE, 0xDD};
  static constexpr uint8_t kFooter[3] = {0xDD, 0xEE, 0xFF};

  static uint32_t baudRateValue(BaudRateCode baudRate) {
    switch (baudRate) {
      case BaudRateCode::Baud9600:
        return 9600UL;
      case BaudRateCode::Baud19200:
        return 19200UL;
      case BaudRateCode::Baud38400:
        return 38400UL;
      case BaudRateCode::Baud57600:
        return 57600UL;
      case BaudRateCode::Baud115200:
        return 115200UL;
      case BaudRateCode::Baud256000:
        return 256000UL;
      default:
        return kConservativeDefaultBaudRate;
    }
  }

  bool sendCommand(uint8_t commandCode, const uint8_t* payload, uint8_t payloadLength) {
    if (serial_ == nullptr) {
      return false;
    }

    serial_->write(kHeader, sizeof(kHeader));
    serial_->write(commandCode);
    serial_->write(payloadLength);

    uint8_t checksum = static_cast<uint8_t>(commandCode + payloadLength);
    for (uint8_t index = 0; index < payloadLength; ++index) {
      serial_->write(payload[index]);
      checksum = static_cast<uint8_t>(checksum + payload[index]);
    }
    serial_->write(checksum);
    serial_->write(kFooter, sizeof(kFooter));

    const uint32_t startMs = millis();
    while (millis() - startMs < commandTimeoutMs_) {
      read();
      if (lastCommandCode_ == commandCode) {
        return true;
      }
      yield();
    }
    return false;
  }

  void consumeByte(uint8_t value) {
    if (frameLength_ == 0) {
      if (value == kHeader[0]) {
        frame_[0] = value;
        frameLength_ = 1;
      }
      return;
    }

    if (frameLength_ >= sizeof(frame_)) {
      frameLength_ = 0;
      return;
    }

    frame_[frameLength_++] = value;

    if (frameLength_ == 2 && frame_[1] != kHeader[1]) {
      frameLength_ = 0;
      return;
    }
    if (frameLength_ == 3 && frame_[2] != kHeader[2]) {
      frameLength_ = 0;
      return;
    }

    if (frameLength_ >= 8 && frame_[frameLength_ - 3] == kFooter[0] && frame_[frameLength_ - 2] == kFooter[1] &&
        frame_[frameLength_ - 1] == kFooter[2]) {
      parseFrame();
      frameLength_ = 0;
    }
  }

  void parseFrame() {
    if (frameLength_ < 8) {
      return;
    }

    const uint8_t commandCode = frame_[3];
    const uint8_t payloadLength = frame_[4];
    const uint8_t expectedLength = static_cast<uint8_t>(3 + 1 + 1 + payloadLength + 1 + 3);
    if (frameLength_ != expectedLength) {
      return;
    }

    uint8_t checksum = static_cast<uint8_t>(commandCode + payloadLength);
    for (uint8_t index = 0; index < payloadLength; ++index) {
      checksum = static_cast<uint8_t>(checksum + frame_[5 + index]);
    }
    if (checksum != frame_[5 + payloadLength]) {
      return;
    }

    lastMessageReceivedMs_ = millis();
    lastCommandCode_ = commandCode;
    lastPayloadLength_ = payloadLength > sizeof(lastPayload_) ? sizeof(lastPayload_) : payloadLength;
    for (uint8_t index = 0; index < lastPayloadLength_; ++index) {
      lastPayload_[index] = frame_[5 + index];
    }

    if (commandCode == 0x03 && payloadLength >= 1) {
      const uint8_t modeByte = frame_[5];
      if (modeByte >= 0x01 && modeByte <= 0x03) {
        reportMode_ = static_cast<ReportMode>(modeByte);
      }
    } else if (commandCode == 0x08 && payloadLength >= 1) {
      targetCount_ = frame_[5] > 5 ? 5 : frame_[5];
      lastTargetUpdateMs_ = lastMessageReceivedMs_;
      if (targetCount_ == 0) {
        clearTargets();
      }
    } else if (commandCode == 0x07 && payloadLength >= 4) {
      parseCoordinatePayload(&frame_[5], payloadLength);
    } else if (commandCode == 0x04) {
      regionsEnabled_ = true;
    } else if (commandCode == 0x05) {
      regionsEnabled_ = false;
    }

    MotionReading reading;
    reading.status = SensorStatus::Ok;
    reading.state = occupied() ? DetectionState::Active : DetectionState::Inactive;
    reading.rose = occupied() && !lastMotionReading_.detected();
    reading.fell = !occupied() && lastMotionReading_.detected();
    reading.timestampMs = lastMessageReceivedMs_;
    reading.lastActiveMs = occupied() ? lastMessageReceivedMs_ : lastMotionReading_.lastActiveMs;
    lastMotionReading_ = reading;
  }

  void parseCoordinatePayload(const uint8_t* payload, uint8_t payloadLength) {
    uint8_t offset = 0;
    uint8_t reportedTargetCount = 0xFF;
    if (payloadLength >= 1 && payload[0] <= 5 && ((payloadLength - 1) % 4) == 0) {
      reportedTargetCount = payload[0];
      offset = 1;
    }

    const uint8_t targetSlots = static_cast<uint8_t>((payloadLength - offset) / 4);
    const uint8_t limit = targetSlots > 5 ? 5 : targetSlots;
    targetCount_ = 0;
    for (uint8_t index = 0; index < 5; ++index) {
      if (index < limit) {
        const uint8_t targetOffset = static_cast<uint8_t>(offset + (index * 4));
        targets_[index].xMm = static_cast<int16_t>(payload[targetOffset] | (payload[targetOffset + 1] << 8));
        targets_[index].yMm = static_cast<int16_t>(payload[targetOffset + 2] | (payload[targetOffset + 3] << 8));
        targets_[index].lastSeenMs = millis();
        targetCount_ += targets_[index].present() ? 1 : 0;
      } else {
        targets_[index] = Target();
      }
    }

    if (reportedTargetCount <= 5 && reportedTargetCount < targetCount_) {
      targetCount_ = reportedTargetCount;
    }
    lastTargetUpdateMs_ = millis();
  }

  bool isTargetFresh(const Target& target, uint32_t now) const {
    return target.present() && target.lastSeenMs != 0 && (now - target.lastSeenMs) <= targetFreshnessTimeoutMs_;
  }

  void clearTargets() {
    for (uint8_t index = 0; index < 5; ++index) {
      targets_[index] = Target();
    }
  }

  Stream* serial_ = nullptr;
  uint16_t sensorTimeoutMs_ = 1500;
  uint16_t commandTimeoutMs_ = 180;
  uint16_t targetFreshnessTimeoutMs_ = 500;
  uint32_t defaultBaudRate_ = kConservativeDefaultBaudRate;
  uint8_t frame_[48] = {0};
  uint8_t frameLength_ = 0;
  uint32_t lastMessageReceivedMs_ = 0;
  uint32_t lastTargetUpdateMs_ = 0;
  uint8_t lastCommandCode_ = 0;
  uint8_t lastPayload_[24] = {0};
  uint8_t lastPayloadLength_ = 0;
  uint8_t targetCount_ = 0;
  Target targets_[5];
  bool regionsEnabled_ = true;
  ReportMode reportMode_ = ReportMode::CoordinatesAndCount;
  MotionReading lastMotionReading_;
};

}  // namespace DiFinders