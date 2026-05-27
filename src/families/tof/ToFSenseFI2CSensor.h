#pragma once

#include <Arduino.h>
#include <Wire.h>

#include "../../core/DiFindersDebug.h"
#include "../../core/DiFindersDiagnostics.h"
#include "../../core/DiFindersTypes.h"

namespace DiFinders {

class ToFSenseFI2CSensor {
 public:
  ToFSenseFI2CSensor() = default;
  explicit ToFSenseFI2CSensor(uint8_t address) : address_(address) {}

  bool begin(TwoWire& wire = Wire, uint8_t address = 0x08) {
    bus_ = &wire;
    address_ = address;
    initialized_ = startMeasurement();
    if (!initialized_) {
      return false;
    }
    delay(100);
    RangeReading reading = read();
    initialized_ = reading.status == SensorStatus::Ok ||
                   reading.status == SensorStatus::OutOfRange ||
                   reading.status == SensorStatus::Error;
    return initialized_;
  }

  bool ready() const {
    return initialized_ && bus_ != nullptr;
  }

  void setTimeout(uint16_t timeoutMs) {
    timeoutMs_ = timeoutMs;
  }

  uint16_t timeout() const {
    return timeoutMs_;
  }

  bool startMeasurement() {
    if (bus_ == nullptr) {
      return false;
    }
    const uint8_t command[2] = {kCommandTakeRangeReading, kCommandSignalStatus};
    bus_->beginTransmission(address_);
    bus_->write(command, sizeof(command));
    lastI2cStatus_ = bus_->endTransmission();
    return lastI2cStatus_ == 0;
  }

  RangeReading read() {
    RangeReading reading;
    reading.timestampMs = millis();
    if (bus_ == nullptr) {
      reading.status = SensorStatus::NotReady;
      lastReading_ = reading;
      return reading;
    }

    uint32_t start = millis();
    while (bus_->requestFrom(address_, static_cast<uint8_t>(8)) != 8) {
      while (bus_->available() > 0) {
        bus_->read();
      }
      if (expired(start)) {
        reading.status = SensorStatus::Timeout;
        lastReading_ = reading;
        return reading;
      }
      delay(2);
    }

    uint32_t distanceMm = 0;
    uint32_t signalAndStatus = 0;
    distanceMm |= static_cast<uint32_t>(static_cast<uint8_t>(bus_->read()));
    distanceMm |= static_cast<uint32_t>(static_cast<uint8_t>(bus_->read())) << 8;
    distanceMm |= static_cast<uint32_t>(static_cast<uint8_t>(bus_->read())) << 16;
    distanceMm |= static_cast<uint32_t>(static_cast<uint8_t>(bus_->read())) << 24;
    signalAndStatus |= static_cast<uint32_t>(static_cast<uint8_t>(bus_->read()));
    signalAndStatus |= static_cast<uint32_t>(static_cast<uint8_t>(bus_->read())) << 8;
    signalAndStatus |= static_cast<uint32_t>(static_cast<uint8_t>(bus_->read())) << 16;
    signalAndStatus |= static_cast<uint32_t>(static_cast<uint8_t>(bus_->read())) << 24;

    lastSignalStrength_ = static_cast<uint16_t>(signalAndStatus >> 16);
    lastDeviceStatus_ = static_cast<uint16_t>(signalAndStatus);
    reading.distanceMm = static_cast<uint16_t>(distanceMm > 65535UL ? 65535UL : distanceMm);
    reading.rawValue = reading.distanceMm;
    reading.status = lastDeviceStatus_ == 1 ? SensorStatus::Ok : SensorStatus::Error;
    if (reading.distanceMm == 0) {
      reading.status = SensorStatus::OutOfRange;
    }

    lastReading_ = reading;
    initialized_ = true;
    (void)startMeasurement();
    return reading;
  }

  RangeReading readIfReady() {
    return read();
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

  uint16_t deviceStatus() const {
    return lastDeviceStatus_;
  }

  const char* lastStatusText() const {
    switch (lastDeviceStatus_) {
      case 1:
        return "ok";
      case 0:
        return "idle";
      default:
        return "device_error";
    }
  }

  uint8_t address() const {
    return address_;
  }

  uint8_t lastI2cStatus() const {
    return lastI2cStatus_;
  }

  SensorProbe probe() const {
    SensorProbe info;
    info.kind = SensorKind::Range;
    info.busType = SensorBusType::I2c;
    info.configured = bus_ != nullptr;
    info.powered = initialized_;
    info.address = address_;
    return info;
  }

  SelfTestReport selfTest() {
    SelfTestReport report;
    report.status = ready() ? lastReading_.status : SensorStatus::NotReady;
    report.passed = ready() &&
                    (lastReading_.status == SensorStatus::Ok || lastReading_.status == SensorStatus::OutOfRange);
    report.value = lastReading_.distanceMm;
    report.detail = static_cast<uint8_t>(lastDeviceStatus_ > 255 ? 255 : lastDeviceStatus_);
    return report;
  }

  const RangeReading& lastReading() const {
    return lastReading_;
  }

  uint8_t lastBusError() const { return lastI2cStatus_; }

  bool readRegister8(uint16_t reg, uint8_t& value) {
    return detail::readReg8_16(bus_, address_, reg, value, &lastI2cStatus_);
  }

  bool readRegister16(uint16_t reg, uint16_t& value) {
    return detail::readReg16_16(bus_, address_, reg, value, &lastI2cStatus_);
  }
  bool writeRegister8(uint16_t reg, uint8_t value) {
    return detail::writeReg8_16(bus_, address_, reg, value, &lastI2cStatus_);
  }
  bool writeRegister16(uint16_t reg, uint16_t value) {
    return detail::writeReg16_16(bus_, address_, reg, value, &lastI2cStatus_);
  }
  bool dumpRegisters(Print& out, uint16_t startReg, uint8_t count) {
    for (uint8_t i = 0; i < count; ++i) {
      uint8_t v = 0;
      uint16_t reg = static_cast<uint16_t>(startReg + i);
      if (!readRegister8(reg, v)) {
        return false;
      }
      out.print(F("0x"));
      if (reg < 0x10) out.print('0');
      out.print(reg, HEX);
      out.print(F("=0x"));
      if (v < 0x10) out.print('0');
      out.println(v, HEX);
    }
    return true;
  }

  SensorHealth health() const {
    return makeHealth(ready(), probe().busType, lastReading_.status, lastI2cStatus_, lastReading_.distanceMm, 0);
  }

 private:
  static constexpr uint8_t kCommandTakeRangeReading = 0x24;
  static constexpr uint8_t kCommandSignalStatus = 0x28;

  bool expired(uint32_t start) const {
    return timeoutMs_ > 0 && (millis() - start) > timeoutMs_;
  }

  TwoWire* bus_ = nullptr;
  uint8_t address_ = 0x08;
  uint8_t lastI2cStatus_ = 0;
  uint16_t timeoutMs_ = 150;
  uint16_t lastSignalStrength_ = 0;
  uint16_t lastDeviceStatus_ = 0;
  bool initialized_ = false;
  RangeReading lastReading_;
};

}  // namespace DiFinders