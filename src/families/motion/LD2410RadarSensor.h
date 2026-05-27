#pragma once

#include <Arduino.h>
#include <Stream.h>

#include "../../core/DiFindersDma.h"
#include "../../core/DiFindersTypes.h"

namespace DiFinders {

class LD2410RadarSensor {
 public:
  LD2410RadarSensor() = default;
  explicit LD2410RadarSensor(Stream& serial) {
    begin(serial);
  }

  void begin(Stream& serial, bool waitForRadar = false) {
    serial_ = &serial;
    frameLength_ = 0;
    expectedFrameLength_ = 0;
    pendingAck_ = 0;
    ackMatched_ = false;
    if (waitForRadar) {
      requestFirmwareVersion();
    }
  }

  void setTimeout(uint16_t timeoutMs) {
    timeoutMs_ = timeoutMs;
  }

  void setCommandTimeout(uint16_t timeoutMs) {
    commandTimeoutMs_ = timeoutMs;
  }

  uint32_t defaultBaudRate() const {
    return 256000UL;
  }

  bool ready() const {
    return serial_ != nullptr;
  }

  bool read() {
    return processIncomingFrames(0);
  }

  bool isConnected() {
    if (millis() - lastPacketMs_ <= timeoutMs_) {
      return true;
    }
    return processIncomingFrames(2) && millis() - lastPacketMs_ <= timeoutMs_;
  }

  bool presenceDetected() const {
    return targetType_ != 0;
  }

  bool stationaryTargetDetected() const {
    return (targetType_ & 0x02) != 0 && stationaryTargetDistanceCm_ > 0 && stationaryTargetEnergy_ > 0;
  }

  bool movingTargetDetected() const {
    return (targetType_ & 0x01) != 0 && movingTargetDistanceCm_ > 0 && movingTargetEnergy_ > 0;
  }

  uint16_t stationaryTargetDistanceCm() const {
    return stationaryTargetDistanceCm_;
  }

  uint16_t movingTargetDistanceCm() const {
    return movingTargetDistanceCm_;
  }

  uint8_t stationaryTargetEnergy() const {
    return stationaryTargetEnergy_ > 100 ? 100 : stationaryTargetEnergy_;
  }

  uint8_t movingTargetEnergy() const {
    return movingTargetEnergy_ > 100 ? 100 : movingTargetEnergy_;
  }

  uint8_t targetType() const {
    return targetType_;
  }

  uint16_t detectedDistanceCm() const {
    return detectedDistanceCm_;
  }

  const MotionReading& lastReading() const {
    return lastMotionReading_;
  }

  MotionReading motion() const {
    return lastMotionReading_;
  }

  uint8_t firmwareMajorVersion() const {
    return firmwareMajorVersion_;
  }

  uint8_t firmwareMinorVersion() const {
    return firmwareMinorVersion_;
  }

  uint32_t firmwareBugfixVersion() const {
    return firmwareBugfixVersion_;
  }

  uint8_t maxGate() const {
    return maxGate_;
  }

  uint8_t maxMovingGate() const {
    return maxMovingGate_;
  }

  uint8_t maxStationaryGate() const {
    return maxStationaryGate_;
  }

  uint16_t idleTimeoutSeconds() const {
    return idleTimeoutSeconds_;
  }

  uint8_t motionSensitivity(uint8_t gate) const {
    return gate < 9 ? motionSensitivity_[gate] : 0;
  }

  uint8_t stationarySensitivity(uint8_t gate) const {
    return gate < 9 ? stationarySensitivity_[gate] : 0;
  }

  bool requestFirmwareVersion() {
    const uint8_t payload[] = {0xA0, 0x00};
    return sendCommand(payload, sizeof(payload), 0xA0);
  }

  bool requestCurrentConfiguration() {
    const uint8_t payload[] = {0x61, 0x00};
    return sendCommand(payload, sizeof(payload), 0x61);
  }

  bool requestRestart() {
    const uint8_t payload[] = {0xA3, 0x00};
    return sendCommand(payload, sizeof(payload), 0xA3);
  }

  bool requestFactoryReset() {
    const uint8_t payload[] = {0xA2, 0x00};
    return sendCommand(payload, sizeof(payload), 0xA2);
  }

  bool requestStartEngineeringMode() {
    const uint8_t payload[] = {0x62, 0x00};
    return sendCommand(payload, sizeof(payload), 0x62);
  }

  bool requestEndEngineeringMode() {
    const uint8_t payload[] = {0x63, 0x00};
    return sendCommand(payload, sizeof(payload), 0x63);
  }

  bool setMaxValues(uint8_t movingGate, uint8_t stationaryGate, uint16_t inactivitySeconds) {
    const uint8_t payload[] = {
        0x60, 0x00,
        0x00, 0x00, movingGate, 0x00,
        0x00, 0x00,
        0x01, 0x00, stationaryGate, 0x00,
        0x00, 0x00,
        0x02, 0x00, static_cast<uint8_t>(inactivitySeconds & 0xFF), static_cast<uint8_t>(inactivitySeconds >> 8)};
    return sendCommand(payload, sizeof(payload), 0x60);
  }

  bool setGateSensitivity(uint8_t gate, uint8_t movingSensitivity, uint8_t stationarySensitivity) {
    const uint8_t payload[] = {
        0x64, 0x00,
        0x00, 0x00, gate, 0x00,
        0x00, 0x00,
        0x01, 0x00, movingSensitivity, 0x00,
        0x00, 0x00,
        0x02, 0x00, stationarySensitivity, 0x00};
    return sendCommand(payload, sizeof(payload), 0x64);
  }

  SensorProbe probe() const {
    SensorProbe info;
    info.kind = SensorKind::Motion;
    info.busType = SensorBusType::Uart;
    info.configured = serial_ != nullptr;
    info.supportsContinuousMode = true;
    info.supportsThresholdConfig = true;
    info.supportsDmaTransfer = dmaPlatformSupported() && busSupportsDma(SensorBusType::Uart);
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
    report.value = presenceDetected() ? detectedDistanceCm_ : 0;
    report.detail = targetType_;
    return report;
  }

 private:
  static constexpr uint8_t kDataHeader[4] = {0xF4, 0xF3, 0xF2, 0xF1};
  static constexpr uint8_t kDataFooter[4] = {0xF8, 0xF7, 0xF6, 0xF5};
  static constexpr uint8_t kCommandHeader[4] = {0xFD, 0xFC, 0xFB, 0xFA};
  static constexpr uint8_t kCommandFooter[4] = {0x04, 0x03, 0x02, 0x01};

  bool sendCommand(const uint8_t* payload, uint16_t length, uint8_t expectedAck) {
    if (serial_ == nullptr) {
      return false;
    }
    if (!enterConfigurationMode()) {
      return false;
    }

    writeCommandFrame(payload, length);
    pendingAck_ = expectedAck;
    ackMatched_ = false;
    const uint32_t startMs = millis();
    while (millis() - startMs < commandTimeoutMs_) {
      if (processIncomingFrames(1) && ackMatched_) {
        leaveConfigurationMode();
        return latestCommandSuccess_;
      }
      yield();
    }

    leaveConfigurationMode();
    return false;
  }

  bool enterConfigurationMode() {
    const uint8_t payload[] = {0xFF, 0x00, 0x01, 0x00};
    writeCommandFrame(payload, sizeof(payload));
    pendingAck_ = 0xFF;
    ackMatched_ = false;
    const uint32_t startMs = millis();
    while (millis() - startMs < commandTimeoutMs_) {
      if (processIncomingFrames(1) && ackMatched_) {
        return latestCommandSuccess_;
      }
      yield();
    }
    return false;
  }

  bool leaveConfigurationMode() {
    const uint8_t payload[] = {0xFE, 0x00};
    writeCommandFrame(payload, sizeof(payload));
    pendingAck_ = 0xFE;
    ackMatched_ = false;
    const uint32_t startMs = millis();
    while (millis() - startMs < commandTimeoutMs_) {
      if (processIncomingFrames(1) && ackMatched_) {
        return latestCommandSuccess_;
      }
      yield();
    }
    return false;
  }

  void writeCommandFrame(const uint8_t* payload, uint16_t length) {
    for (uint8_t value : kCommandHeader) {
      serial_->write(value);
    }
    serial_->write(static_cast<uint8_t>(length & 0xFF));
    serial_->write(static_cast<uint8_t>(length >> 8));
    for (uint16_t index = 0; index < length; ++index) {
      serial_->write(payload[index]);
    }
    for (uint8_t value : kCommandFooter) {
      serial_->write(value);
    }
  }

  bool processIncomingFrames(uint16_t waitMs) {
    if (serial_ == nullptr) {
      return false;
    }

    const uint32_t startMs = millis();
    bool processed = false;
    do {
      while (serial_->available() > 0) {
        const uint8_t value = static_cast<uint8_t>(serial_->read());
        processed = true;
        consumeByte(value);
      }
      if (waitMs == 0) {
        break;
      }
      yield();
    } while (millis() - startMs < waitMs);

    return processed;
  }

  void consumeByte(uint8_t value) {
    if (frameLength_ == 0) {
      if (value == kDataHeader[0] || value == kCommandHeader[0]) {
        frame_[0] = value;
        frameLength_ = 1;
        expectedFrameLength_ = 0;
      }
      return;
    }

    if (frameLength_ >= sizeof(frame_)) {
      frameLength_ = 0;
      expectedFrameLength_ = 0;
      return;
    }

    frame_[frameLength_++] = value;

    if (frameLength_ == 4) {
      const bool dataHeader = frame_[0] == kDataHeader[0] && frame_[1] == kDataHeader[1] && frame_[2] == kDataHeader[2] && frame_[3] == kDataHeader[3];
      const bool ackHeader = frame_[0] == kCommandHeader[0] && frame_[1] == kCommandHeader[1] && frame_[2] == kCommandHeader[2] && frame_[3] == kCommandHeader[3];
      if (!dataHeader && !ackHeader) {
        frameLength_ = 0;
        expectedFrameLength_ = 0;
      }
      return;
    }

    if (frameLength_ == 6) {
      expectedFrameLength_ = static_cast<uint16_t>(frame_[4] | (frame_[5] << 8)) + 10;
      if (expectedFrameLength_ > sizeof(frame_) || expectedFrameLength_ < 10) {
        frameLength_ = 0;
        expectedFrameLength_ = 0;
      }
      return;
    }

    if (expectedFrameLength_ != 0 && frameLength_ >= expectedFrameLength_) {
      if (validFooter()) {
        if (frame_[0] == kDataHeader[0]) {
          parseDataFrame();
        } else {
          parseCommandFrame();
        }
      }
      frameLength_ = 0;
      expectedFrameLength_ = 0;
    }
  }

  bool validFooter() const {
    if (frameLength_ < 10) {
      return false;
    }
    const uint8_t* footer = frame_[0] == kDataHeader[0] ? kDataFooter : kCommandFooter;
    return frame_[frameLength_ - 4] == footer[0] && frame_[frameLength_ - 3] == footer[1] &&
           frame_[frameLength_ - 2] == footer[2] && frame_[frameLength_ - 1] == footer[3];
  }

  void parseDataFrame() {
    const uint16_t payloadLength = static_cast<uint16_t>(frame_[4] | (frame_[5] << 8));
    if (frameLength_ != static_cast<uint16_t>(payloadLength + 10)) {
      return;
    }

    // HLK-LD2410S compact report: state @6 (0/1=no one, 2/3=someone), distance cm @7-8.
    if (frame_[6] != 0x02 && frame_[7] != 0xAA && payloadLength >= 3) {
      const uint8_t state = frame_[6];
      if (state <= 3) {
        const uint16_t distCm = static_cast<uint16_t>(frame_[7] | (frame_[8] << 8));
        targetType_ = state >= 2 ? 0x03 : 0;
        movingTargetDistanceCm_ = distCm;
        stationaryTargetDistanceCm_ = distCm;
        detectedDistanceCm_ = distCm;
        movingTargetEnergy_ = state >= 2 ? 64 : 0;
        stationaryTargetEnergy_ = movingTargetEnergy_;
        updateMotionReading();
        return;
      }
    }

    if (frame_[6] != 0x02 || frame_[7] != 0xAA || frame_[17] != 0x55 || frame_[18] != 0x00) {
      return;
    }

    targetType_ = frame_[8];
    movingTargetDistanceCm_ = static_cast<uint16_t>(frame_[9] | (frame_[10] << 8));
    movingTargetEnergy_ = frame_[11];
    stationaryTargetDistanceCm_ = static_cast<uint16_t>(frame_[12] | (frame_[13] << 8));
    stationaryTargetEnergy_ = frame_[14];
    detectedDistanceCm_ = static_cast<uint16_t>(frame_[15] | (frame_[16] << 8));
    updateMotionReading();
  }

  void updateMotionReading() {
    lastPacketMs_ = millis();
    MotionReading reading;
    reading.status = SensorStatus::Ok;
    reading.state = presenceDetected() ? DetectionState::Active : DetectionState::Inactive;
    reading.rose = !lastMotionReading_.detected() && presenceDetected();
    reading.fell = lastMotionReading_.detected() && !presenceDetected();
    reading.timestampMs = lastPacketMs_;
    reading.lastActiveMs = presenceDetected() ? lastPacketMs_ : lastMotionReading_.lastActiveMs;
    lastMotionReading_ = reading;
  }

  void parseCommandFrame() {
    latestAck_ = frame_[6];
    latestCommandSuccess_ = frame_[8] == 0x00 && frame_[9] == 0x00;
    if (latestCommandSuccess_) {
      lastPacketMs_ = millis();
    }

    const uint16_t payloadLength = static_cast<uint16_t>(frame_[4] | (frame_[5] << 8));
    if (payloadLength == 12 && latestAck_ == 0xA0 && latestCommandSuccess_) {
      firmwareMajorVersion_ = frame_[13];
      firmwareMinorVersion_ = frame_[12];
      firmwareBugfixVersion_ = static_cast<uint32_t>(frame_[14]) |
                               (static_cast<uint32_t>(frame_[15]) << 8) |
                               (static_cast<uint32_t>(frame_[16]) << 16) |
                               (static_cast<uint32_t>(frame_[17]) << 24);
    } else if (payloadLength == 28 && latestAck_ == 0x61 && latestCommandSuccess_) {
      maxGate_ = frame_[11];
      maxMovingGate_ = frame_[12];
      maxStationaryGate_ = frame_[13];
      for (uint8_t index = 0; index < 9; ++index) {
        motionSensitivity_[index] = frame_[14 + index];
        stationarySensitivity_[index] = frame_[23 + index];
      }
      idleTimeoutSeconds_ = static_cast<uint16_t>(frame_[32] | (frame_[33] << 8));
    }

    if (pendingAck_ != 0 && latestAck_ == pendingAck_) {
      ackMatched_ = true;
    }
  }

  Stream* serial_ = nullptr;
  uint16_t timeoutMs_ = 1500;
  uint16_t commandTimeoutMs_ = 200;
  uint32_t lastPacketMs_ = 0;
  uint8_t frame_[48] = {0};
  uint16_t frameLength_ = 0;
  uint16_t expectedFrameLength_ = 0;
  uint8_t pendingAck_ = 0;
  bool ackMatched_ = false;
  bool latestCommandSuccess_ = false;
  uint8_t latestAck_ = 0;

  uint8_t targetType_ = 0;
  uint16_t movingTargetDistanceCm_ = 0;
  uint8_t movingTargetEnergy_ = 0;
  uint16_t stationaryTargetDistanceCm_ = 0;
  uint8_t stationaryTargetEnergy_ = 0;
  uint16_t detectedDistanceCm_ = 0;
  MotionReading lastMotionReading_;

  uint8_t firmwareMajorVersion_ = 0;
  uint8_t firmwareMinorVersion_ = 0;
  uint32_t firmwareBugfixVersion_ = 0;
  uint8_t maxGate_ = 0;
  uint8_t maxMovingGate_ = 0;
  uint8_t maxStationaryGate_ = 0;
  uint16_t idleTimeoutSeconds_ = 0;
  uint8_t motionSensitivity_[9] = {0};
  uint8_t stationarySensitivity_[9] = {0};
};

}  // namespace DiFinders
