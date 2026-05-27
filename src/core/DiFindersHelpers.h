#pragma once



#include <Arduino.h>



#include "DiFindersConstants.h"

#include "DiFindersTypes.h"



namespace DiFinders {



/**

 * Sketch helpers — ADC scaling and motion/proximity state only.

 * Use sensor.read() and SensorStatus in sketches (no df_ok / DF_PROBE macros).

 */



inline bool df_hit(const MotionReading& s) {

  return s.state == DetectionState::Active;

}



inline bool df_hit(const ProximityReading& s) {

  return s.state == DetectionState::Active;

}



inline uint16_t df_adc(uint8_t kind) {

  if (kind == DF_ADC_REF) {

#if defined(ARDUINO_ARCH_AVR)

    return 5000;

#else

    return 3300;

#endif

  }

#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_STM32) || defined(ARDUINO_ARCH_RP2040)

  return 4095;

#else

  return 1023;

#endif

}



inline uint16_t df_adc() {

  return df_adc(DF_ADC_MAX);

}



}  // namespace DiFinders

