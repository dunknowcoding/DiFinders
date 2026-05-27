#pragma once

#include "LD2410RadarSensor.h"

namespace DiFinders {

// HLK-LD2412 uses the same F4/F3/F2/F1 report frames and FD/FC command frames as LD2410.
// Default UART baud is 115200 (LD2410 defaults to 256000 on many boards).
class LD2412RadarSensor : public LD2410RadarSensor {
 public:
  LD2412RadarSensor() = default;
  explicit LD2412RadarSensor(Stream& serial) : LD2410RadarSensor(serial) {}

  uint32_t defaultBaudRate() const {
    return 115200UL;
  }
};

}  // namespace DiFinders
