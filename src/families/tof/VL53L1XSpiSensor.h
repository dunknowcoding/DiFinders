#pragma once

#include <Arduino.h>
#include <SPI.h>

#include "../../core/DiFindersDebug.h"
#include "../../core/DiFindersGpioInterrupt.h"
#include "../../core/DiFindersDma.h"
#include "../../core/DiFindersSpi.h"
#include "../../core/DiFindersTypes.h"

namespace DiFinders {

/**
 * VL53L1X on SPI (e.g. VL53L1CB breakout). I2C-only boards use VL53L1XSensor.
 */
class VL53L1XSpiSensor {
 public:
  bool begin(uint8_t csPin, uint32_t hz = 2000000UL) {
    csPin_ = csPin;
    spiHz_ = hz;
    pinMode(csPin_, OUTPUT);
    digitalWrite(csPin_, HIGH);
    SPI.begin();
    initialized_ = initCore();
    if (initialized_) {
      applyLongDistanceMode();
      setMeasurementTimingBudget(50000UL);
      clearInterrupt();
    }
    return initialized_;
  }

  bool ready() const {
    return initialized_;
  }

  void setTimeout(uint16_t timeoutMs) {
    ioTimeoutMs_ = timeoutMs;
  }

  void startContinuous(uint32_t periodMs = 60) {
    if (!initialized_) {
      return;
    }
    uint32_t minimumPeriodMs = (measurementTimingBudgetUs_ + 999U) / 1000U;
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
    continuousMode_ = false;
  }

  bool dataReady() {
    return initialized_ && (readReg(GPIO__TIO_HV_STATUS) & 0x01U) == 0;
  }

  RangeReading read(bool blocking = true) {
    if (!continuousMode_) {
      clearInterrupt();
      writeReg(SYSTEM__MODE_START, 0x10);
      return readMeasurement(blocking, true);
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

  uint16_t millimeters() {
    return read().distanceMm;
  }

  bool attachDataReadyPin(uint8_t pin, uint8_t mode = DF_IRQ_FALLING) {
    intPin_ = pin;
    pinMode(intPin_, INPUT_PULLUP);
    s_irqTarget_ = this;
    attachGpioInterrupt(intPin_, VL53L1XSpiSensor::irqHandler, irqLine_, mode);
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

  SensorProbe probe() const {
    SensorProbe info;
    info.kind = SensorKind::Range;
    info.busType = SensorBusType::Spi;
    info.configured = initialized_;
    info.supportsContinuousMode = true;
    info.supportsInterruptReady = true;
    info.supportsDmaTransfer = dmaPlatformSupported() && busSupportsDma(SensorBusType::Spi);
    info.primaryPin = csPin_;
    info.secondaryPin = intPin_;
    info.deviceId = initialized_ ? 0xEA : 0x00;
    return info;
  }

  const RangeReading& lastReading() const {
    return lastReading_;
  }

 private:
  static void irqHandler() {
    if (s_irqTarget_ != nullptr) {
      gpioInterruptIsr(&s_irqTarget_->irqLine_);
    }
  }

  static VL53L1XSpiSensor* s_irqTarget_;

  static constexpr uint8_t kUnassignedPin = 0xFF;
  static constexpr uint32_t kTimingGuardUs = 4528UL;

  enum Reg : uint16_t {
    SOFT_RESET = 0x0000,
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
    SYSTEM__SEQUENCE_CONFIG = 0x0081,
    SYSTEM__GROUPED_PARAMETER_HOLD = 0x0082,
    SYSTEM__INTERRUPT_CLEAR = 0x0086,
    SYSTEM__MODE_START = 0x0087,
    RESULT__RANGE_STATUS = 0x0089,
    RESULT__OSC_CALIBRATE_VAL = 0x00DE,
    FIRMWARE__SYSTEM_STATUS = 0x00E5,
    PAD_I2C_HV__EXTSUP_CONFIG = 0x002E,
    GPIO__TIO_HV_STATUS = 0x0031,
    SIGMA_ESTIMATOR__EFFECTIVE_PULSE_WIDTH_NS = 0x0036,
    SIGMA_ESTIMATOR__EFFECTIVE_AMBIENT_WIDTH_NS = 0x0037,
    SYSTEM__INTERRUPT_CONFIG_GPIO = 0x0046,
    PHASECAL_CONFIG__TIMEOUT_MACROP = 0x004B,
    SYSTEM__THRESH_RATE_HIGH = 0x0050,
    SYSTEM__THRESH_RATE_LOW = 0x0052,
    DSS_CONFIG__APERTURE_ATTENUATION = 0x0057,
    IDENTIFICATION__MODEL_ID = 0x010F,
  };

  bool initCore() {
    if (readReg16(IDENTIFICATION__MODEL_ID) != 0xEACC) {
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

    writeReg(PAD_I2C_HV__EXTSUP_CONFIG, static_cast<uint8_t>(readReg(PAD_I2C_HV__EXTSUP_CONFIG) | 0x01U));
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
    return true;
  }

  void applyLongDistanceMode() {
    writeReg(PHASECAL_CONFIG__TIMEOUT_MACROP, 0x09);
    writeReg(RANGE_CONFIG__VCSEL_PERIOD_A, 0x0F);
    writeReg(RANGE_CONFIG__VCSEL_PERIOD_B, 0x0D);
    writeReg(RANGE_CONFIG__VALID_PHASE_HIGH, 0xC8);
    writeReg(SD_CONFIG__WOI_SD0, 0x0F);
    writeReg(SD_CONFIG__WOI_SD1, 0x0D);
    writeReg(SD_CONFIG__INITIAL_PHASE_SD0, 14);
    writeReg(SD_CONFIG__INITIAL_PHASE_SD1, 10);
  }

  bool setMeasurementTimingBudget(uint32_t budgetUs) {
    measurementTimingBudgetUs_ = budgetUs;
    if (!initialized_ || budgetUs <= kTimingGuardUs || budgetUs > 1100000UL) {
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

    uint8_t rawStatus = buffer[0];
    uint16_t distance = static_cast<uint16_t>((static_cast<uint16_t>(buffer[13]) << 8) | buffer[14]);
    reading.distanceMm = distance;
    reading.rawValue = distance;
    reading.status = mapRangeStatus(rawStatus, distance);
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

  static uint32_t timeoutMicrosecondsToMclks(uint32_t timeoutUs, uint32_t macroPeriodUs) {
    return (((timeoutUs << 12U) + (macroPeriodUs >> 1U)) / macroPeriodUs);
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
    return detail::spiWriteReg8(csPin_, reg, value, spiHz_);
  }

  bool writeReg16(uint16_t reg, uint16_t value) {
    return detail::spiWriteReg16(csPin_, reg, value, spiHz_);
  }

  bool writeReg32(uint16_t reg, uint32_t value) {
    SPI.beginTransaction(SPISettings(spiHz_, MSBFIRST, SPI_MODE0));
    digitalWrite(csPin_, LOW);
    SPI.transfer(static_cast<uint8_t>((reg >> 8) & 0x7FU));
    SPI.transfer(static_cast<uint8_t>(reg & 0xFFU));
    SPI.transfer(static_cast<uint8_t>(value >> 24));
    SPI.transfer(static_cast<uint8_t>(value >> 16));
    SPI.transfer(static_cast<uint8_t>(value >> 8));
    SPI.transfer(static_cast<uint8_t>(value));
    digitalWrite(csPin_, HIGH);
    SPI.endTransaction();
    return true;
  }

  uint8_t readReg(uint16_t reg) {
    uint8_t value = 0;
    detail::spiReadReg8(csPin_, reg, value, spiHz_);
    return value;
  }

  uint16_t readReg16(uint16_t reg) {
    uint16_t value = 0;
    detail::spiReadReg16(csPin_, reg, value, spiHz_);
    return value;
  }

  bool readMulti(uint16_t reg, uint8_t* buffer, uint8_t length) {
    return detail::spiReadMulti(csPin_, reg, buffer, length, spiHz_);
  }

  uint8_t csPin_ = kUnassignedPin;
  uint8_t intPin_ = kUnassignedPin;
  uint32_t spiHz_ = 2000000UL;
  uint16_t ioTimeoutMs_ = 500;
  uint16_t fastOscFrequency_ = 0x0B00;
  uint16_t oscCalibrateVal_ = 1;
  uint32_t measurementTimingBudgetUs_ = 50000UL;
  bool initialized_ = false;
  bool continuousMode_ = false;
  bool didTimeout_ = false;
  RangeReading lastReading_;
  GpioInterruptLine irqLine_;
};

VL53L1XSpiSensor* VL53L1XSpiSensor::s_irqTarget_ = nullptr;

}  // namespace DiFinders
