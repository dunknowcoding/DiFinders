#pragma once

#include <Arduino.h>
#include <ctype.h>
#include <stdlib.h>

#include "../../core/DiFindersDiagnostics.h"
#include "../../core/DiFindersTypes.h"

namespace DiFinders {

class AsciiUartRangeSensor {
 public:
  AsciiUartRangeSensor() = default;

  explicit AsciiUartRangeSensor(Stream& stream) {
    begin(stream);
  }

  void begin(Stream& stream) {
    stream_ = &stream;
  }

  void setTimeout(uint16_t timeoutMs) {
    timeoutMs_ = timeoutMs;
  }

  void setDefaultBaudRate(uint32_t baudRate) {
    defaultBaudRate_ = baudRate;
  }

  uint32_t defaultBaudRate() const {
    return defaultBaudRate_;
  }

  void setUnitMillimeters(float scale) {
    unitToMillimeters_ = scale <= 0.0f ? 1.0f : scale;
  }

  void setFrameDelimiters(char startChar, char endChar) {
    startChar_ = startChar;
    endChar_ = endChar;
  }

  void setPrefix(const String& prefix) {
    prefix_ = prefix;
  }

  void allowSignedValues(bool enabled) {
    allowSignedValues_ = enabled;
  }

  bool read() {
    if (stream_ == nullptr) {
      lastReading_ = RangeReading();
      lastReading_.status = SensorStatus::NotReady;
      lastReading_.timestampMs = millis();
      return false;
    }

    char buffer[32];
    size_t length = 0;
    const uint32_t startMs = millis();
    bool collecting = startChar_ == '\0';

    while (millis() - startMs < timeoutMs_) {
      while (stream_->available() > 0) {
        const char raw = static_cast<char>(stream_->read());
        if (!collecting) {
          if (raw == startChar_) {
            collecting = true;
            length = 0;
          }
          continue;
        }

        if (endChar_ != '\0' && raw == endChar_) {
          buffer[length] = '\0';
          return parseBuffer(buffer);
        }

        if (endChar_ == '\n' && raw == '\r') {
          continue;
        }

        if (raw == '\n' && endChar_ == '\0') {
          buffer[length] = '\0';
          return parseBuffer(buffer);
        }

        if (length + 1 < sizeof(buffer)) {
          buffer[length++] = raw;
        } else {
          length = 0;
          collecting = startChar_ == '\0';
          ++frameErrorCount_;
          break;
        }
      }
    }

    lastReading_ = RangeReading();
    lastReading_.status = SensorStatus::Timeout;
    lastReading_.timestampMs = millis();
    return false;
  }

  const RangeReading& lastReading() const {
    return lastReading_;
  }

  uint16_t millimeters() const {
    return lastReading_.distanceMm;
  }

  uint16_t frameErrorCount() const {
    return frameErrorCount_;
  }

  SensorProbe probe() const {
    SensorProbe info;
    info.kind = SensorKind::Range;
    info.busType = SensorBusType::Uart;
    info.configured = stream_ != nullptr;
    info.supportsContinuousMode = true;
    return info;
  }

  SelfTestReport selfTest() const {
    SelfTestReport report;
    report.passed = stream_ != nullptr;
    report.status = report.passed ? lastReading_.status : SensorStatus::NotReady;
    report.value = lastReading_.distanceMm;
    report.detail = static_cast<uint8_t>(frameErrorCount_ > 0 ? 1 : 0);
    return report;
  }

  bool ready() const {
    return stream_ != nullptr;
  }

  uint8_t lastBusError() const { return 0; }

  void flushRx() {
    while (stream_ != nullptr && stream_->available() > 0) {
      stream_->read();
    }
  }

  SensorHealth health() const {
    return makeHealth(ready(), probe().busType, lastReading_.status, 0, lastReading_.distanceMm, frameErrorCount_);
  }

 private:
  bool parseBuffer(char* buffer) {
    String line(buffer);
    line.trim();
    if (line.length() == 0) {
      lastReading_ = RangeReading();
      lastReading_.status = SensorStatus::Error;
      lastReading_.timestampMs = millis();
      ++frameErrorCount_;
      return false;
    }

    if (prefix_.length() > 0) {
      if (!line.startsWith(prefix_)) {
        lastReading_ = RangeReading();
        lastReading_.status = SensorStatus::Error;
        lastReading_.timestampMs = millis();
        ++frameErrorCount_;
        return false;
      }
      line.remove(0, prefix_.length());
      line.trim();
    }

    int startIndex = -1;
    for (uint16_t index = 0; index < line.length(); ++index) {
      const char value = line.charAt(index);
      const bool isNumeric = isDigit(value) || value == '.' || value == ',' ||
                             (allowSignedValues_ && index == 0 && value == '-');
      if (isNumeric) {
        startIndex = index;
        break;
      }
    }

    if (startIndex < 0) {
      lastReading_ = RangeReading();
      lastReading_.status = SensorStatus::Error;
      lastReading_.timestampMs = millis();
      ++frameErrorCount_;
      return false;
    }

    String numeric = line.substring(startIndex);
    numeric.replace(",", ".");

    char temp[20];
    numeric.toCharArray(temp, sizeof(temp));
    char* endPtr = nullptr;
    const float value = static_cast<float>(strtod(temp, &endPtr));
    if (endPtr == temp) {
      lastReading_ = RangeReading();
      lastReading_.status = SensorStatus::Error;
      lastReading_.timestampMs = millis();
      ++frameErrorCount_;
      return false;
    }

    float millimeters = value * unitToMillimeters_;
    if (millimeters < 0.0f) {
      millimeters = 0.0f;
    }

    lastReading_ = RangeReading();
    lastReading_.distanceMm = static_cast<uint16_t>(millimeters > 65535.0f ? 65535.0f : millimeters);
    lastReading_.rawValue = lastReading_.distanceMm;
    lastReading_.timestampMs = millis();
    lastReading_.status = lastReading_.distanceMm == 0 ? SensorStatus::OutOfRange : SensorStatus::Ok;
    return lastReading_.status == SensorStatus::Ok;
  }

  Stream* stream_ = nullptr;
  uint16_t timeoutMs_ = 120;
  uint16_t frameErrorCount_ = 0;
  uint32_t defaultBaudRate_ = 19200;
  float unitToMillimeters_ = 1.0f;
  char startChar_ = '\0';
  char endChar_ = '\n';
  bool allowSignedValues_ = false;
  String prefix_;
  RangeReading lastReading_;
};

}  // namespace DiFinders
