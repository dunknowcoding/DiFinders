#pragma once

#include <Arduino.h>
#include <Wire.h>

#include "../../core/DiFindersDebug.h"
#include "../../core/DiFindersGpioInterrupt.h"
#include "../../core/DiFindersI2C.h"
#include "../../core/DiFindersTypes.h"

namespace DiFinders {

class VL6180XSensor {
 public:
  VL6180XSensor() = default;

  explicit VL6180XSensor(uint8_t address) : address_(address) {}

  bool begin(TwoWire& wire = Wire, uint8_t address = 0x29) {
    bus_ = &wire;
    address_ = address;
    initialized_ = initMandatory();
    if (!initialized_) {
      return false;
    }
    configureDefault();
    return true;
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

  uint16_t timeout() const {
    return ioTimeoutMs_;
  }

  bool timeoutOccurred() {
    bool result = didTimeout_;
    didTimeout_ = false;
    return result;
  }

  bool setAddress(uint8_t newAddress) {
    if (!writeReg(I2C_SLAVE__DEVICE_ADDRESS, newAddress & 0x7F)) {
      return false;
    }
    address_ = newAddress;
    return true;
  }

  uint8_t address() const {
    return address_;
  }

  void configureDefault() {
    writeReg(READOUT__AVERAGING_SAMPLE_PERIOD, 0x30);
    writeReg(SYSALS__ANALOGUE_GAIN, 0x46);
    writeReg(SYSRANGE__VHV_REPEAT_RATE, 0xFF);
    writeReg16(SYSALS__INTEGRATION_PERIOD, 0x0063);
    writeReg(SYSRANGE__VHV_RECALIBRATE, 0x01);
    writeReg(SYSRANGE__INTERMEASUREMENT_PERIOD, 0x09);
    writeReg(SYSALS__INTERMEASUREMENT_PERIOD, 0x31);
    writeReg(SYSTEM__INTERRUPT_CONFIG_GPIO, 0x24);
    writeReg(SYSRANGE__MAX_CONVERGENCE_TIME, 0x31);
    writeReg(INTERLEAVED_MODE__ENABLE, 0x00);
    setScaling(1);
  }

  void setScaling(uint8_t scaling) {
    static const uint16_t scalerValues[] = {0, 253, 127, 84};
    static const uint8_t defaultCrosstalkValidHeight = 20;

    if (scaling < 1 || scaling > 3) {
      return;
    }

    scaling_ = scaling;
    writeReg16(RANGE_SCALER, scalerValues[scaling_]);
    writeReg(SYSRANGE__PART_TO_PART_RANGE_OFFSET, static_cast<uint8_t>(partToPartOffset_ / scaling_));
    writeReg(SYSRANGE__CROSSTALK_VALID_HEIGHT, defaultCrosstalkValidHeight / scaling_);

    uint8_t checks = readReg(SYSRANGE__RANGE_CHECK_ENABLES);
    writeReg(SYSRANGE__RANGE_CHECK_ENABLES, static_cast<uint8_t>((checks & 0xFE) | (scaling_ == 1 ? 1 : 0)));
  }

  uint8_t scaling() const {
    return scaling_;
  }

  uint8_t readRangeRaw() {
    writeReg(SYSRANGE__START, 0x01);
    return readContinuousRangeRaw();
  }

  uint16_t millimeters() {
    return read().distanceMm;
  }

  bool dataReady() {
    return initialized_ && ((readReg(RESULT__INTERRUPT_STATUS_GPIO) & 0x07U) == 0x04U);
  }

  bool readIfReady(RangeReading& reading) {
    if (!dataReady()) {
      return false;
    }
    uint8_t raw = readReg(RESULT__RANGE_VAL);
    writeReg(SYSTEM__INTERRUPT_CLEAR, 0x01);
    uint8_t deviceStatus = readRangeStatus();
    reading.timestampMs = millis();
    reading.rawValue = raw;
    reading.distanceMm = static_cast<uint16_t>(raw) * scaling_;
    reading.status = mapRangeStatus(deviceStatus);
    lastReading_ = reading;
    return reading.status != SensorStatus::NotReady;
  }

  bool attachDataReadyPin(uint8_t pin, uint8_t mode = DF_IRQ_FALLING) {
    intPin_ = pin;
    pinMode(intPin_, INPUT_PULLUP);
    s_irqTarget_ = this;
    attachGpioInterrupt(intPin_, VL6180XSensor::irqHandler, irqLine_, mode);
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

  RangeReading read() {
    RangeReading reading;
    reading.timestampMs = millis();

    if (!initialized_) {
      reading.status = SensorStatus::NotReady;
      lastReading_ = reading;
      return reading;
    }

    uint8_t raw = readRangeRaw();
    uint8_t deviceStatus = readRangeStatus();
    lastDeviceStatus_ = deviceStatus;

    reading.rawValue = raw;
    if (didTimeout_) {
      reading.status = SensorStatus::Timeout;
      lastReading_ = reading;
      return reading;
    }

    reading.distanceMm = static_cast<uint16_t>(raw) * scaling_;
    reading.status = mapRangeStatus(deviceStatus);
    lastReading_ = reading;
    return reading;
  }

  uint16_t readAmbient() {
    writeReg(SYSALS__START, 0x01);
    return readAmbientContinuous();
  }

  void startContinuous(uint16_t periodMs = 100) {
    int16_t periodReg = static_cast<int16_t>(periodMs / 10U) - 1;
    periodReg = constrain(periodReg, 0, 254);
    writeReg(SYSRANGE__INTERMEASUREMENT_PERIOD, static_cast<uint8_t>(periodReg));
    writeReg(SYSRANGE__START, 0x03);
    continuousMode_ = true;
  }

  void startAmbientContinuous(uint16_t periodMs = 500) {
    int16_t periodReg = static_cast<int16_t>(periodMs / 10U) - 1;
    periodReg = constrain(periodReg, 0, 254);
    writeReg(SYSALS__INTERMEASUREMENT_PERIOD, static_cast<uint8_t>(periodReg));
    writeReg(SYSALS__START, 0x03);
  }

  void startInterleavedContinuous(uint16_t periodMs = 500) {
    int16_t periodReg = static_cast<int16_t>(periodMs / 10U) - 1;
    periodReg = constrain(periodReg, 0, 254);
    writeReg(INTERLEAVED_MODE__ENABLE, 1);
    writeReg(SYSALS__INTERMEASUREMENT_PERIOD, static_cast<uint8_t>(periodReg));
    writeReg(SYSALS__START, 0x03);
  }

  void stopContinuous() {
    writeReg(SYSRANGE__START, 0x01);
    writeReg(SYSALS__START, 0x01);
    writeReg(INTERLEAVED_MODE__ENABLE, 0);
    continuousMode_ = false;
  }

  uint8_t lastDeviceStatus() const {
    return lastDeviceStatus_;
  }

  uint8_t lastI2cStatus() const {
    return lastI2cStatus_;
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
    info.secondaryPin = shutdownPin_;
    info.address = address_;
    info.deviceId = initialized_ ? 0xB4 : 0x00;
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

  const RangeReading& lastReading() const {
    return lastReading_;
  }

  const char* lastStatusText() const {
    return statusText(lastReading_.status);
  }

 private:
  static void irqHandler() {
    if (s_irqTarget_ != nullptr) {
      gpioInterruptIsr(&s_irqTarget_->irqLine_);
    }
  }

  static VL6180XSensor* s_irqTarget_;

  static constexpr uint8_t kUnassignedPin = 0xFF;

  enum Reg : uint16_t {
    IDENTIFICATION__MODEL_ID = 0x000,
    SYSTEM__INTERRUPT_CONFIG_GPIO = 0x014,
    SYSTEM__INTERRUPT_CLEAR = 0x015,
    SYSTEM__FRESH_OUT_OF_RESET = 0x016,
    SYSRANGE__START = 0x018,
    SYSRANGE__INTERMEASUREMENT_PERIOD = 0x01B,
    SYSRANGE__MAX_CONVERGENCE_TIME = 0x01C,
    SYSRANGE__CROSSTALK_VALID_HEIGHT = 0x021,
    SYSRANGE__PART_TO_PART_RANGE_OFFSET = 0x024,
    SYSRANGE__RANGE_CHECK_ENABLES = 0x02D,
    SYSRANGE__VHV_RECALIBRATE = 0x02E,
    SYSRANGE__VHV_REPEAT_RATE = 0x031,
    SYSALS__START = 0x038,
    SYSALS__INTERMEASUREMENT_PERIOD = 0x03E,
    SYSALS__ANALOGUE_GAIN = 0x03F,
    SYSALS__INTEGRATION_PERIOD = 0x040,
    RESULT__RANGE_STATUS = 0x04D,
    RESULT__INTERRUPT_STATUS_GPIO = 0x04F,
    RESULT__ALS_VAL = 0x050,
    RESULT__RANGE_VAL = 0x062,
    RANGE_SCALER = 0x096,
    READOUT__AVERAGING_SAMPLE_PERIOD = 0x10A,
    I2C_SLAVE__DEVICE_ADDRESS = 0x212,
    INTERLEAVED_MODE__ENABLE = 0x2A3,
  };

  bool initMandatory() {
    uint8_t modelId = readReg(IDENTIFICATION__MODEL_ID);
    if (modelId != 0xB4) {
      return false;
    }

    partToPartOffset_ = static_cast<int8_t>(readReg(SYSRANGE__PART_TO_PART_RANGE_OFFSET));

    if (readReg(SYSTEM__FRESH_OUT_OF_RESET) == 1) {
      scaling_ = 1;
      writeReg(0x0207, 0x01);
      writeReg(0x0208, 0x01);
      writeReg(0x0096, 0x00);
      writeReg(0x0097, 0xFD);
      writeReg(0x00E3, 0x01);
      writeReg(0x00E4, 0x03);
      writeReg(0x00E5, 0x02);
      writeReg(0x00E6, 0x01);
      writeReg(0x00E7, 0x03);
      writeReg(0x00F5, 0x02);
      writeReg(0x00D9, 0x05);
      writeReg(0x00DB, 0xCE);
      writeReg(0x00DC, 0x03);
      writeReg(0x00DD, 0xF8);
      writeReg(0x009F, 0x00);
      writeReg(0x00A3, 0x3C);
      writeReg(0x00B7, 0x00);
      writeReg(0x00BB, 0x3C);
      writeReg(0x00B2, 0x09);
      writeReg(0x00CA, 0x09);
      writeReg(0x0198, 0x01);
      writeReg(0x01B0, 0x17);
      writeReg(0x01AD, 0x00);
      writeReg(0x00FF, 0x05);
      writeReg(0x0100, 0x05);
      writeReg(0x0199, 0x05);
      writeReg(0x01A6, 0x1B);
      writeReg(0x01AC, 0x3E);
      writeReg(0x01A7, 0x1F);
      writeReg(0x0030, 0x00);
      writeReg(SYSTEM__FRESH_OUT_OF_RESET, 0);
      return lastI2cStatus_ == 0;
    }

    uint16_t currentScaler = readReg16(RANGE_SCALER);
    if (currentScaler == 84) {
      scaling_ = 3;
    } else if (currentScaler == 127) {
      scaling_ = 2;
    } else {
      scaling_ = 1;
    }
    partToPartOffset_ = static_cast<int8_t>(partToPartOffset_ * scaling_);
    return true;
  }

  uint8_t readContinuousRangeRaw() {
    uint32_t start = millis();
    while ((readReg(RESULT__INTERRUPT_STATUS_GPIO) & 0x07) != 0x04) {
      if (timeoutExpired(start)) {
        didTimeout_ = true;
        return 255;
      }
    }

    uint8_t range = readReg(RESULT__RANGE_VAL);
    writeReg(SYSTEM__INTERRUPT_CLEAR, 0x01);
    return range;
  }

  uint16_t readAmbientContinuous() {
    uint32_t start = millis();
    while ((readReg(RESULT__INTERRUPT_STATUS_GPIO) & 0x38) != 0x20) {
      if (timeoutExpired(start)) {
        didTimeout_ = true;
        return 0;
      }
    }

    uint16_t ambient = readReg16(RESULT__ALS_VAL);
    writeReg(SYSTEM__INTERRUPT_CLEAR, 0x02);
    return ambient;
  }

  uint8_t readRangeStatus() {
    return static_cast<uint8_t>(readReg(RESULT__RANGE_STATUS) >> 4);
  }

  SensorStatus mapRangeStatus(uint8_t status) const {
    switch (status) {
      case 0:
        return SensorStatus::Ok;
      case 12:
      case 13:
      case 14:
      case 15:
        return SensorStatus::OutOfRange;
      default:
        return SensorStatus::Error;
    }
  }

  bool timeoutExpired(uint32_t start) const {
    return ioTimeoutMs_ > 0 && (millis() - start) > ioTimeoutMs_;
  }

  bool writeReg(uint16_t reg, uint8_t value) {
    return detail::writeReg8_16(bus_, address_, reg, value, &lastI2cStatus_);
  }

  bool writeReg16(uint16_t reg, uint16_t value) {
    return detail::writeReg16_16(bus_, address_, reg, value, &lastI2cStatus_);
  }

  uint8_t readReg(uint16_t reg) {
    uint8_t value = 0;
    detail::readReg8_16(bus_, address_, reg, value, &lastI2cStatus_);
    return value;
  }

  uint16_t readReg16(uint16_t reg) {
    uint16_t value = 0;
    detail::readReg16_16(bus_, address_, reg, value, &lastI2cStatus_);
    return value;
  }

  TwoWire* bus_ = &Wire;
  uint8_t address_ = 0x29;
  uint8_t shutdownPin_ = kUnassignedPin;
  uint8_t scaling_ = 1;
  int8_t partToPartOffset_ = 0;
  uint16_t ioTimeoutMs_ = 200;
  bool didTimeout_ = false;
  bool initialized_ = false;
  bool continuousMode_ = false;
  bool powered_ = true;
  bool shutdownActiveHigh_ = true;
  uint8_t lastDeviceStatus_ = 0;
  uint8_t lastI2cStatus_ = 0;
  RangeReading lastReading_;
  GpioInterruptLine irqLine_;
  uint8_t intPin_ = kUnassignedPin;
};

VL6180XSensor* VL6180XSensor::s_irqTarget_ = nullptr;

}  // namespace DiFinders
