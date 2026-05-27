/*
 * OpenProtocol_Mmwave_BasicRead.ino — DFRobot C4001 / MR24HPC1 open UART protocol
 *
 * Use DFRobot_C4001 below; MR24HPC1 is the same OpenProtocolMmwaveSensor API.
 *
 * Reference board: Arduino UNO R4 Minima
 *   USB monitor : Serial @ 115200
 *   Sensor UART : Serial1 @ 115200 — MCU RX=D0, TX=D1 (cross TX/RX to module)
 *
 * See docs/USER_GUIDE.md for wiring and protocol notes.
 */

#include <DiFinders.h>

DFRobot_C4001 radar;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }

#if defined(HAVE_HWSERIAL1) || defined(SERIAL_PORT_HARDWARE1)
  Serial1.begin(115200);
  radar.begin(Serial1);
  radar.enableUnderlyingMessage(true);
  Serial.println(F("Open-protocol mmWave ready"));
#else
  Serial.println(F("Need hardware Serial1 (UNO R4, Mega, ESP32, RP2040)."));
#endif
}

void loop() {
#if !defined(HAVE_HWSERIAL1) && !defined(SERIAL_PORT_HARDWARE1)
  delay(1000);
  return;
#endif

  radar.read();
  MotionReading motion = radar.motion();
  Serial.print(F("motion="));
  Serial.print(motion.detected() ? 1 : 0);
  Serial.print(F(" detected="));
  Serial.println(radar.presenceDetected() ? 1 : 0);
  delay(100);
}
