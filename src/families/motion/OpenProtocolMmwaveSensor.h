#pragma once

#include <Arduino.h>
#include <Stream.h>

#include "../../core/DiFindersTypes.h"
#include "../../core/DiFindersDiagnostics.h"

namespace DiFinders {

class OpenProtocolMmwaveSensor {
 public:
  OpenProtocolMmwaveSensor() = default;
  explicit OpenProtocolMmwaveSensor(Stream& serial) {
    begin(serial);
  }

  void begin(Stream& serial) {
    serial_ = &serial;
  }

  void setTimeout(uint16_t timeoutMs) {
    timeoutMs_ = timeoutMs;
  }

  uint32_t defaultBaudRate() const {
    return 115200UL;
  }

  bool read() {
    if (serial_ == nullptr) {
      return false;
    }

    bool processed = false;
    while (serial_->available() > 0) {
      const uint8_t value = static_cast<uint8_t>(serial_->read());
      processed = true;
      consumeByte(value);
    }
    return processed;
  }

  bool sendRawCommand(const uint8_t* bytes, size_t length) {
    if (serial_ == nullptr || bytes == nullptr || length == 0) {
      return false;
    }
    for (size_t index = 0; index < length; ++index) {
      serial_->write(bytes[index]);
    }
    return true;
  }

  bool enableUnderlyingMessage(bool enabled) {
    const uint8_t frame[] = {0x53, 0x59, 0x08, 0x00, 0x00, 0x01, static_cast<uint8_t>(enabled ? 0x01 : 0x00),
                             static_cast<uint8_t>(enabled ? 0xB6 : 0xB5), 0x54, 0x43};
    return sendRawCommand(frame, sizeof(frame));
  }

  bool reset() {
    const uint8_t frame[] = {0x53, 0x59, 0x01, 0x02, 0x00, 0x01, 0x0F, 0xBF, 0x54, 0x43};
    return sendRawCommand(frame, sizeof(frame));
  }

  bool queryDeviceId() {
    const uint8_t frame[] = {0x53, 0x59, 0x02, 0xA1, 0x00, 0x01, 0x0F, 0x60, 0x54, 0x43};
    return sendRawCommand(frame, sizeof(frame));
  }

  bool queryFirmwareVersion() {
    const uint8_t frame[] = {0x53, 0x59, 0x02, 0xA4, 0x00, 0x01, 0x0F, 0x62, 0x54, 0x43};
    return sendRawCommand(frame, sizeof(frame));
  }

  bool presenceDetected() const {
    return presenceState_ == 0x01 || movementState_ == 0x02;
  }

  bool movingDetected() const {
    return movementState_ == 0x02;
  }

  bool stationaryDetected() const {
    return movementState_ == 0x01;
  }

  uint8_t bodySign() const {
    return bodySign_;
  }

  uint8_t directionState() const {
    return directionState_;
  }

  uint8_t lastFrameType() const {
    return lastFrameType_;
  }

  uint8_t lastFrameSubtype() const {
    return lastFrameSubtype_;
  }

  uint8_t lastFrameLength() const {
    return lastFrameLength_;
  }

  const uint8_t* lastFrame() const {
    return lastFrame_;
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
    report.status = lastFrameTimestampMs_ != 0 && millis() - lastFrameTimestampMs_ <= timeoutMs_ ? SensorStatus::Ok : SensorStatus::Timeout;
    report.passed = report.status == SensorStatus::Ok;
    report.value = bodySign_;
    report.detail = lastFrameType_;
    return report;
  }

  uint8_t lastBusError() const { return 0; }

  void flushRx() {
    while (serial_ != nullptr && serial_->available() > 0) {
      serial_->read();
    }
  }

  SensorHealth health() const {
    return makeHealth(serial_ != nullptr, probe().busType,
                      lastMotionReading_.status, 0, bodySign_, 0);
  }

 private:
  void consumeByte(uint8_t value) {
    if (frameIndex_ == 0) {
      if (value == 0x53) {
        frame_[frameIndex_++] = value;
      }
      return;
    }

    if (frameIndex_ == 1) {
      if (value == 0x59) {
        frame_[frameIndex_++] = value;
      } else {
        frameIndex_ = 0;
      }
      return;
    }

    if (frameIndex_ >= sizeof(frame_)) {
      frameIndex_ = 0;
      expectedFrameLength_ = 0;
      return;
    }

    frame_[frameIndex_++] = value;
    if (frameIndex_ == 3) {
      expectedFrameLength_ = static_cast<uint8_t>(frame_[2] + 8);
      if (expectedFrameLength_ > sizeof(frame_) || expectedFrameLength_ < 8) {
        frameIndex_ = 0;
        expectedFrameLength_ = 0;
      }
      return;
    }

    if (expectedFrameLength_ != 0 && frameIndex_ >= expectedFrameLength_) {
      if (frame_[expectedFrameLength_ - 2] == 0x54 && frame_[expectedFrameLength_ - 1] == 0x43 && checksumValid()) {
        handleFrame();
      }
      frameIndex_ = 0;
      expectedFrameLength_ = 0;
    }
  }

  bool checksumValid() const {
    uint16_t sum = 0;
    for (uint8_t index = 0; index < expectedFrameLength_ - 3; ++index) {
      sum += frame_[index];
    }
    return static_cast<uint8_t>(sum & 0xFF) == frame_[expectedFrameLength_ - 3];
  }

  void handleFrame() {
    lastFrameLength_ = expectedFrameLength_;
    for (uint8_t index = 0; index < lastFrameLength_; ++index) {
      lastFrame_[index] = frame_[index];
    }
    lastFrameTimestampMs_ = millis();

    if (lastFrameLength_ < 6) {
      return;
    }

    lastFrameType_ = lastFrame_[3];
    lastFrameSubtype_ = lastFrame_[4];
    if (lastFrameType_ == 0x80 && lastFrameLength_ >= 8) {
      const uint8_t value = lastFrame_[5];
      if (lastFrameSubtype_ == 0x01) {
        presenceState_ = value;
      } else if (lastFrameSubtype_ == 0x02) {
        movementState_ = value;
      } else if (lastFrameSubtype_ == 0x03) {
        bodySign_ = value;
      } else if (lastFrameSubtype_ == 0x0B) {
        directionState_ = value;
      }
    }

    MotionReading reading;
    reading.status = SensorStatus::Ok;
    reading.state = presenceDetected() ? DetectionState::Active : DetectionState::Inactive;
    reading.rose = presenceDetected() && !lastMotionReading_.detected();
    reading.fell = !presenceDetected() && lastMotionReading_.detected();
    reading.timestampMs = lastFrameTimestampMs_;
    reading.lastActiveMs = presenceDetected() ? lastFrameTimestampMs_ : lastMotionReading_.lastActiveMs;
    lastMotionReading_ = reading;
  }

  Stream* serial_ = nullptr;
  uint16_t timeoutMs_ = 1200;
  uint8_t frame_[48] = {0};
  uint8_t frameIndex_ = 0;
  uint8_t expectedFrameLength_ = 0;
  uint8_t lastFrame_[48] = {0};
  uint8_t lastFrameLength_ = 0;
  uint8_t lastFrameType_ = 0;
  uint8_t lastFrameSubtype_ = 0;
  uint8_t presenceState_ = 0;
  uint8_t movementState_ = 0;
  uint8_t bodySign_ = 0;
  uint8_t directionState_ = 0;
  uint32_t lastFrameTimestampMs_ = 0;
  MotionReading lastMotionReading_;
};

}  // namespace DiFinders
