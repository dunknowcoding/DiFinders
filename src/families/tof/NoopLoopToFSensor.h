#pragma once

#include <Arduino.h>
#include <Stream.h>

#include "../../core/DiFindersDebug.h"
#include "../../core/DiFindersDiagnostics.h"
#include "../../core/DiFindersTypes.h"

namespace DiFinders {

class NoopLoopToFSensor {
 public:
  NoopLoopToFSensor() = default;
  explicit NoopLoopToFSensor(Stream& serial) { begin(serial); }

  void begin(Stream& serial) {
    serial_ = &serial;
  }

  bool ready() const {
    return serial_ != nullptr;
  }

  void setTimeout(uint16_t timeoutMs) {
    timeoutMs_ = timeoutMs;
  }

  void setFrameHeaderSecondByte(uint8_t headerByte) {
    frameHeader1_ = headerByte;
  }

  uint8_t frameHeaderSecondByte() const {
    return frameHeader1_;
  }

  void setOkStatusByte(uint8_t statusByte) {
    okStatusByte_ = statusByte;
  }

  void setOutOfRangeStatusByte(uint8_t statusByte) {
    outOfRangeStatusByte_ = statusByte;
  }

  void setDistanceCalibration(int16_t offsetMm, uint16_t scalePermille = 1000) {
    offsetMm_ = offsetMm;
    scalePermille_ = scalePermille == 0 ? 1 : scalePermille;
  }

  void setDefaultBaudRate(uint32_t baudRate) {
    defaultBaudRate_ = baudRate;
  }

  uint32_t defaultBaudRate() const {
    return defaultBaudRate_;
  }

  uint16_t timeout() const {
    return timeoutMs_;
  }

  void setMinSignalStrength(uint16_t minSignalStrength) {
    minSignalStrength_ = minSignalStrength;
  }

  uint16_t minSignalStrength() const {
    return minSignalStrength_;
  }

  RangeReading read() {
    RangeReading reading;
    reading.timestampMs = millis();
    if (!ready()) {
      reading.status = SensorStatus::NotReady;
      lastReading_ = reading;
      return reading;
    }

    uint8_t frame[kFrameLength] = {0};
    if (!readFrame(frame)) {
      reading.status = SensorStatus::Timeout;
      lastReading_ = reading;
      return reading;
    }

    const uint8_t checksum = checksum8(frame, kFrameLength - 1);
    if (checksum != frame[kFrameLength - 1]) {
      ++frameErrorCount_;
      reading.status = SensorStatus::Error;
      lastReading_ = reading;
      return reading;
    }

    uint32_t distanceMm = static_cast<uint32_t>(frame[8]) |
                          (static_cast<uint32_t>(frame[9]) << 8) |
                          (static_cast<uint32_t>(frame[10]) << 16);
    lastStatusByte_ = frame[11];
    lastSignalStrength_ = static_cast<uint16_t>(frame[12]) |
                          (static_cast<uint16_t>(frame[13]) << 8);

    if (scalePermille_ != 1000) {
      distanceMm = (distanceMm * scalePermille_) / 1000UL;
    }
    if (offsetMm_ != 0) {
      if (offsetMm_ > 0) {
        distanceMm += static_cast<uint32_t>(offsetMm_);
      } else {
        const uint32_t magnitude = static_cast<uint32_t>(-offsetMm_);
        distanceMm = distanceMm > magnitude ? distanceMm - magnitude : 0;
      }
    }

    reading.distanceMm = static_cast<uint16_t>(distanceMm > 65535UL ? 65535UL : distanceMm);
    reading.rawValue = reading.distanceMm;

    if (lastStatusByte_ == okStatusByte_) {
      reading.status = minSignalStrength_ > 0 && lastSignalStrength_ < minSignalStrength_
                           ? SensorStatus::OutOfRange
                           : SensorStatus::Ok;
    } else if (lastStatusByte_ == outOfRangeStatusByte_) {
      reading.status = SensorStatus::OutOfRange;
    } else {
      reading.status = SensorStatus::Error;
    }

    lastReading_ = reading;
    return reading;
  }

  uint16_t millimeters() {
    return read().distanceMm;
  }

  uint16_t signalStrength() {
    read();
    return lastSignalStrength_;
  }

  uint16_t lastSignalStrength() const {
    return lastSignalStrength_;
  }

  uint8_t statusByte() const {
    return lastStatusByte_;
  }

  const char* lastStatusText() const {
    switch (lastStatusByte_) {
      case 0x00:
        return "ok";
      case 0xFF:
        return "out_of_range";
      default:
        return "device_error";
    }
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

  SensorProbe probe() const {
    SensorProbe info;
    info.kind = SensorKind::Range;
    info.busType = SensorBusType::Uart;
    info.configured = ready();
    info.supportsContinuousMode = true;
    return info;
  }

  SelfTestReport selfTest() {
    SelfTestReport report;
    report.passed = ready();
    report.status = ready() ? lastReading_.status : SensorStatus::NotReady;
    if (!ready()) {
      report.status = SensorStatus::NotReady;
      return report;
    }
    report.passed = lastReading_.status == SensorStatus::Ok ||
                    lastReading_.status == SensorStatus::OutOfRange ||
                    lastReading_.status == SensorStatus::NotReady;
    report.value = lastReading_.distanceMm;
    report.detail = lastStatusByte_;
    return report;
  }

  const RangeReading& lastReading() const {
    return lastReading_;
  }

  uint8_t lastBusError() const { return 0; }

  void flushRx() {
    while (serial_ != nullptr && serial_->available() > 0) {
      serial_->read();
    }
  }

  SensorHealth health() const {
    return makeHealth(ready(), probe().busType, lastReading_.status, 0, lastReading_.distanceMm,
                      frameErrorCount_ + droppedFrameCount_);
  }

 private:
  static constexpr uint8_t kFrameLength = 16;
  static constexpr uint8_t kFrameHeader0 = 0x57;
  static constexpr uint8_t kFrameHeader1 = 0x00;

  static uint8_t checksum8(const uint8_t* bytes, uint8_t length) {
    uint8_t sum = 0;
    for (uint8_t index = 0; index < length; ++index) {
      sum = static_cast<uint8_t>(sum + bytes[index]);
    }
    return sum;
  }

  bool expired(uint32_t start) const {
    return timeoutMs_ > 0 && (millis() - start) > timeoutMs_;
  }

  bool readFrame(uint8_t* frame) {
    uint8_t temp[kFrameLength] = {0};
    uint8_t index = 0;
    uint32_t start = millis();

    while (true) {
      while (serial_->available() > 0) {
        const uint8_t value = static_cast<uint8_t>(serial_->read());

        if (index == 0) {
          if (value != kFrameHeader0) {
            continue;
          }
          temp[index++] = value;
          continue;
        }

        if (index == 1) {
          if (value != frameHeader1_) {
            index = 0;
            if (value == kFrameHeader0) {
              temp[index++] = value;
            }
            continue;
          }
          temp[index++] = value;
          continue;
        }

        temp[index++] = value;
        if (index < kFrameLength) {
          continue;
        }

        for (uint8_t frameIndex = 0; frameIndex < kFrameLength; ++frameIndex) {
          frame[frameIndex] = temp[frameIndex];
        }
        lastFrameTimestampMs_ = millis();
        if (serial_->available() > 0) {
          ++droppedFrameCount_;
        }
        return true;
      }

      if (expired(start)) {
        return false;
      }
    }
  }

  Stream* serial_ = nullptr;
  uint16_t timeoutMs_ = 150;
  uint16_t minSignalStrength_ = 0;
  uint16_t lastSignalStrength_ = 0;
  uint16_t frameErrorCount_ = 0;
  uint16_t droppedFrameCount_ = 0;
  uint32_t lastFrameTimestampMs_ = 0;
  uint32_t defaultBaudRate_ = 115200;
  uint16_t scalePermille_ = 1000;
  uint8_t lastStatusByte_ = 0xFF;
  uint8_t frameHeader1_ = kFrameHeader1;
  uint8_t okStatusByte_ = 0x00;
  uint8_t outOfRangeStatusByte_ = 0xFF;
  int16_t offsetMm_ = 0;
  RangeReading lastReading_;
};

}  // namespace DiFinders