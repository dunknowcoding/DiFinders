#pragma once

#include <Arduino.h>
#include <Stream.h>

#include "../../core/DiFindersTypes.h"
#include "../../core/DiFindersDiagnostics.h"

namespace DiFinders {

class LD2411RadarSensor {
 public:
  LD2411RadarSensor() = default;
  explicit LD2411RadarSensor(Stream& serial) {
    begin(serial);
  }

  void begin(Stream& serial, bool waitForRadar = false) {
    serial_ = &serial;
    frameLength_ = 0;
    expectedFrameLength_ = 0;
    pendingAck_ = 0;
    ackMatched_ = false;
    latestCommandSuccess_ = false;
    latestAck_ = 0;
    if (waitForRadar) {
      isConnected();
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

  bool movingTargetDetected() const {
    return (targetType_ & 0x01) != 0;
  }

  bool stationaryTargetDetected() const {
    return (targetType_ & 0x02) != 0;
  }

  uint8_t targetType() const {
    return targetType_;
  }

  uint16_t detectedDistanceCm() const {
    return detectedDistanceCm_;
  }

  bool bluetoothEnabled() const {
    return bluetoothEnabled_;
  }

  const MotionReading& lastReading() const {
    return lastMotionReading_;
  }

  MotionReading motion() const {
    return lastMotionReading_;
  }

  bool requestRestart() {
    const uint8_t payload[] = {0xA3, 0x00};
    return sendCommand(payload, sizeof(payload), 0xA3);
  }

  bool setBluetoothEnabled(bool enabled) {
    const uint8_t payload[] = {0xA4, 0x00, static_cast<uint8_t>(enabled ? 0x01 : 0x00), 0x00};
    const bool ok = sendCommand(payload, sizeof(payload), 0xA4);
    if (ok) {
      bluetoothEnabled_ = enabled;
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
    report.value = presenceDetected() ? detectedDistanceCm_ : 0;
    report.detail = targetType_;
    return report;
  }

  uint8_t lastBusError() const { return 0; }

  uint32_t faultCount() const { return 0; }

  SensorHealth health() const {
    const bool link = serial_ != nullptr && (millis() - lastPacketMs_ <= timeoutMs_);
    const SensorStatus st = link ? SensorStatus::Ok : SensorStatus::Timeout;
    return makeHealth(serial_ != nullptr, probe().busType, st, 0, detectedDistanceCm_, faultCount());
  }

 private:
  static constexpr uint8_t kDataHeader[2] = {0xAA, 0xAA};
  static constexpr uint8_t kDataFooter[2] = {0x55, 0x55};
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
        processed = true;
        consumeByte(static_cast<uint8_t>(serial_->read()));
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

    if (frame_[0] == kDataHeader[0]) {
      if (frameLength_ == 2) {
        if (frame_[1] != kDataHeader[1]) {
          frameLength_ = 0;
        }
        return;
      }
      if (expectedFrameLength_ == 0) {
        expectedFrameLength_ = 7;
      }
      if (frameLength_ >= expectedFrameLength_) {
        if (frame_[5] == kDataFooter[0] && frame_[6] == kDataFooter[1]) {
          parseDataFrame();
        }
        frameLength_ = 0;
        expectedFrameLength_ = 0;
      }
      return;
    }

    if (frameLength_ == 4) {
      const bool ackHeader = frame_[0] == kCommandHeader[0] && frame_[1] == kCommandHeader[1] &&
                             frame_[2] == kCommandHeader[2] && frame_[3] == kCommandHeader[3];
      if (!ackHeader) {
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
      if (validCommandFooter()) {
        parseCommandFrame();
      }
      frameLength_ = 0;
      expectedFrameLength_ = 0;
    }
  }

  bool validCommandFooter() const {
    if (frameLength_ < 10) {
      return false;
    }
    return frame_[frameLength_ - 4] == kCommandFooter[0] && frame_[frameLength_ - 3] == kCommandFooter[1] &&
           frame_[frameLength_ - 2] == kCommandFooter[2] && frame_[frameLength_ - 1] == kCommandFooter[3];
  }

  void parseDataFrame() {
    targetType_ = frame_[2];
    detectedDistanceCm_ = static_cast<uint16_t>(frame_[3] | (frame_[4] << 8));
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
    const uint16_t payloadLength = static_cast<uint16_t>(frame_[4] | (frame_[5] << 8));
    latestCommandSuccess_ = payloadLength >= 4 && frame_[8] == 0x00 && frame_[9] == 0x00;

    if (latestCommandSuccess_) {
      lastPacketMs_ = millis();
    }

    if (pendingAck_ != 0 && latestAck_ == pendingAck_) {
      ackMatched_ = true;
    }
  }

  Stream* serial_ = nullptr;
  uint16_t timeoutMs_ = 1500;
  uint16_t commandTimeoutMs_ = 250;
  uint32_t lastPacketMs_ = 0;
  uint8_t frame_[40] = {0};
  uint16_t frameLength_ = 0;
  uint16_t expectedFrameLength_ = 0;
  uint8_t pendingAck_ = 0;
  bool ackMatched_ = false;
  bool latestCommandSuccess_ = false;
  uint8_t latestAck_ = 0;

  uint8_t targetType_ = 0;
  uint16_t detectedDistanceCm_ = 0;
  bool bluetoothEnabled_ = false;
  MotionReading lastMotionReading_;
};

}  // namespace DiFinders