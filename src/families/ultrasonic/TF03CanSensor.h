#pragma once

#include <Arduino.h>

#include "../../core/DiFindersCan.h"
#include "../../core/DiFindersDiagnostics.h"
#include "../../core/DiFindersTypes.h"

namespace DiFinders {

/**
 * Benewake TF03 / TF03-180 in CAN output mode.
 *
 * Measurement frames (default TX ID 0x03, 11-bit standard):
 *   Byte0-1 distance in cm (little-endian), Byte2-3 signal strength, Byte4-5 = 0.
 * Configure the module for CAN (not UART) with Benewake BW_TFDS or serial commands
 * before using this driver. See docs/USER_GUIDE.md §7 (CAN bus).
 */
class TF03CanSensor {
 public:
  static constexpr uint32_t kDefaultTransmitId = 0x03U;
  static constexpr uint32_t kDefaultHostReceiveId = 0x3003U;

  TF03CanSensor() = default;

  bool ready() const {
    return true;
  }

  void setTransmitId(uint32_t id) {
    transmitId_ = id;
  }

  uint32_t transmitId() const {
    return transmitId_;
  }

  void setMinStrength(uint16_t minStrength) {
    minStrength_ = minStrength;
  }

  /** Feed one CAN frame from your MCP2515 / TWAI / MCP2517 stack. */
  bool onCanFrame(const CanFrame& frame) {
    if (frame.extended || frame.id != transmitId_) {
      return false;
    }
    return parseMeasurement(frame.data, frame.dlc);
  }

  bool onCanFrame(uint32_t id, const uint8_t* data, uint8_t dlc, bool extended = false) {
    CanFrame frame;
    if (!canFrameFromRaw(frame, id, data, dlc, extended)) {
      return false;
    }
    return onCanFrame(frame);
  }

  RangeReading read() const {
    return lastReading_;
  }

  uint16_t lastStrength() const {
    return lastStrength_;
  }

  uint32_t framesAccepted() const {
    return framesAccepted_;
  }

  uint32_t framesRejected() const {
    return framesRejected_;
  }

  SensorProbe probe() const {
    SensorProbe info;
    info.kind = SensorKind::Range;
    info.busType = SensorBusType::Can;
    info.configured = true;
    info.supportsContinuousMode = true;
    info.deviceId = static_cast<uint8_t>(transmitId_ & 0xFFU);
    return info;
  }

  SelfTestReport selfTest() {
    SelfTestReport report;
    report.status = lastReading_.status;
    report.passed = lastReading_.status == SensorStatus::Ok || lastReading_.status == SensorStatus::OutOfRange;
    report.value = lastReading_.distanceMm;
    report.detail = static_cast<uint8_t>(lastStrength_ > 0 ? 1 : 0);
    return report;
  }

  uint8_t lastBusError() const {
    return 0;
  }

  SensorHealth health() const {
    return makeHealth(true, SensorBusType::Can, lastReading_.status, 0, lastReading_.distanceMm, framesRejected_);
  }

 private:
  bool parseMeasurement(const uint8_t* data, uint8_t dlc) {
    if (dlc < 4) {
      ++framesRejected_;
      return false;
    }

    /* Benewake CAN distance payload (see husarion/tf03_driver, TF03 manual §4.2). */
    const uint16_t distanceCm = static_cast<uint16_t>(data[0] | (static_cast<uint16_t>(data[1]) << 8));
    const uint16_t strength = static_cast<uint16_t>(data[2] | (static_cast<uint16_t>(data[3]) << 8));

    RangeReading reading;
    reading.timestampMs = millis();
    reading.distanceMm = static_cast<uint16_t>(distanceCm * 10U);
    reading.rawValue = distanceCm;
    lastStrength_ = strength;

    if (distanceCm == 0) {
      reading.status = SensorStatus::OutOfRange;
    } else if (minStrength_ > 0 && strength < minStrength_) {
      reading.status = SensorStatus::OutOfRange;
    } else {
      reading.status = SensorStatus::Ok;
    }

    lastReading_ = reading;
    ++framesAccepted_;
    return true;
  }

  uint32_t transmitId_ = kDefaultTransmitId;
  uint16_t minStrength_ = 0;
  uint16_t lastStrength_ = 0;
  uint32_t framesAccepted_ = 0;
  uint32_t framesRejected_ = 0;
  RangeReading lastReading_;
};

}  // namespace DiFinders
