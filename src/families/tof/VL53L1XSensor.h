#pragma once

#include <Arduino.h>
#include <Wire.h>

#include "../../core/DiFindersDebug.h"
#include "../../core/DiFindersGpioInterrupt.h"
#include "../../core/DiFindersI2C.h"
#include "../../core/DiFindersTypes.h"

namespace DiFinders {

class VL53L1XSensor {
 public:
  enum class DistanceMode : uint8_t {
    Short,
    Medium,
    Long,
    Unknown,
  };

  VL53L1XSensor() = default;
  explicit VL53L1XSensor(uint8_t address) : address_(address) {}

  bool begin(TwoWire& wire = Wire, uint8_t address = 0x29, bool io2v8 = true) {
    bus_ = &wire;
    address_ = address;
    io2v8_ = io2v8;
    initialized_ = initCore();
    if (!initialized_) {
      return false;
    }

    setROISize(roiWidth_, roiHeight_);
    setROICenter(roiCenter_);
    setDistanceMode(distanceMode_);
    setMeasurementTimingBudget(measurementTimingBudgetUs_);
    clearInterrupt();
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
      delay(3);
    }
    powered_ = true;
  }

  void powerOff() {
    if (shutdownPin_ != kUnassignedPin) {
      digitalWrite(shutdownPin_, shutdownActiveHigh_ ? LOW : HIGH);
      delay(3);
    }
    powered_ = false;
    initialized_ = false;
    continuousMode_ = false;
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

  bool setDistanceMode(DistanceMode mode) {
    if (!initialized_) {
      distanceMode_ = mode;
      return true;
    }

    switch (mode) {
      case DistanceMode::Short:
        writeReg(PHASECAL_CONFIG__TIMEOUT_MACROP, 0x14);
        writeReg(RANGE_CONFIG__VCSEL_PERIOD_A, 0x07);
        writeReg(RANGE_CONFIG__VCSEL_PERIOD_B, 0x05);
        writeReg(RANGE_CONFIG__VALID_PHASE_HIGH, 0x38);
        writeReg(SD_CONFIG__WOI_SD0, 0x07);
        writeReg(SD_CONFIG__WOI_SD1, 0x05);
        writeReg(SD_CONFIG__INITIAL_PHASE_SD0, 6);
        writeReg(SD_CONFIG__INITIAL_PHASE_SD1, 6);
        break;

      case DistanceMode::Medium:
        writeReg(PHASECAL_CONFIG__TIMEOUT_MACROP, 0x0A);
        writeReg(RANGE_CONFIG__VCSEL_PERIOD_A, 0x0B);
        writeReg(RANGE_CONFIG__VCSEL_PERIOD_B, 0x09);
        writeReg(RANGE_CONFIG__VALID_PHASE_HIGH, 0x78);
        writeReg(SD_CONFIG__WOI_SD0, 0x0B);
        writeReg(SD_CONFIG__WOI_SD1, 0x09);
        writeReg(SD_CONFIG__INITIAL_PHASE_SD0, 10);
        writeReg(SD_CONFIG__INITIAL_PHASE_SD1, 10);
        break;

      case DistanceMode::Long:
        writeReg(PHASECAL_CONFIG__TIMEOUT_MACROP, 0x0A);
        writeReg(RANGE_CONFIG__VCSEL_PERIOD_A, 0x0F);
        writeReg(RANGE_CONFIG__VCSEL_PERIOD_B, 0x0D);
        writeReg(RANGE_CONFIG__VALID_PHASE_HIGH, 0xB8);
        writeReg(SD_CONFIG__WOI_SD0, 0x0F);
        writeReg(SD_CONFIG__WOI_SD1, 0x0D);
        writeReg(SD_CONFIG__INITIAL_PHASE_SD0, 14);
        writeReg(SD_CONFIG__INITIAL_PHASE_SD1, 14);
        break;

      default:
        return false;
    }

    distanceMode_ = mode;
    return setMeasurementTimingBudget(measurementTimingBudgetUs_);
  }

  DistanceMode distanceMode() const {
    return distanceMode_;
  }

  bool useShortDistanceMode() {
    return setDistanceMode(DistanceMode::Short);
  }

  bool useMediumDistanceMode() {
    return setDistanceMode(DistanceMode::Medium);
  }

  bool useLongDistanceMode() {
    return setDistanceMode(DistanceMode::Long);
  }

  bool setMeasurementTimingBudget(uint32_t budgetUs) {
    measurementTimingBudgetUs_ = budgetUs;
    if (!initialized_) {
      return true;
    }
    if (budgetUs <= kTimingGuardUs || budgetUs > 1100000UL) {
      return false;
    }

    uint32_t rangeTimeoutUs = (budgetUs - kTimingGuardUs) / 2UL;
    uint32_t macroPeriodUs = calcMacroPeriod(readReg(RANGE_CONFIG__VCSEL_PERIOD_A));
    uint32_t phasecalTimeoutMclks = timeoutMicrosecondsToMclks(1000U, macroPeriodUs);
    if (phasecalTimeoutMclks > 0xFFU) {
      phasecalTimeoutMclks = 0xFFU;
    }

    writeReg(PHASECAL_CONFIG__TIMEOUT_MACROP, static_cast<uint8_t>(phasecalTimeoutMclks));
    writeReg16(MM_CONFIG__TIMEOUT_MACROP_A, encodeTimeout(timeoutMicrosecondsToMclks(1U, macroPeriodUs)));
    writeReg16(RANGE_CONFIG__TIMEOUT_MACROP_A, encodeTimeout(timeoutMicrosecondsToMclks(rangeTimeoutUs, macroPeriodUs)));

    macroPeriodUs = calcMacroPeriod(readReg(RANGE_CONFIG__VCSEL_PERIOD_B));
    writeReg16(MM_CONFIG__TIMEOUT_MACROP_B, encodeTimeout(timeoutMicrosecondsToMclks(1U, macroPeriodUs)));
    writeReg16(RANGE_CONFIG__TIMEOUT_MACROP_B, encodeTimeout(timeoutMicrosecondsToMclks(rangeTimeoutUs, macroPeriodUs)));
    return true;
  }

  uint32_t measurementTimingBudget() const {
    return measurementTimingBudgetUs_;
  }

  void setROISize(uint8_t width, uint8_t height) {
    if (width < 4) {
      width = 4;
    }
    if (height < 4) {
      height = 4;
    }
    if (width > 16) {
      width = 16;
    }
    if (height > 16) {
      height = 16;
    }
    roiWidth_ = width;
    roiHeight_ = height;
    if (initialized_) {
      if (width > 10 || height > 10) {
        setROICenter(199);
      }
      writeReg(ROI_CONFIG__USER_ROI_REQUESTED_GLOBAL_XY_SIZE,
               static_cast<uint8_t>(((height - 1U) << 4U) | (width - 1U)));
    }
  }

  void getROISize(uint8_t& width, uint8_t& height) const {
    width = roiWidth_;
    height = roiHeight_;
  }

  void setROICenter(uint8_t spadNumber) {
    roiCenter_ = spadNumber;
    if (initialized_) {
      writeReg(ROI_CONFIG__USER_ROI_CENTRE_SPAD, spadNumber);
    }
  }

  uint8_t roiCenter() const {
    return roiCenter_;
  }

  /**
   * Reception / beam window on the 16x16 SPAD grid (ST ROI).
   * This narrows which SPADs contribute to ONE range reading — not per-cell 8x8 multizone.
   * spadWidth/spadHeight: 4..16. centerSpad: index on ST SPAD map (199 = default centre).
   */
  void setBeamWindow(uint8_t spadWidth, uint8_t spadHeight, uint8_t centerSpad = 199) {
    setROISize(spadWidth, spadHeight);
    setROICenter(centerSpad);
  }

  void useFullBeamWindow() {
    setBeamWindow(16, 16, 199);
  }

  /** Map a cell on the 16x16 SPAD grid to centre SPAD index (see ST UM4598 SPAD map). */
  static uint8_t spadCenterFromGrid(uint8_t gridX, uint8_t gridY) {
    if (gridX > 15) {
      gridX = 15;
    }
    if (gridY > 15) {
      gridY = 15;
    }
    return static_cast<uint8_t>((static_cast<uint16_t>(gridY) * 16U) + gridX);
  }

  void startContinuous(uint32_t periodMs = 50) {
    if (!initialized_) {
      return;
    }
    uint32_t minimumPeriodMs = (measurementTimingBudgetUs_ + 999U) / 1000U;
    if (periodMs == 0) {
      periodMs = minimumPeriodMs;
    }
    if (periodMs < minimumPeriodMs) {
      periodMs = minimumPeriodMs;
    }
    writeReg32(SYSTEM__INTERMEASUREMENT_PERIOD, periodMs * oscCalibrateVal_);
    clearInterrupt();
    writeReg(SYSTEM__MODE_START, 0x40);
    continuousMode_ = true;
  }

  void stopContinuous() {
    if (!initialized_) {
      return;
    }
    writeReg(SYSTEM__MODE_START, 0x80);
    writeReg(PHASECAL_CONFIG__OVERRIDE, 0x00);
    continuousMode_ = false;
  }

  bool dataReady() {
    return initialized_ && (readReg(GPIO__TIO_HV_STATUS) & 0x01U) == 0;
  }

  RangeReading read(bool blocking = true) {
    if (!continuousMode_) {
      return readSingle(blocking);
    }
    return readMeasurement(blocking, false);
  }

  bool readIfReady(RangeReading& reading) {
    if (!dataReady()) {
      return false;
    }
    reading = read(false);
    return reading.status != SensorStatus::NotReady;
  }

  /** Wire module INT/GPIO1 to an MCU pin; use with startContinuous() + readIfReady(). */
  bool attachDataReadyPin(uint8_t pin, uint8_t mode = DF_IRQ_FALLING) {
    intPin_ = pin;
    pinMode(intPin_, INPUT_PULLUP);
    s_irqTarget_ = this;
    attachGpioInterrupt(intPin_, VL53L1XSensor::irqHandler, irqLine_, mode);
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

  /** ST distance window (mm). windowMode: 0=below, 1=above, 2=out, 3=in (default 2 = outside band alert). */
  bool setDistanceWindowMm(uint16_t innerMm, uint16_t outerMm, uint8_t windowMode = 2) {
    if (!initialized_) {
      return false;
    }
    writeReg16(SYSTEM__DISTANCE_THRESH_LOW, innerMm);
    writeReg16(SYSTEM__DISTANCE_THRESH_HIGH, outerMm);
    uint8_t gpioCfg = static_cast<uint8_t>((readReg(SYSTEM__INTERRUPT_CONFIG_GPIO) & 0xF0U) | (windowMode & 0x0FU));
    writeReg(SYSTEM__INTERRUPT_CONFIG_GPIO, gpioCfg);
    return true;
  }

  bool isOutsideDistanceWindow(uint16_t distanceMm, uint16_t innerMm, uint16_t outerMm) const {
    return distanceMm < innerMm || distanceMm > outerMm;
  }

  RangeReading readSingle(bool blocking = true) {
    if (!initialized_) {
      RangeReading reading;
      reading.timestampMs = millis();
      reading.status = SensorStatus::NotReady;
      lastReading_ = reading;
      return reading;
    }
    clearInterrupt();
    writeReg(SYSTEM__MODE_START, 0x10);
    return readMeasurement(blocking, true);
  }

  uint16_t millimeters() {
    return read().distanceMm;
  }

  uint16_t lastAmbientRateMcps() const {
    return lastAmbientRateMcps_;
  }

  uint16_t lastSignalRateMcps() const {
    return lastSignalRateMcps_;
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
    info.supportsBeamRoi = true;
    info.secondaryPin = shutdownPin_;
    info.address = address_;
    info.deviceId = initialized_ ? 0xEA : 0x00;
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

  static VL53L1XSensor* s_irqTarget_;

  static constexpr uint8_t kUnassignedPin = 0xFF;
  static constexpr uint32_t kTimingGuardUs = 4528UL;

  enum Reg : uint16_t {
    SOFT_RESET = 0x0000,
    I2C_SLAVE__DEVICE_ADDRESS = 0x0001,
    OSC_MEASURED__FAST_OSC__FREQUENCY = 0x0006,
    MM_CONFIG__TIMEOUT_MACROP_A = 0x005A,
    MM_CONFIG__TIMEOUT_MACROP_B = 0x005C,
    RANGE_CONFIG__TIMEOUT_MACROP_A = 0x005E,
    RANGE_CONFIG__VCSEL_PERIOD_A = 0x0060,
    RANGE_CONFIG__TIMEOUT_MACROP_B = 0x0061,
    RANGE_CONFIG__VCSEL_PERIOD_B = 0x0063,
    RANGE_CONFIG__VALID_PHASE_HIGH = 0x0069,
    SYSTEM__INTERMEASUREMENT_PERIOD = 0x006C,
    SD_CONFIG__WOI_SD0 = 0x0078,
    SD_CONFIG__WOI_SD1 = 0x0079,
    SD_CONFIG__INITIAL_PHASE_SD0 = 0x007A,
    SD_CONFIG__INITIAL_PHASE_SD1 = 0x007B,
    ROI_CONFIG__USER_ROI_CENTRE_SPAD = 0x007F,
    ROI_CONFIG__USER_ROI_REQUESTED_GLOBAL_XY_SIZE = 0x0080,
    SYSTEM__SEQUENCE_CONFIG = 0x0081,
    SYSTEM__GROUPED_PARAMETER_HOLD = 0x0082,
    FIRMWARE__ENABLE = 0x0085,
    SYSTEM__INTERRUPT_CLEAR = 0x0086,
    SYSTEM__MODE_START = 0x0087,
    RESULT__RANGE_STATUS = 0x0089,
    PHASECAL_RESULT__REFERENCE_PHASE = 0x00D6,
    PHASECAL_RESULT__VCSEL_START = 0x00D8,
    RESULT__OSC_CALIBRATE_VAL = 0x00DE,
    FIRMWARE__SYSTEM_STATUS = 0x00E5,
    GPH__SYSTEM__INTERRUPT_CONFIG_GPIO = 0x0F28,
    GPH__RANGE_CONFIG__VCSEL_PERIOD_A = 0x0F3C,
    GPH__RANGE_CONFIG__VCSEL_PERIOD_B = 0x0F3D,
    GPH__RANGE_CONFIG__VALID_PHASE_HIGH = 0x0F45,
    PAD_I2C_HV__EXTSUP_CONFIG = 0x002E,
    GPIO__TIO_HV_STATUS = 0x0031,
    SIGMA_ESTIMATOR__EFFECTIVE_PULSE_WIDTH_NS = 0x0036,
    SIGMA_ESTIMATOR__EFFECTIVE_AMBIENT_WIDTH_NS = 0x0037,
    SYSTEM__INTERRUPT_CONFIG_GPIO = 0x0046,
    SYSTEM__DISTANCE_THRESH_HIGH = 0x0072,
    SYSTEM__DISTANCE_THRESH_LOW = 0x0074,
    PHASECAL_CONFIG__TIMEOUT_MACROP = 0x004B,
    PHASECAL_CONFIG__OVERRIDE = 0x004D,
    SYSTEM__THRESH_RATE_HIGH = 0x0050,
    SYSTEM__THRESH_RATE_LOW = 0x0052,
    DSS_CONFIG__APERTURE_ATTENUATION = 0x0057,
    IDENTIFICATION__MODEL_ID = 0x010F,
  };

  bool initCore() {
    uint16_t modelId = readReg16(IDENTIFICATION__MODEL_ID);
    if (modelId != 0xEACC) {
      return false;
    }

    writeReg(SOFT_RESET, 0x00);
    delayMicroseconds(100);
    writeReg(SOFT_RESET, 0x01);
    delay(1);

    uint32_t start = millis();
    while ((readReg(FIRMWARE__SYSTEM_STATUS) & 0x01U) == 0U) {
      if (timeoutExpired(start)) {
        didTimeout_ = true;
        return false;
      }
    }

    if (io2v8_) {
      writeReg(PAD_I2C_HV__EXTSUP_CONFIG, static_cast<uint8_t>(readReg(PAD_I2C_HV__EXTSUP_CONFIG) | 0x01U));
    }

    fastOscFrequency_ = readReg16(OSC_MEASURED__FAST_OSC__FREQUENCY);
    if (fastOscFrequency_ == 0) {
      fastOscFrequency_ = 0x0B00;
    }

    oscCalibrateVal_ = readReg16(RESULT__OSC_CALIBRATE_VAL) & 0x03FFU;
    if (oscCalibrateVal_ == 0) {
      oscCalibrateVal_ = 1;
    }

    writeReg(GPIO__TIO_HV_STATUS, 0x02);
    writeReg(SIGMA_ESTIMATOR__EFFECTIVE_PULSE_WIDTH_NS, 8);
    writeReg(SIGMA_ESTIMATOR__EFFECTIVE_AMBIENT_WIDTH_NS, 16);
    writeReg16(SYSTEM__THRESH_RATE_HIGH, 0x0000);
    writeReg16(SYSTEM__THRESH_RATE_LOW, 0x0000);
    writeReg(DSS_CONFIG__APERTURE_ATTENUATION, 0x38);
    writeReg(SYSTEM__INTERRUPT_CONFIG_GPIO, 0x20);
    writeReg(SYSTEM__GROUPED_PARAMETER_HOLD, 0x00);
    writeReg(SYSTEM__SEQUENCE_CONFIG, 0x8B);
    clearInterrupt();
    return lastI2cStatus_ == 0;
  }

  RangeReading readMeasurement(bool blocking, bool singleShot) {
    RangeReading reading;
    reading.timestampMs = millis();

    if (!initialized_) {
      reading.status = SensorStatus::NotReady;
      lastReading_ = reading;
      return reading;
    }

    if (blocking) {
      uint32_t start = millis();
      while (!dataReady()) {
        if (timeoutExpired(start)) {
          didTimeout_ = true;
          reading.status = SensorStatus::Timeout;
          lastReading_ = reading;
          return reading;
        }
        delayMicroseconds(150);
      }
    } else if (!dataReady()) {
      reading.status = SensorStatus::NotReady;
      lastReading_ = reading;
      return reading;
    }

    uint8_t buffer[17] = {0};
    if (!readMulti(RESULT__RANGE_STATUS, buffer, sizeof(buffer))) {
      reading.status = SensorStatus::Error;
      lastReading_ = reading;
      return reading;
    }

    lastDeviceStatus_ = buffer[0];
    lastAmbientRateMcps_ = static_cast<uint16_t>((static_cast<uint16_t>(buffer[7]) << 8) | buffer[8]);
    uint16_t distance = static_cast<uint16_t>((static_cast<uint16_t>(buffer[13]) << 8) | buffer[14]);
    lastSignalRateMcps_ = static_cast<uint16_t>((static_cast<uint16_t>(buffer[15]) << 8) | buffer[16]);

    reading.distanceMm = distance;
    reading.rawValue = distance;
    reading.status = mapRangeStatus(lastDeviceStatus_, distance);
    clearInterrupt();

    if (singleShot) {
      continuousMode_ = false;
    }

    lastReading_ = reading;
    return reading;
  }

  SensorStatus mapRangeStatus(uint8_t rawStatus, uint16_t distanceMm) const {
    switch (rawStatus) {
      case 0:
        return distanceMm == 0 ? SensorStatus::OutOfRange : SensorStatus::Ok;
      case 1:
      case 2:
      case 3:
      case 4:
        return SensorStatus::OutOfRange;
      default:
        return SensorStatus::Error;
    }
  }

  bool clearInterrupt() {
    return writeReg(SYSTEM__INTERRUPT_CLEAR, 0x01);
  }

  static uint32_t decodeTimeout(uint16_t regValue) {
    return ((static_cast<uint32_t>(regValue & 0x00FFU)) << (regValue >> 8U)) + 1U;
  }

  static uint16_t encodeTimeout(uint32_t timeoutMclks) {
    uint32_t lsByte = 0;
    uint16_t msByte = 0;
    if (timeoutMclks == 0) {
      return 0;
    }
    lsByte = timeoutMclks - 1U;
    while ((lsByte & 0xFFFFFF00UL) != 0UL) {
      lsByte >>= 1U;
      ++msByte;
    }
    return static_cast<uint16_t>((msByte << 8U) | (lsByte & 0xFFU));
  }

  static uint32_t timeoutMclksToMicroseconds(uint32_t timeoutMclks, uint32_t macroPeriodUs) {
    return static_cast<uint32_t>(((static_cast<uint64_t>(timeoutMclks) * macroPeriodUs) + 0x800ULL) >> 12U);
  }

  static uint32_t timeoutMicrosecondsToMclks(uint32_t timeoutUs, uint32_t macroPeriodUs) {
    return (((timeoutUs << 12U) + (macroPeriodUs >> 1U)) / macroPeriodUs);
  }

  uint32_t calcMacroPeriod(uint8_t vcselPeriod) const {
    uint32_t pllPeriodUs = (static_cast<uint32_t>(1UL) << 30U) / fastOscFrequency_;
    uint8_t vcselPeriodPclks = static_cast<uint8_t>((vcselPeriod + 1U) << 1U);
    uint32_t macroPeriodUs = 2304UL * pllPeriodUs;
    macroPeriodUs >>= 6U;
    macroPeriodUs *= vcselPeriodPclks;
    macroPeriodUs >>= 6U;
    return macroPeriodUs;
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
  uint8_t roiWidth_ = 16;
  uint8_t roiHeight_ = 16;
  uint8_t roiCenter_ = 199;
  uint16_t ioTimeoutMs_ = 500;
  uint16_t fastOscFrequency_ = 0x0B00;
  uint16_t oscCalibrateVal_ = 1;
  uint16_t lastAmbientRateMcps_ = 0;
  uint16_t lastSignalRateMcps_ = 0;
  uint32_t measurementTimingBudgetUs_ = 50000UL;
  bool io2v8_ = true;
  bool shutdownActiveHigh_ = true;
  bool powered_ = true;
  bool initialized_ = false;
  bool continuousMode_ = false;
  bool didTimeout_ = false;
  DistanceMode distanceMode_ = DistanceMode::Long;
  RangeReading lastReading_;
  GpioInterruptLine irqLine_;
  uint8_t intPin_ = kUnassignedPin;
};

VL53L1XSensor* VL53L1XSensor::s_irqTarget_ = nullptr;

}  // namespace DiFinders