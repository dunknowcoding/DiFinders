#pragma once

#include <Arduino.h>
#include <Wire.h>

#include "../../core/DiFindersDebug.h"
#include "../../core/DiFindersGpioInterrupt.h"
#include "../../core/DiFindersI2C.h"
#include "../../core/DiFindersDiagnostics.h"
#include "../../core/DiFindersTypes.h"

namespace DiFinders {

class VL53L0XSensor {
 public:
  VL53L0XSensor() = default;
  explicit VL53L0XSensor(uint8_t address) : address_(address) {}

  bool begin(TwoWire& wire = Wire, uint8_t address = 0x29, bool io2v8 = true) {
    bus_ = &wire;
    address_ = address;
    io2v8_ = io2v8;
    delay(10);
    initialized_ = initCore();
    return initialized_;
  }

  void attachShutdownPin(uint8_t shutdownPin, bool activeHigh = true) {
    shutdownPin_ = shutdownPin;
    shutdownActiveHigh_ = activeHigh;
    pinMode(shutdownPin_, OUTPUT);
    powerOn();
  }

  void powerOn() {
    if (shutdownPin_ != kUnassignedPin) {
      digitalWrite(shutdownPin_, shutdownActiveHigh_ ? HIGH : LOW);
      delay(2);
    }
    powered_ = true;
  }

  void powerOff() {
    if (shutdownPin_ != kUnassignedPin) {
      digitalWrite(shutdownPin_, shutdownActiveHigh_ ? LOW : HIGH);
      delay(2);
    }
    powered_ = false;
    initialized_ = false;
  }

  bool powered() const {
    return powered_;
  }

  bool ready() const {
    return initialized_;
  }

  void setTimeout(uint16_t timeoutMs) {
    ioTimeoutMs_ = timeoutMs;
  }

  bool timeoutOccurred() {
    bool result = didTimeout_;
    didTimeout_ = false;
    return result;
  }

  bool setAddress(uint8_t newAddress) {
    if (!writeReg(I2C_SLAVE_DEVICE_ADDRESS, newAddress & 0x7F)) {
      return false;
    }
    address_ = newAddress;
    return true;
  }

  uint8_t address() const {
    return address_;
  }

  void setSignalRateLimitMcps(float limitMcps) {
    if (limitMcps < 0.0f) {
      limitMcps = 0.0f;
    }
    if (limitMcps > 511.99f) {
      limitMcps = 511.99f;
    }
    uint16_t encoded = static_cast<uint16_t>(limitMcps * 128.0f);
    writeReg16(FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT, encoded);
  }

  void startContinuous(uint32_t periodMs = 0) {
    if (!initialized_) {
      return;
    }

    writeReg(0x80, 0x01);
    writeReg(0xFF, 0x01);
    writeReg(0x00, 0x00);
    writeReg(0x91, stopVariable_);
    writeReg(0x00, 0x01);
    writeReg(0xFF, 0x00);
    writeReg(0x80, 0x00);

    if (periodMs != 0) {
      uint16_t oscCalibrate = readReg16(OSC_CALIBRATE_VAL);
      uint32_t period = periodMs;
      if (oscCalibrate != 0) {
        period *= oscCalibrate;
      }
      writeReg32(SYSTEM_INTERMEASUREMENT_PERIOD, period);
      writeReg(SYSRANGE_START, 0x04);
    } else {
      writeReg(SYSRANGE_START, 0x02);
    }
    continuousMode_ = true;
  }

  void stopContinuous() {
    writeReg(SYSRANGE_START, 0x01);
    writeReg(0xFF, 0x01);
    writeReg(0x00, 0x00);
    writeReg(0x91, 0x00);
    writeReg(0x00, 0x01);
    writeReg(0xFF, 0x00);
    continuousMode_ = false;
  }

  RangeReading read() {
    return continuousMode_ ? readContinuous() : readSingle();
  }

  RangeReading readSingle() {
    RangeReading reading;
    reading.timestampMs = millis();

    if (!initialized_) {
      reading.status = SensorStatus::NotReady;
      lastReading_ = reading;
      return reading;
    }

    writeReg(0x80, 0x01);
    writeReg(0xFF, 0x01);
    writeReg(0x00, 0x00);
    writeReg(0x91, stopVariable_);
    writeReg(0x00, 0x01);
    writeReg(0xFF, 0x00);
    writeReg(0x80, 0x00);

    writeReg(SYSRANGE_START, 0x01);
    uint32_t start = millis();
    while (readReg(SYSRANGE_START) & 0x01) {
      if (timeoutExpired(start)) {
        didTimeout_ = true;
        reading.status = SensorStatus::Timeout;
        lastReading_ = reading;
        return reading;
      }
      delayMicroseconds(150);
    }

    return readContinuous();
  }

  RangeReading readContinuous() {
    RangeReading reading;
    reading.timestampMs = millis();
    if (!initialized_) {
      reading.status = SensorStatus::NotReady;
      lastReading_ = reading;
      return reading;
    }

    uint32_t start = millis();
    while ((readReg(RESULT_INTERRUPT_STATUS) & 0x07) == 0) {
      if (timeoutExpired(start)) {
        didTimeout_ = true;
        reading.status = SensorStatus::Timeout;
        lastReading_ = reading;
        return reading;
      }
      delayMicroseconds(150);
    }

    uint16_t range = readReg16(static_cast<uint8_t>(RESULT_RANGE_STATUS + 10));
    lastDeviceStatus_ = static_cast<uint8_t>((readReg(RESULT_RANGE_STATUS) & 0x78) >> 3);
    writeReg(SYSTEM_INTERRUPT_CLEAR, 0x01);

    reading.distanceMm = range;
    reading.rawValue = range;
    reading.status = mapDeviceStatus(lastDeviceStatus_);
    lastReading_ = reading;
    return reading;
  }

  uint16_t millimeters() {
    return read().distanceMm;
  }

  bool dataReady() {
    return initialized_ && ((readReg(RESULT_INTERRUPT_STATUS) & 0x07U) != 0U);
  }

  bool readIfReady(RangeReading& reading) {
    if (!dataReady()) {
      return false;
    }
    reading = continuousMode_ ? readContinuous() : readSingle();
    return reading.status != SensorStatus::NotReady;
  }

  bool attachDataReadyPin(uint8_t pin, uint8_t mode = DF_IRQ_FALLING) {
    intPin_ = pin;
    pinMode(intPin_, INPUT_PULLUP);
    s_irqTarget_ = this;
    attachGpioInterrupt(intPin_, VL53L0XSensor::irqHandler, irqLine_, mode);
    return true;
  }

  void detachDataReadyPin() {
    if (intPin_ != kUnassignedPin) {
      detachGpioInterrupt(intPin_);
    }
    irqLine_.disarm();
    if (s_irqTarget_ == this) {
      s_irqTarget_ = nullptr;
    }
  }

  bool takeDataReadyEvent() {
    return irqLine_.takeEvent();
  }

  /** Distance window for GPIO threshold alerts (mm). Mode 2 = alert when outside [inner, outer]. */
  bool setDistanceWindowMm(uint16_t innerMm, uint16_t outerMm, uint8_t windowMode = 2) {
    if (!initialized_) {
      return false;
    }
    writeReg16(SYSTEM_THRESH_HIGH, outerMm);
    writeReg16(SYSTEM_THRESH_LOW, innerMm);
    uint8_t gpioCfg = static_cast<uint8_t>((readReg(SYSTEM_INTERRUPT_CONFIG_GPIO) & 0xF0U) | (windowMode & 0x0FU));
    writeReg(SYSTEM_INTERRUPT_CONFIG_GPIO, gpioCfg);
    return true;
  }

  bool isOutsideDistanceWindow(uint16_t distanceMm, uint16_t innerMm, uint16_t outerMm) const {
    return distanceMm < innerMm || distanceMm > outerMm;
  }

  uint16_t deviceStatus() const {
    return lastDeviceStatus_;
  }

  SensorProbe probe() const {
    SensorProbe info;
    info.kind = SensorKind::Range;
    info.busType = SensorBusType::I2c;
    info.configured = initialized_;
    info.powered = powered_;
    info.supportsAddressChange = true;
    info.supportsContinuousMode = true;
    info.supportsInterruptReady = true;
    info.supportsThresholdConfig = true;
    info.secondaryPin = shutdownPin_;
    info.address = address_;
    info.deviceId = initialized_ ? 0xEE : 0x00;
    return info;
  }

  SelfTestReport selfTest() {
    SelfTestReport report;
    report.passed = initialized_;
    report.status = initialized_ ? SensorStatus::Ok : SensorStatus::NotReady;
    report.value = lastReading_.distanceMm;
    report.detail = lastDeviceStatus_;
    return report;
  }

  uint8_t lastI2cStatus() const {
    return lastI2cStatus_;
  }

  const RangeReading& lastReading() const {
    return lastReading_;
  }

  const char* lastStatusText() const {
    return statusText(lastReading_.status);
  }

  uint8_t lastBusError() const { return lastI2cStatus_; }

  bool readRegister8(uint8_t reg, uint8_t& value) {
    return detail::readReg8(bus_, address_, reg, value, &lastI2cStatus_);
  }
  bool readRegister16(uint8_t reg, uint16_t& value) {
    return detail::readReg16(bus_, address_, reg, value, &lastI2cStatus_);
  }
  bool writeRegister8(uint8_t reg, uint8_t value) {
    return detail::writeReg8(bus_, address_, reg, value, &lastI2cStatus_);
  }
  bool writeRegister16(uint8_t reg, uint16_t value) {
    return detail::writeReg16(bus_, address_, reg, value, &lastI2cStatus_);
  }
  bool dumpRegisters(Print& out, uint8_t startReg, uint8_t count) {
    for (uint8_t i = 0; i < count; ++i) {
      uint8_t v = 0;
      uint8_t reg = static_cast<uint8_t>(startReg + i);
      if (!readRegister8(reg, v)) {
        return false;
      }
      out.print(F("0x"));
      if (reg < 0x10) {
        out.print('0');
      }
      out.print(reg, HEX);
      out.print(F("=0x"));
      if (v < 0x10) {
        out.print('0');
      }
      out.println(v, HEX);
    }
    return true;
  }

  SensorHealth health() const {
    return makeHealth(ready(), probe().busType, lastReading_.status, lastI2cStatus_, lastReading_.distanceMm,
                      didTimeout_ ? 1U : 0U);
  }

 private:
  static void irqHandler() {
    if (s_irqTarget_ != nullptr) {
      gpioInterruptIsr(&s_irqTarget_->irqLine_);
    }
  }

  static VL53L0XSensor* s_irqTarget_;

  static constexpr uint8_t kUnassignedPin = 0xFF;

  enum Reg : uint8_t {
    SYSRANGE_START = 0x00,
    SYSTEM_SEQUENCE_CONFIG = 0x01,
    SYSTEM_INTERMEASUREMENT_PERIOD = 0x04,
    SYSTEM_INTERRUPT_CONFIG_GPIO = 0x0A,
    SYSTEM_THRESH_HIGH = 0x0C,
    SYSTEM_THRESH_LOW = 0x0E,
    SYSTEM_INTERRUPT_CLEAR = 0x0B,
    RESULT_INTERRUPT_STATUS = 0x13,
    RESULT_RANGE_STATUS = 0x14,
    I2C_SLAVE_DEVICE_ADDRESS = 0x8A,
    MSRC_CONFIG_CONTROL = 0x60,
    FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT = 0x44,
    GLOBAL_CONFIG_SPAD_ENABLES_REF_0 = 0xB0,
    DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD = 0x4E,
    DYNAMIC_SPAD_REF_EN_START_OFFSET = 0x4F,
    GLOBAL_CONFIG_REF_EN_START_SELECT = 0xB6,
    GPIO_HV_MUX_ACTIVE_HIGH = 0x84,
    VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV = 0x89,
    OSC_CALIBRATE_VAL = 0xF8,
    FINAL_RANGE_CONFIG_VCSEL_PERIOD = 0x70,
    PRE_RANGE_CONFIG_VCSEL_PERIOD = 0x50,
    PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI = 0x51,
    FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI = 0x71,
    SYSTEM_RANGE_CONFIG = 0x09,
    SOFT_RESET_GO2_SOFT_RESET_N = 0xBF,
    IDENTIFICATION_MODEL_ID = 0xC0,
  };

  bool initCore() {
    if (readReg(IDENTIFICATION_MODEL_ID) != 0xEE) {
      return false;
    }

    if (io2v8_) {
      writeReg(VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV, static_cast<uint8_t>(readReg(VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV) | 0x01));
    }

    writeReg(0x88, 0x00);
    writeReg(0x80, 0x01);
    writeReg(0xFF, 0x01);
    writeReg(0x00, 0x00);
    stopVariable_ = readReg(0x91);
    writeReg(0x00, 0x01);
    writeReg(0xFF, 0x00);
    writeReg(0x80, 0x00);

    writeReg(MSRC_CONFIG_CONTROL, static_cast<uint8_t>(readReg(MSRC_CONFIG_CONTROL) | 0x12));
    setSignalRateLimitMcps(0.25f);
    writeReg(SYSTEM_SEQUENCE_CONFIG, 0xFF);

    uint8_t spadCount = 0;
    bool aperture = false;
    if (!getSpadInfo(spadCount, aperture)) {
      return false;
    }

    uint8_t spadMap[6] = {0};
    if (!readMulti(GLOBAL_CONFIG_SPAD_ENABLES_REF_0, spadMap, 6)) {
      return false;
    }
    writeReg(0xFF, 0x01);
    writeReg(DYNAMIC_SPAD_REF_EN_START_OFFSET, 0x00);
    writeReg(DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD, 0x2C);
    writeReg(0xFF, 0x00);
    writeReg(GLOBAL_CONFIG_REF_EN_START_SELECT, 0xB4);

    uint8_t firstSpad = aperture ? 12 : 0;
    uint8_t enabled = 0;
    for (uint8_t i = 0; i < 48; ++i) {
      if (i < firstSpad || enabled == spadCount) {
        spadMap[i / 8] &= static_cast<uint8_t>(~(1 << (i % 8)));
      } else if ((spadMap[i / 8] >> (i % 8)) & 0x01) {
        ++enabled;
      }
    }
    if (!writeMulti(GLOBAL_CONFIG_SPAD_ENABLES_REF_0, spadMap, 6)) {
      return false;
    }

    applyDefaultTuning();

    writeReg(SYSTEM_INTERRUPT_CONFIG_GPIO, 0x04);
    writeReg(GPIO_HV_MUX_ACTIVE_HIGH, static_cast<uint8_t>(readReg(GPIO_HV_MUX_ACTIVE_HIGH) & ~0x10));
    writeReg(SYSTEM_INTERRUPT_CLEAR, 0x01);
    writeReg(SYSTEM_SEQUENCE_CONFIG, 0xE8);

    if (!performSingleRefCalibration(0x40)) {
      return false;
    }
    if (!performSingleRefCalibration(0x00)) {
      return false;
    }

    writeReg(SYSTEM_SEQUENCE_CONFIG, 0xE8);
    return lastI2cStatus_ == 0;
  }

  void applyDefaultTuning() {
    writeReg(0xFF, 0x01);
    writeReg(0x00, 0x00);
    writeReg(0xFF, 0x00);
    writeReg(0x09, 0x00);
    writeReg(0x10, 0x00);
    writeReg(0x11, 0x00);
    writeReg(0x24, 0x01);
    writeReg(0x25, 0xFF);
    writeReg(0x75, 0x00);
    writeReg(0xFF, 0x01);
    writeReg(0x4E, 0x2C);
    writeReg(0x48, 0x00);
    writeReg(0x30, 0x20);
    writeReg(0xFF, 0x00);
    writeReg(0x30, 0x09);
    writeReg(0x54, 0x00);
    writeReg(0x31, 0x04);
    writeReg(0x32, 0x03);
    writeReg(0x40, 0x83);
    writeReg(0x46, 0x25);
    writeReg(0x60, 0x00);
    writeReg(0x27, 0x00);
    writeReg(0x50, 0x06);
    writeReg(0x51, 0x00);
    writeReg(0x52, 0x96);
    writeReg(0x56, 0x08);
    writeReg(0x57, 0x30);
    writeReg(0x61, 0x00);
    writeReg(0x62, 0x00);
    writeReg(0x64, 0x00);
    writeReg(0x65, 0x00);
    writeReg(0x66, 0xA0);
    writeReg(0xFF, 0x01);
    writeReg(0x22, 0x32);
    writeReg(0x47, 0x14);
    writeReg(0x49, 0xFF);
    writeReg(0x4A, 0x00);
    writeReg(0xFF, 0x00);
    writeReg(0x7A, 0x0A);
    writeReg(0x7B, 0x00);
    writeReg(0x78, 0x21);
    writeReg(0xFF, 0x01);
    writeReg(0x23, 0x34);
    writeReg(0x42, 0x00);
    writeReg(0x44, 0xFF);
    writeReg(0x45, 0x26);
    writeReg(0x46, 0x05);
    writeReg(0x40, 0x40);
    writeReg(0x0E, 0x06);
    writeReg(0x20, 0x1A);
    writeReg(0x43, 0x40);
    writeReg(0xFF, 0x00);
    writeReg(0x34, 0x03);
    writeReg(0x35, 0x44);
    writeReg(0xFF, 0x01);
    writeReg(0x31, 0x04);
    writeReg(0x4B, 0x09);
    writeReg(0x4C, 0x05);
    writeReg(0x4D, 0x04);
    writeReg(0xFF, 0x00);
    writeReg(0x44, 0x00);
    writeReg(0x45, 0x20);
    writeReg(0x47, 0x08);
    writeReg(0x48, 0x28);
    writeReg(0x67, 0x00);
    writeReg(0x70, 0x04);
    writeReg(0x71, 0x01);
    writeReg(0x72, 0xFE);
    writeReg(0x76, 0x00);
    writeReg(0x77, 0x00);
    writeReg(0xFF, 0x01);
    writeReg(0x0D, 0x01);
    writeReg(0xFF, 0x00);
    writeReg(0x80, 0x01);
    writeReg(0x01, 0xF8);
    writeReg(0xFF, 0x01);
    writeReg(0x8E, 0x01);
    writeReg(0x00, 0x01);
    writeReg(0xFF, 0x00);
    writeReg(0x80, 0x00);
  }

  bool getSpadInfo(uint8_t& count, bool& typeIsAperture) {
    writeReg(0x80, 0x01);
    writeReg(0xFF, 0x01);
    writeReg(0x00, 0x00);
    writeReg(0xFF, 0x06);
    writeReg(0x83, static_cast<uint8_t>(readReg(0x83) | 0x04));
    writeReg(0xFF, 0x07);
    writeReg(0x81, 0x01);
    writeReg(0x80, 0x01);
    writeReg(0x94, 0x6B);
    writeReg(0x83, 0x00);

    uint32_t start = millis();
    while (readReg(0x83) == 0x00) {
      if (timeoutExpired(start)) {
        return false;
      }
      delayMicroseconds(150);
    }

    writeReg(0x83, 0x01);
    uint8_t temp = readReg(0x92);
    count = temp & 0x7F;
    typeIsAperture = ((temp >> 7) & 0x01) != 0;

    writeReg(0x81, 0x00);
    writeReg(0xFF, 0x06);
    writeReg(0x83, static_cast<uint8_t>(readReg(0x83) & ~0x04));
    writeReg(0xFF, 0x01);
    writeReg(0x00, 0x01);
    writeReg(0xFF, 0x00);
    writeReg(0x80, 0x00);
    return true;
  }

  bool performSingleRefCalibration(uint8_t initByte) {
    writeReg(SYSRANGE_START, static_cast<uint8_t>(0x01 | initByte));
    uint32_t start = millis();
    while ((readReg(RESULT_INTERRUPT_STATUS) & 0x07) == 0) {
      if (timeoutExpired(start)) {
        return false;
      }
      delayMicroseconds(150);
    }
    writeReg(SYSTEM_INTERRUPT_CLEAR, 0x01);
    writeReg(SYSRANGE_START, 0x00);
    return true;
  }

  SensorStatus mapDeviceStatus(uint8_t status) const {
    switch (status) {
      case 0:
        return SensorStatus::Ok;
      case 1:
      case 2:
      case 4:
      case 5:
        return SensorStatus::OutOfRange;
      default:
        return SensorStatus::Error;
    }
  }

  bool timeoutExpired(uint32_t start) const {
    return ioTimeoutMs_ > 0 && (millis() - start) > ioTimeoutMs_;
  }

  bool writeReg(uint8_t reg, uint8_t value) {
    return detail::writeReg8(bus_, address_, reg, value, &lastI2cStatus_);
  }

  bool writeReg16(uint8_t reg, uint16_t value) {
    return detail::writeReg16(bus_, address_, reg, value, &lastI2cStatus_);
  }

  bool writeReg32(uint8_t reg, uint32_t value) {
    return detail::writeReg32(bus_, address_, reg, value, &lastI2cStatus_);
  }

  uint8_t readReg(uint8_t reg) {
    uint8_t value = 0;
    detail::readReg8(bus_, address_, reg, value, &lastI2cStatus_);
    return value;
  }

  uint16_t readReg16(uint8_t reg) {
    uint16_t value = 0;
    detail::readReg16(bus_, address_, reg, value, &lastI2cStatus_);
    return value;
  }

  bool writeMulti(uint8_t reg, const uint8_t* src, uint8_t count) {
    bus_->beginTransmission(address_);
    bus_->write(reg);
    while (count-- > 0) {
      bus_->write(*src++);
    }
    lastI2cStatus_ = bus_->endTransmission();
    return lastI2cStatus_ == 0;
  }

  bool readMulti(uint8_t reg, uint8_t* dst, uint8_t count) {
    bus_->beginTransmission(address_);
    bus_->write(reg);
    lastI2cStatus_ = bus_->endTransmission(false);
    if (lastI2cStatus_ != 0) {
      return false;
    }
    uint8_t received = bus_->requestFrom(address_, count);
    if (received != count) {
      lastI2cStatus_ = 4;
      return false;
    }
    while (count-- > 0) {
      *dst++ = bus_->read();
    }
    return true;
  }

  TwoWire* bus_ = &Wire;
  uint8_t address_ = 0x29;
  uint8_t shutdownPin_ = kUnassignedPin;
  uint16_t ioTimeoutMs_ = 200;
  bool didTimeout_ = false;
  bool initialized_ = false;
  bool continuousMode_ = false;
  bool io2v8_ = true;
  bool powered_ = true;
  bool shutdownActiveHigh_ = true;
  uint8_t stopVariable_ = 0;
  uint8_t lastDeviceStatus_ = 0;
  uint8_t lastI2cStatus_ = 0;
  RangeReading lastReading_;
  GpioInterruptLine irqLine_;
  uint8_t intPin_ = kUnassignedPin;
};

VL53L0XSensor* VL53L0XSensor::s_irqTarget_ = nullptr;

}  // namespace DiFinders
