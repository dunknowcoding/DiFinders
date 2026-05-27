#pragma once

#include <Arduino.h>

#include "DiFindersTypes.h"

namespace DiFinders {

inline const char* sensorKindText(SensorKind kind) {
  switch (kind) {
    case SensorKind::Range:
      return "range";
    case SensorKind::Proximity:
      return "proximity";
    case SensorKind::Reflective:
      return "reflective";
    case SensorKind::Motion:
      return "motion";
    default:
      return "unknown";
  }
}

inline const char* statusText(SensorStatus status) {
  switch (status) {
    case SensorStatus::Ok:
      return "ok";
    case SensorStatus::Timeout:
      return "timeout";
    case SensorStatus::OutOfRange:
      return "out_of_range";
    case SensorStatus::NotReady:
      return "not_ready";
    case SensorStatus::Disabled:
      return "disabled";
    case SensorStatus::Error:
    default:
      return "error";
  }
}

inline const char* detectionText(DetectionState state) {
  return state == DetectionState::Active ? "active" : "inactive";
}

inline void printSensorProbe(Print& out, const char* name, const SensorProbe& probe) {
  out.print(name);
  out.print(": kind=");
  out.print(sensorKindText(probe.kind));
  out.print(" bus=");
  out.print(sensorBusTypeName(probe.busType));
  out.print(" configured=");
  out.print(probe.configured ? 1 : 0);
  out.print(" powered=");
  out.print(probe.powered ? 1 : 0);

  if (probe.primaryPin != 0xFF) {
    out.print(" pin1=");
    out.print(probe.primaryPin);
  }
  if (probe.secondaryPin != 0xFF) {
    out.print(" pin2=");
    out.print(probe.secondaryPin);
  }
  if (probe.address != 0x00) {
    out.print(" addr=0x");
    if (probe.address < 16) {
      out.print('0');
    }
    out.print(probe.address, HEX);
  }
  if (probe.deviceId != 0x00) {
    out.print(" id=0x");
    if (probe.deviceId < 16) {
      out.print('0');
    }
    out.print(probe.deviceId, HEX);
  }

  out.print(" caps=");
  int printed = 0;
  if (probe.supportsAddressChange) {
    out.print("addr");
    printed = 1;
  }
  if (probe.supportsContinuousMode) {
    if (printed != 0) {
      out.print(',');
    }
    out.print("cont");
    printed = 1;
  }
  if (probe.supportsInterruptReady) {
    if (printed != 0) {
      out.print(',');
    }
    out.print("irq-ready");
    printed = 1;
  }
  if (probe.supportsTemperatureReading) {
    if (printed != 0) {
      out.print(',');
    }
    out.print("temp");
    printed = 1;
  }
  if (probe.supportsThresholdConfig) {
    if (printed != 0) {
      out.print(',');
    }
    out.print("thresh");
    printed = 1;
  }
  if (probe.supportsBeamRoi) {
    if (printed != 0) {
      out.print(',');
    }
    out.print("beam-roi");
    printed = 1;
  }
  if (probe.supportsDmaTransfer) {
    if (printed != 0) {
      out.print(',');
    }
    out.print("dma");
    printed = 1;
  }
  if (printed == 0) {
    out.print("none");
  }
  out.println();
}

inline void printSelfTest(Print& out, const char* name, const SelfTestReport& report) {
  out.print(name);
  out.print(": pass=");
  out.print(report.passed ? 1 : 0);
  out.print(" status=");
  out.print(statusText(report.status));
  out.print(" value=");
  out.print(report.value);
  out.print(" detail=");
  out.println(report.detail);
}

}  // namespace DiFinders
