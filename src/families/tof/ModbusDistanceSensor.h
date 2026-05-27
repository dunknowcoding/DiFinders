#pragma once

#include <Arduino.h>
#include <Stream.h>

#include "../../core/DiFindersDiagnostics.h"
#include "../../core/DiFindersTypes.h"

namespace DiFinders {

class ModbusDistanceSensor {
 public:
  ModbusDistanceSensor() = default;
  explicit ModbusDistanceSensor(Stream& serial) { begin(serial); }

  void begin(Stream& serial) {
    serial_ = &serial;
  }

  bool ready() const {
    return serial_ != nullptr;
  }

  void setTimeout(uint16_t timeoutMs) {
    timeoutMs_ = timeoutMs;
  }

  void setResponseDelay(uint16_t responseDelayMs) {
    responseDelayMs_ = responseDelayMs;
  }

  void setDeviceAddress(uint8_t deviceAddress) {
    deviceAddress_ = deviceAddress;
  }

  uint8_t deviceAddress() const {
    return deviceAddress_;
  }

  void setDistanceRegister(uint16_t registerAddress, uint8_t registerCount = 1, uint8_t functionCode = 0x04) {
    registerAddress_ = registerAddress;
    registerCount_ = registerCount < 1 ? 1 : (registerCount > 2 ? 2 : registerCount);
    functionCode_ = functionCode;
  }

  void setScaling(uint16_t numerator, uint16_t denominator = 1) {
    scaleNumerator_ = numerator == 0 ? 1 : numerator;
    scaleDenominator_ = denominator == 0 ? 1 : denominator;
  }

  void setTransmitEnablePin(uint8_t pin, bool activeHigh = true) {
    txEnablePin_ = pin;
    txEnableActiveHigh_ = activeHigh;
    pinMode(txEnablePin_, OUTPUT);
    setTransmitMode(false);
  }

  RangeReading read() {
    RangeReading reading;
    reading.timestampMs = millis();
    if (!ready()) {
      reading.status = SensorStatus::NotReady;
      lastReading_ = reading;
      return reading;
    }

    flushInput();
    uint8_t request[8] = {
        deviceAddress_,
        functionCode_,
        static_cast<uint8_t>(registerAddress_ >> 8),
        static_cast<uint8_t>(registerAddress_),
        0x00,
        registerCount_,
        0x00,
        0x00,
    };
    const uint16_t requestCrc = crc16(request, 6);
    request[6] = static_cast<uint8_t>(requestCrc);
    request[7] = static_cast<uint8_t>(requestCrc >> 8);

    setTransmitMode(true);
    serial_->write(request, sizeof(request));
    serial_->flush();
    setTransmitMode(false);

    if (responseDelayMs_ > 0) {
      delay(responseDelayMs_);
    }

    const uint8_t expectedLength = static_cast<uint8_t>(5 + registerCount_ * 2);
    uint8_t response[9] = {0};
    if (!readBytes(response, expectedLength)) {
      reading.status = SensorStatus::Timeout;
      lastReading_ = reading;
      return reading;
    }

    const uint16_t responseCrc = crc16(response, expectedLength - 2);
    const uint16_t actualCrc = static_cast<uint16_t>(response[expectedLength - 2]) |
                               (static_cast<uint16_t>(response[expectedLength - 1]) << 8);
    if (responseCrc != actualCrc || response[0] != deviceAddress_) {
      reading.status = SensorStatus::Error;
      ++frameErrorCount_;
      lastReading_ = reading;
      return reading;
    }

    if ((response[1] & 0x80U) != 0) {
      lastExceptionCode_ = response[2];
      reading.status = SensorStatus::Error;
      lastReading_ = reading;
      return reading;
    }

    if (response[1] != functionCode_ || response[2] != static_cast<uint8_t>(registerCount_ * 2)) {
      reading.status = SensorStatus::Error;
      ++frameErrorCount_;
      lastReading_ = reading;
      return reading;
    }

    uint32_t raw = 0;
    if (registerCount_ == 1) {
      raw = (static_cast<uint16_t>(response[3]) << 8) | response[4];
    } else {
      raw = static_cast<uint32_t>(response[3]) << 24;
      raw |= static_cast<uint32_t>(response[4]) << 16;
      raw |= static_cast<uint32_t>(response[5]) << 8;
      raw |= static_cast<uint32_t>(response[6]);
    }

    rawValue_ = raw;
    const uint32_t scaled = (raw * scaleNumerator_) / scaleDenominator_;
    reading.distanceMm = static_cast<uint16_t>(scaled > 65535UL ? 65535UL : scaled);
    reading.rawValue = static_cast<uint16_t>(raw > 65535UL ? 65535UL : raw);
    reading.status = reading.distanceMm == 0 ? SensorStatus::OutOfRange : SensorStatus::Ok;
    lastReading_ = reading;
    return reading;
  }

  uint16_t millimeters() {
    return read().distanceMm;
  }

  uint32_t rawValue() const {
    return rawValue_;
  }

  uint8_t lastExceptionCode() const {
    return lastExceptionCode_;
  }

  uint16_t frameErrorCount() const {
    return frameErrorCount_;
  }

  SensorProbe probe() const {
    SensorProbe info;
    info.kind = SensorKind::Range;
    info.busType = SensorBusType::Rs485;
    info.configured = ready();
    info.address = deviceAddress_;
    return info;
  }

  SelfTestReport selfTest() {
    SelfTestReport report;
    report.status = ready() ? lastReading_.status : SensorStatus::NotReady;
    report.passed = ready() &&
                    (lastReading_.status == SensorStatus::Ok || lastReading_.status == SensorStatus::OutOfRange);
    report.value = lastReading_.distanceMm;
    report.detail = lastExceptionCode_;
    return report;
  }

  const RangeReading& lastReading() const {
    return lastReading_;
  }

  uint8_t lastBusError() const { return 0; }

  void flushRx() {
    flushInput();
  }

  SensorHealth health() const {
    return makeHealth(ready(), probe().busType, lastReading_.status, 0, lastReading_.distanceMm, frameErrorCount_);
  }

 private:
  static uint16_t crc16(const uint8_t* data, uint8_t length) {
    uint16_t crc = 0xFFFF;
    for (uint8_t index = 0; index < length; ++index) {
      crc ^= data[index];
      for (uint8_t bit = 0; bit < 8; ++bit) {
        if ((crc & 0x0001U) != 0) {
          crc >>= 1;
          crc ^= 0xA001U;
        } else {
          crc >>= 1;
        }
      }
    }
    return crc;
  }

  bool expired(uint32_t start) const {
    return timeoutMs_ > 0 && (millis() - start) > timeoutMs_;
  }

  void setTransmitMode(bool transmitEnabled) {
    if (txEnablePin_ == 0xFF) {
      return;
    }
    const bool level = transmitEnabled ? txEnableActiveHigh_ : !txEnableActiveHigh_;
    digitalWrite(txEnablePin_, level ? HIGH : LOW);
    delayMicroseconds(40);
  }

  void flushInput() {
    while (serial_ != nullptr && serial_->available() > 0) {
      serial_->read();
    }
  }

  bool readBytes(uint8_t* buffer, uint8_t length) {
    uint32_t start = millis();
    uint8_t index = 0;
    while (index < length) {
      while (serial_->available() > 0 && index < length) {
        buffer[index++] = static_cast<uint8_t>(serial_->read());
      }
      if (index >= length) {
        return true;
      }
      if (expired(start)) {
        return false;
      }
    }
    return true;
  }

  Stream* serial_ = nullptr;
  uint16_t timeoutMs_ = 200;
  uint16_t responseDelayMs_ = 40;
  uint16_t registerAddress_ = 0x0000;
  uint16_t scaleNumerator_ = 1;
  uint16_t scaleDenominator_ = 1;
  uint32_t rawValue_ = 0;
  uint8_t deviceAddress_ = 0x01;
  uint8_t functionCode_ = 0x04;
  uint8_t registerCount_ = 1;
  uint8_t txEnablePin_ = 0xFF;
  uint8_t lastExceptionCode_ = 0;
  uint16_t frameErrorCount_ = 0;
  bool txEnableActiveHigh_ = true;
  RangeReading lastReading_;
};

}  // namespace DiFinders