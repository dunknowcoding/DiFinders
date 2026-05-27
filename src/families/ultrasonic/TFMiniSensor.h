#pragma once

#include <Arduino.h>
#include <Stream.h>

#include "../../core/DiFindersTypes.h"

namespace DiFinders {

class TFMiniSensor {
 public:
  TFMiniSensor() = default;
  explicit TFMiniSensor(Stream& serial) { begin(serial); }

  void begin(Stream& serial) {
    serial_ = &serial;
  }

  bool ready() const {
    return serial_ != nullptr;
  }

  void setTimeout(uint16_t timeoutMs) {
    timeoutMs_ = timeoutMs;
  }

  void setMinStrength(uint16_t minStrength) {
    minStrength_ = minStrength;
  }

  RangeReading read() {
    RangeReading reading;
    reading.timestampMs = millis();
    if (!ready()) {
      reading.status = SensorStatus::NotReady;
      lastReading_ = reading;
      return reading;
    }

    uint8_t frame[9] = {0};
    if (!readFrame(frame)) {
      reading.status = SensorStatus::Timeout;
      lastReading_ = reading;
      return reading;
    }

    if (!validFrame(frame)) {
      reading.status = SensorStatus::Error;
      lastReading_ = reading;
      return reading;
    }

    reading.distanceMm = static_cast<uint16_t>((static_cast<uint16_t>(frame[3]) << 8) | frame[2]);
    reading.rawValue = reading.distanceMm;
    lastStrength_ = static_cast<uint16_t>((static_cast<uint16_t>(frame[5]) << 8) | frame[4]);
    uint16_t rawTemperature = static_cast<uint16_t>((static_cast<uint16_t>(frame[7]) << 8) | frame[6]);
    lastTemperatureC_ = static_cast<int16_t>((rawTemperature / 8U) - 256U);

    if (reading.distanceMm == 0) {
      reading.status = SensorStatus::OutOfRange;
      lastReading_ = reading;
      return reading;
    }

    reading.status = minStrength_ > 0 && lastStrength_ < minStrength_ ? SensorStatus::OutOfRange : SensorStatus::Ok;
    lastReading_ = reading;
    return reading;
  }

  uint16_t millimeters() {
    return read().distanceMm;
  }

  uint16_t strength() {
    read();
    return lastStrength_;
  }

  int16_t temperatureC() {
    read();
    return lastTemperatureC_;
  }

  uint16_t lastStrength() const {
    return lastStrength_;
  }

  int16_t lastTemperatureC() const {
    return lastTemperatureC_;
  }

  SensorProbe probe() const {
    SensorProbe info;
    info.kind = SensorKind::Range;
    info.busType = SensorBusType::Uart;
    info.configured = ready();
    info.supportsTemperatureReading = true;
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
    report.detail = static_cast<uint8_t>(lastStrength_ > 0 ? 1 : 0);
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

 private:
  bool readFrame(uint8_t* frame) {
    uint8_t temp[9] = {0};
    uint8_t index = 0;
    uint32_t start = millis();
    bool haveFrame = false;
    while (true) {
      while (serial_->available() > 0) {
        uint8_t value = static_cast<uint8_t>(serial_->read());

        if (index == 0) {
          if (value != 0x59) {
            continue;
          }
          temp[0] = value;
          index = 1;
          continue;
        }

        if (index == 1) {
          if (value != 0x59) {
            index = 0;
            continue;
          }
          temp[1] = value;
          index = 2;
          continue;
        }

        temp[index] = value;
        ++index;
        if (index < 9) {
          continue;
        }

        if (validFrame(temp)) {
          for (uint8_t frameIndex = 0; frameIndex < 9; ++frameIndex) {
            frame[frameIndex] = temp[frameIndex];
          }
          haveFrame = true;
          lastFrameTimestampMs_ = millis();
          if (serial_->available() > 0) {
            ++droppedFrameCount_;
          }
        } else {
          ++frameErrorCount_;
        }

        if (value == 0x59) {
          temp[0] = 0x59;
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
    }
  }

  bool readByte(uint8_t& value, uint32_t start) {
    while (serial_->available() < 1) {
      if (expired(start)) {
        return false;
      }
    }
    value = static_cast<uint8_t>(serial_->read());
    return true;
  }

  bool validFrame(const uint8_t* frame) const {
    if (frame[0] != 0x59 || frame[1] != 0x59) {
      return false;
    }
    uint8_t checksum = 0;
    for (uint8_t index = 0; index < 8; ++index) {
      checksum = static_cast<uint8_t>(checksum + frame[index]);
    }
    return checksum == frame[8];
  }

  bool expired(uint32_t start) const {
    return timeoutMs_ > 0 && (millis() - start) > timeoutMs_;
  }

  Stream* serial_ = nullptr;
  uint16_t timeoutMs_ = 150;
  uint16_t minStrength_ = 0;
  uint16_t lastStrength_ = 0;
  uint16_t frameErrorCount_ = 0;
  uint16_t droppedFrameCount_ = 0;
  uint32_t lastFrameTimestampMs_ = 0;
  int16_t lastTemperatureC_ = -1000;
  RangeReading lastReading_;
};

}  // namespace DiFinders