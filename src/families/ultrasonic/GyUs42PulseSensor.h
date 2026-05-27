#pragma once

#include "PulseUltrasonicSensor.h"

namespace DiFinders {

class GyUs42PulseSensor : public PulseUltrasonicSensor {
 public:
  GyUs42PulseSensor() = default;

  GyUs42PulseSensor(uint8_t triggerPin, uint8_t echoPin, uint8_t echoPinMode = INPUT)
      : PulseUltrasonicSensor(triggerPin, echoPin, echoPinMode) {
    setMinDistanceMm(200);
  }

  void begin(uint8_t triggerPin, uint8_t echoPin, uint8_t echoPinMode = INPUT) {
    PulseUltrasonicSensor::begin(triggerPin, echoPin, echoPinMode);
    setMinDistanceMm(200);
  }
};

}  // namespace DiFinders
