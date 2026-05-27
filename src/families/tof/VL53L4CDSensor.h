#pragma once

#include <Arduino.h>
#include <Wire.h>

#include "../../core/DiFindersDebug.h"
#include "../../core/DiFindersGpioInterrupt.h"
#include "../../core/DiFindersI2C.h"
#include "../../core/DiFindersTypes.h"

namespace DiFinders {

class VL53L4CDSensor {
 public:
  VL53L4CDSensor() = default;
  explicit VL53L4CDSensor(uint8_t address) : address_(address) {}

  bool begin(TwoWire& wire = Wire, uint8_t address = 0x29) {
    bus_ = &wire;
    address_ = address;
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
      delay(10);
    }
    powered_ = true;
  }

  void powerOff() {
    if (shutdownPin_ != kUnassignedPin) {
      digitalWrite(shutdownPin_, shutdownActiveHigh_ ? LOW : HIGH);
      delay(10);
    }
    powered_ = false;
    initialized_ = false;
    ranging_ = false;
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

  bool setRangeTiming(uint32_t timingBudgetMs, uint32_t interMeasurementMs = 0) {
    timingBudgetMs_ = timingBudgetMs;
    interMeasurementMs_ = interMeasurementMs;
    if (!initialized_) {
      return true;
    }
    if (timingBudgetMs < 10 || timingBudgetMs > 200) {
      return false;
    }

    uint16_t oscFrequency = readReg16(OSC_FREQUENCY);
    if (oscFrequency == 0) {
      return false;
    }

    uint32_t macroPeriodUs = (2304UL * (0x40000000UL / oscFrequency)) >> 6U;
    uint32_t timingBudgetUs = timingBudgetMs * 1000UL;

    if (interMeasurementMs == 0) {
      writeReg32(INTERMEASUREMENT_MS, 0);
      timingBudgetUs -= 2500UL;
    } else {
      if (interMeasurementMs <= timingBudgetMs) {
        return false;
      }
      uint16_t clockPll = readReg16(RESULT_OSC_CALIBRATE_VAL) & 0x03FFU;
      if (clockPll == 0) {
        clockPll = 1;
      }
      uint32_t interMeasurementFactor = (static_cast<uint32_t>(interMeasurementMs) * clockPll * 1055UL) / 1000UL;
      writeReg32(INTERMEASUREMENT_MS, interMeasurementFactor);
      timingBudgetUs = (timingBudgetUs - 4300UL) / 2UL;
    }

    uint16_t encodedA = encodeRangeTiming(timingBudgetUs, macroPeriodUs * 16UL);
    uint16_t encodedB = encodeRangeTiming(timingBudgetUs, macroPeriodUs * 12UL);
    writeReg16(RANGE_CONFIG_A, encodedA);
    writeReg16(RANGE_CONFIG_B, encodedB);
    return true;
  }

  void getRangeTiming(uint32_t& timingBudgetMs, uint32_t& interMeasurementMs) const {
    timingBudgetMs = timingBudgetMs_;
    interMeasurementMs = interMeasurementMs_;
  }

  void startContinuous(uint32_t interMeasurementMs = 0) {
    if (!initialized_) {
      return;
    }
    setRangeTiming(timingBudgetMs_, interMeasurementMs);
    uint32_t programmedIntermeasurement = readReg32(INTERMEASUREMENT_MS);
    writeReg(SYSTEM_START, programmedIntermeasurement == 0 ? 0x21 : 0x40);
    ranging_ = true;
  }

  void stopContinuous() {
    if (!initialized_) {
      return;
    }
    writeReg(SYSTEM_START, 0x00);
    ranging_ = false;
  }

  bool dataReady() {
    return initialized_ && (readReg(GPIO_TIO_HV_STATUS) & 0x01U) == 0;
  }

  RangeReading read() {
    RangeReading reading;
    reading.timestampMs = millis();
    if (!initialized_) {
      reading.status = SensorStatus::NotReady;
      lastReading_ = reading;
      return reading;
    }

    if (!ranging_) {
      startContinuous(interMeasurementMs_);
    }

    if (!waitForDataReady()) {
      reading.status = SensorStatus::Timeout;
      lastReading_ = reading;
      return reading;
    }

    uint8_t rawBlock[16] = {0};
    if (!readMulti(RESULT_RANGE_STATUS, rawBlock, sizeof(rawBlock))) {
      reading.status = SensorStatus::Error;
      lastReading_ = reading;
      return reading;
    }

    uint8_t rawStatus = rawBlock[0];
    uint16_t rawSpads = static_cast<uint16_t>((static_cast<uint16_t>(rawBlock[3]) << 8) | rawBlock[4]);
    uint16_t rawSignal = static_cast<uint16_t>((static_cast<uint16_t>(rawBlock[5]) << 8) | rawBlock[6]);
    uint16_t rawAmbient = static_cast<uint16_t>((static_cast<uint16_t>(rawBlock[7]) << 8) | rawBlock[8]);
    uint16_t rawSigma = static_cast<uint16_t>((static_cast<uint16_t>(rawBlock[9]) << 8) | rawBlock[10]);
    uint16_t distance = static_cast<uint16_t>((static_cast<uint16_t>(rawBlock[13]) << 8) | rawBlock[14]);

    lastDeviceStatus_ = rawStatus;
    lastSpadCount_ = static_cast<uint16_t>(rawSpads / 256U);
    lastSignalRateKcps_ = static_cast<uint16_t>(rawSignal * 8U);
    lastAmbientRateKcps_ = static_cast<uint16_t>(rawAmbient * 8U);
    lastSigmaMm_ = static_cast<uint16_t>(rawSigma / 4U);

    reading.distanceMm = distance;
    reading.rawValue = distance;
    reading.status = mapStatus(rawStatus, distance);

    clearInterrupt();
    lastReading_ = reading;
    return reading;
  }

  bool readIfReady(RangeReading& reading) {
    if (!dataReady()) {
      return false;
    }
    reading = read();
    return reading.status != SensorStatus::NotReady;
  }

  bool attachDataReadyPin(uint8_t pin, uint8_t mode = DF_IRQ_FALLING) {
    intPin_ = pin;
    pinMode(intPin_, INPUT_PULLUP);
    s_irqTarget_ = this;
    attachGpioInterrupt(intPin_, VL53L4CDSensor::irqHandler, irqLine_, mode);
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

  uint16_t millimeters() {
    return read().distanceMm;
  }

  bool setOffset(int16_t offsetMm) {
    uint16_t encoded = static_cast<uint16_t>(offsetMm * 4);
    bool ok = writeReg16(RANGE_OFFSET_MM, encoded);
    ok = writeReg16(INNER_OFFSET_MM, 0x0000) && ok;
    ok = writeReg16(OUTER_OFFSET_MM, 0x0000) && ok;
    if (ok) {
      offsetMm_ = offsetMm;
    }
    return ok;
  }

  int16_t offset() const {
    return offsetMm_;
  }

  bool setXtalk(uint16_t xtalkKcps) {
    bool ok = writeReg16(XTALK_X_PLANE_GRADIENT_KCPS, 0x0000);
    ok = writeReg16(XTALK_Y_PLANE_GRADIENT_KCPS, 0x0000) && ok;
    ok = writeReg16(XTALK_PLANE_OFFSET_KCPS, static_cast<uint16_t>((xtalkKcps << 9U) / 1000U)) && ok;
    if (ok) {
      xtalkKcps_ = xtalkKcps;
    }
    return ok;
  }

  uint16_t xtalk() const {
    return xtalkKcps_;
  }

  bool setSignalThreshold(uint16_t signalKcps) {
    signalThresholdKcps_ = signalKcps;
    return writeReg16(MIN_COUNT_RATE_RTN_LIMIT_MCPS, static_cast<uint16_t>(signalKcps << 7U));
  }

  uint16_t signalThreshold() const {
    return signalThresholdKcps_;
  }

  bool setSigmaThreshold(uint16_t sigmaMm) {
    sigmaThresholdMm_ = sigmaMm;
    return writeReg16(RANGE_CONFIG_SIGMA_THRESH, static_cast<uint16_t>(sigmaMm << 2U));
  }

  uint16_t sigmaThreshold() const {
    return sigmaThresholdMm_;
  }

  bool startTemperatureUpdate() {
    if (!initialized_) {
      return false;
    }
    writeReg(VHV_CONFIG_TIMEOUT_MACROP_LOOP_BOUND, 0x81);
    writeReg(0x000B, 0x92);
    writeReg(SYSTEM_START, 0x40);
    if (!waitForDataReady()) {
      return false;
    }
    clearInterrupt();
    writeReg(VHV_CONFIG_TIMEOUT_MACROP_LOOP_BOUND, 0x09);
    writeReg(0x000B, 0x00);
    return true;
  }

  uint16_t lastSignalRateKcps() const {
    return lastSignalRateKcps_;
  }

  uint16_t lastAmbientRateKcps() const {
    return lastAmbientRateKcps_;
  }

  uint16_t lastSigmaMm() const {
    return lastSigmaMm_;
  }

  uint16_t lastSpadCount() const {
    return lastSpadCount_;
  }

  uint8_t deviceStatus() const {
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
    info.deviceId = initialized_ ? 0xEB : 0x00;
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

 private:
  static void irqHandler() {
    if (s_irqTarget_ != nullptr) {
      gpioInterruptIsr(&s_irqTarget_->irqLine_);
    }
  }

  static VL53L4CDSensor* s_irqTarget_;

  static constexpr uint8_t kUnassignedPin = 0xFF;
  static constexpr uint8_t kDefaultConfig[] = {
      0x12, 0x00, 0x00, 0x11, 0x02, 0x00, 0x02, 0x08, 0x00, 0x08, 0x10, 0x01, 0x01, 0x00, 0x00, 0x00,
      0x00, 0xFF, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x0B, 0x00, 0x00, 0x02, 0x14, 0x21,
      0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0xC8, 0x00, 0x00, 0x38, 0xFF, 0x01, 0x00, 0x08, 0x00,
      0x00, 0x01, 0xCC, 0x07, 0x01, 0xF1, 0x05, 0x00, 0xA0, 0x00, 0x80, 0x08, 0x38, 0x00, 0x00, 0x00,
      0x00, 0x0F, 0x89, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x07, 0x05, 0x06, 0x06, 0x00,
      0x00, 0x02, 0xC7, 0xFF, 0x9B, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00};

  enum Reg : uint16_t {
    SOFT_RESET = 0x0000,
    I2C_SLAVE_DEVICE_ADDRESS = 0x0001,
    VHV_CONFIG_TIMEOUT_MACROP_LOOP_BOUND = 0x0008,
    XTALK_PLANE_OFFSET_KCPS = 0x0016,
    XTALK_X_PLANE_GRADIENT_KCPS = 0x0018,
    XTALK_Y_PLANE_GRADIENT_KCPS = 0x001A,
    RANGE_OFFSET_MM = 0x001E,
    INNER_OFFSET_MM = 0x0020,
    OUTER_OFFSET_MM = 0x0022,
    GPIO_HV_MUX_CTRL = 0x0030,
    GPIO_TIO_HV_STATUS = 0x0031,
    SYSTEM_INTERRUPT = 0x0046,
    RANGE_CONFIG_A = 0x005E,
    RANGE_CONFIG_B = 0x0061,
    RANGE_CONFIG_SIGMA_THRESH = 0x0064,
    MIN_COUNT_RATE_RTN_LIMIT_MCPS = 0x0066,
    INTERMEASUREMENT_MS = 0x006C,
    SYSTEM_INTERRUPT_CLEAR = 0x0086,
    SYSTEM_START = 0x0087,
    RESULT_RANGE_STATUS = 0x0089,
    RESULT_SPAD_NB = 0x008C,
    RESULT_SIGNAL_RATE = 0x008E,
    RESULT_AMBIENT_RATE = 0x0090,
    RESULT_SIGMA = 0x0092,
    RESULT_DISTANCE = 0x0096,
    RESULT_OSC_CALIBRATE_VAL = 0x00DE,
    FIRMWARE_SYSTEM_STATUS = 0x00E5,
    IDENTIFICATION_MODEL_ID = 0x010F,
    OSC_FREQUENCY = 0x0006,
  };

  bool initCore() {
    uint16_t sensorId = readReg16(IDENTIFICATION_MODEL_ID);
    if (sensorId != 0xEBAA) {
      return false;
    }

    writeReg(SOFT_RESET, 0x00);
    delay(1);
    writeReg(SOFT_RESET, 0x01);

    uint32_t start = millis();
    while (readReg(FIRMWARE_SYSTEM_STATUS) != 0x03U) {
      if (timeoutExpired(start)) {
        didTimeout_ = true;
        return false;
      }
      delay(1);
    }

    for (uint8_t offset = 0; offset < sizeof(kDefaultConfig); ++offset) {
      writeReg(static_cast<uint16_t>(0x002DU + offset), kDefaultConfig[offset]);
    }

    writeReg(SYSTEM_START, 0x40);
    if (!waitForDataReady()) {
      didTimeout_ = true;
      return false;
    }
    clearInterrupt();
    stopContinuous();
    writeReg(VHV_CONFIG_TIMEOUT_MACROP_LOOP_BOUND, 0x09);
    writeReg(0x000B, 0x00);
    writeReg16(0x0024, 0x0500);
    return setRangeTiming(timingBudgetMs_, interMeasurementMs_);
  }

  bool clearInterrupt() {
    return writeReg(SYSTEM_INTERRUPT_CLEAR, 0x01);
  }

  bool waitForDataReady() {
    uint32_t start = millis();
    while (!dataReady()) {
      if (timeoutExpired(start)) {
        didTimeout_ = true;
        return false;
      }
      delay(1);
    }
    return true;
  }

  SensorStatus mapStatus(uint8_t rawStatus, uint16_t distanceMm) const {
    if (rawStatus == 0) {
      return distanceMm == 0 ? SensorStatus::OutOfRange : SensorStatus::Ok;
    }
    if (rawStatus == 2 || rawStatus == 3 || rawStatus == 4) {
      return SensorStatus::OutOfRange;
    }
    return SensorStatus::Error;
  }

  static uint16_t encodeRangeTiming(uint32_t timingBudgetUs, uint32_t scaledMacroPeriodUs) {
    uint32_t lsByte = ((timingBudgetUs << 12U) + ((scaledMacroPeriodUs >> 6U) >> 1U)) / (scaledMacroPeriodUs >> 6U);
    if (lsByte > 0) {
      lsByte -= 1U;
    }
    uint16_t msByte = 0;
    while ((lsByte & 0xFFFFFF00UL) != 0UL) {
      lsByte >>= 1U;
      ++msByte;
    }
    return static_cast<uint16_t>((msByte << 8U) | (lsByte & 0xFFU));
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

  bool writeReg32(uint16_t reg, uint32_t value) {
    bus_->beginTransmission(address_);
    bus_->write(static_cast<uint8_t>(reg >> 8));
    bus_->write(static_cast<uint8_t>(reg));
    bus_->write(static_cast<uint8_t>(value >> 24));
    bus_->write(static_cast<uint8_t>(value >> 16));
    bus_->write(static_cast<uint8_t>(value >> 8));
    bus_->write(static_cast<uint8_t>(value));
    lastI2cStatus_ = bus_->endTransmission();
    return lastI2cStatus_ == 0;
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

  uint32_t readReg32(uint16_t reg) {
    uint32_t value = 0;
    detail::readReg32_16(bus_, address_, reg, value, &lastI2cStatus_);
    return value;
  }

  bool readMulti(uint16_t reg, uint8_t* buffer, uint8_t length) {
    bus_->beginTransmission(address_);
    bus_->write(static_cast<uint8_t>(reg >> 8));
    bus_->write(static_cast<uint8_t>(reg));
    lastI2cStatus_ = bus_->endTransmission(false);
    if (lastI2cStatus_ != 0) {
      return false;
    }
    uint8_t received = bus_->requestFrom(address_, length);
    if (received != length) {
      lastI2cStatus_ = 4;
      return false;
    }
    for (uint8_t index = 0; index < length; ++index) {
      buffer[index] = static_cast<uint8_t>(bus_->read());
    }
    return true;
  }

  TwoWire* bus_ = &Wire;
  uint8_t address_ = 0x29;
  uint8_t shutdownPin_ = kUnassignedPin;
  uint8_t lastI2cStatus_ = 0;
  uint8_t lastDeviceStatus_ = 0;
  uint16_t ioTimeoutMs_ = 1000;
  uint16_t lastSignalRateKcps_ = 0;
  uint16_t lastAmbientRateKcps_ = 0;
  uint16_t lastSigmaMm_ = 0;
  uint16_t lastSpadCount_ = 0;
  uint16_t xtalkKcps_ = 0;
  uint16_t signalThresholdKcps_ = 1024;
  uint16_t sigmaThresholdMm_ = 90;
  int16_t offsetMm_ = 0;
  uint32_t timingBudgetMs_ = 50;
  uint32_t interMeasurementMs_ = 0;
  bool shutdownActiveHigh_ = true;
  bool powered_ = true;
  bool initialized_ = false;
  bool ranging_ = false;
  bool didTimeout_ = false;
  RangeReading lastReading_;
  GpioInterruptLine irqLine_;
  uint8_t intPin_ = kUnassignedPin;
};

VL53L4CDSensor* VL53L4CDSensor::s_irqTarget_ = nullptr;

}  // namespace DiFinders