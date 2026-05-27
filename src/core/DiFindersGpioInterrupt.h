#pragma once

#include <Arduino.h>

namespace DiFinders {

/** Pin modes for attachInterrupt (same as Arduino constants). */
#define DF_IRQ_LOW 0
#define DF_IRQ_HIGH 1
#define DF_IRQ_CHANGE 2
#define DF_IRQ_FALLING 3
#define DF_IRQ_RISING 4

/**
 * One GPIO interrupt line. Use one static instance per IRQ pin in the sketch.
 * The ISR must call gpioInterruptIsr(&yourLine) from a plain function (AVR-safe).
 */
class GpioInterruptLine {
 public:
  void arm() {
    pending_ = false;
    armed_ = true;
  }

  void disarm() {
    armed_ = false;
    pending_ = false;
  }

  void notifyFromIsr() {
    if (armed_) {
      pending_ = true;
    }
  }

  bool takeEvent() {
    if (!pending_) {
      return false;
    }
    pending_ = false;
    return true;
  }

  bool pending() const {
    return pending_;
  }

 private:
  volatile bool pending_ = false;
  volatile bool armed_ = false;
};

inline void gpioInterruptIsr(GpioInterruptLine* line) {
  if (line != nullptr) {
    line->notifyFromIsr();
  }
}

inline void attachGpioInterrupt(uint8_t pin, void (*isrFn)(), GpioInterruptLine& line, uint8_t mode) {
  line.arm();
#if defined(ARDUINO_ARCH_RENESAS)
  attachInterrupt(digitalPinToInterrupt(pin), isrFn, static_cast<PinStatus>(mode));
#else
  attachInterrupt(digitalPinToInterrupt(pin), isrFn, mode);
#endif
}

inline void detachGpioInterrupt(uint8_t pin) {
  detachInterrupt(digitalPinToInterrupt(pin));
}

}  // namespace DiFinders
