#pragma once

#include <Arduino.h>
#include <Stream.h>

#include "../../core/DiFindersTypes.h"
#include "../../core/DiFindersDiagnostics.h"

namespace DiFinders {

class A02YYUWSensor {
 public:
  A02YYUWSensor() = default;
  explicit A02YYUWSensor(Stream& serial) { begin(serial); }

  void begin(Stream& serial) {
    serial_ = &serial;
  }

  bool ready() const {
    return serial_ != nullptr;
  }

  void setTimeout(uint16_t timeoutMs) {
    timeoutMs_ = timeoutMs;
  }

  RangeReading read() {
    RangeReading reading;
    reading.timestampMs = millis();
    if (!ready()) {
      reading.status = SensorStatus::NotReady;
      lastReading_ = reading;
      return reading;
    }

    uint8_t frame[4] = {0, 0, 0, 0};
    if (!readFrame(frame)) {
      reading.status = SensorStatus::Timeout;
      lastReading_ = reading;
      return reading;
    }

    uint8_t checksum = static_cast<uint8_t>(frame[0] + frame[1] + frame[2]);
    if (frame[0] != 0xFF || checksum != frame[3]) {
      reading.status = SensorStatus::Error;
      reading.rawValue = static_cast<uint16_t>((frame[1] << 8) | frame[2]);
      lastReading_ = reading;
      return reading;
    }

    reading.distanceMm = static_cast<uint16_t>((frame[1] << 8) | frame[2]);
    reading.rawValue = reading.distanceMm;
    reading.status = reading.distanceMm == 0 ? SensorStatus::OutOfRange : SensorStatus::Ok;
    lastReading_ = reading;
    return reading;
  }

  uint16_t millimeters() {
    return read().distanceMm;
  }

  SensorProbe probe() const {
    SensorProbe info;
    info.kind = SensorKind::Range;
    info.busType = SensorBusType::Uart;
    info.configured = ready();
    return info;
  }

  SelfTestReport selfTest() {
    SelfTestReport report;
    report.passed = ready();
    report.status = ready() ? SensorStatus::Ok : SensorStatus::NotReady;
    report.value = lastReading_.distanceMm;
    return report;
  }

  const RangeReading& lastReading() const {
    return lastReading_;
  }

  uint32_t lastFrameTimestampMs() const {
    return lastFrameTimestampMs_;
  }

  uint16_t frameErrorCount() const {
    return frameErrorCount_;
  }

  uint16_t droppedFrameCount() const {
    return droppedFrameCount_;

  }

  uint8_t lastBusError() const { return 0; }

  void flushRx() {
    while (serial_ != nullptr && serial_->available() > 0) {
      serial_->read();
    }
  }

  uint32_t faultCount() const { return frameErrorCount_ + droppedFrameCount_; }

  SensorHealth health() const {
    return makeHealth(ready(), probe().busType, lastReading_.status, 0, lastReading_.distanceMm, faultCount());
  }
 private:
  bool readFrame(uint8_t* frame) {
    uint8_t temp[4] = {0, 0, 0, 0};
    uint8_t index = 0;
    uint32_t start = millis();
    bool haveFrame = false;
    while (true) {
      while (serial_->available() > 0) {
        uint8_t value = static_cast<uint8_t>(serial_->read());

        if (index == 0) {
          if (value != 0xFF) {
            continue;
          }
          temp[0] = value;
          index = 1;
          continue;
        }

        temp[index] = value;
        ++index;

        if (index < 4) {
          continue;
        }

        uint8_t checksum = static_cast<uint8_t>(temp[0] + temp[1] + temp[2]);
        if (checksum == temp[3]) {
          frame[0] = temp[0];
          frame[1] = temp[1];
          frame[2] = temp[2];
          frame[3] = temp[3];
          haveFrame = true;
          lastFrameTimestampMs_ = millis();
          if (serial_->available() > 0) {
            ++droppedFrameCount_;
          }
        } else {
          ++frameErrorCount_;
        }

        if (value == 0xFF) {
          temp[0] = 0xFF;
          index = 1;
        } else {
          index = 0;
        }
      }

      if (haveFrame) {
        return true;
      }
      if (expired(start)) {
        return false;
      }
      delay(1);
    }
  }

  bool readByte(uint8_t& value, uint32_t start) {
    while (serial_->available() < 1) {
      if (expired(start)) {
        return false;
      }
      delay(1);
    }
    value = static_cast<uint8_t>(serial_->read());
    return true;
  }

  bool expired(uint32_t start) const {
    return timeoutMs_ > 0 && (millis() - start) > timeoutMs_;
  }

  Stream* serial_ = nullptr;
  uint16_t timeoutMs_ = 150;
  uint16_t frameErrorCount_ = 0;
  uint16_t droppedFrameCount_ = 0;
  uint32_t lastFrameTimestampMs_ = 0;
  RangeReading lastReading_;
};

}  // namespace DiFinders
