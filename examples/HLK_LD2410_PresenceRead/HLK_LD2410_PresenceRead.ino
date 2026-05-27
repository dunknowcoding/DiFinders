/*
 * HLK_LD2410_PresenceRead.ino — mmWave presence over UART (not the OT2 GPIO pin)
 *
 * Most LD2410 boards use a 256000 baud serial link. Use a second hardware UART
 * (Serial1) for the module; keep Serial for the USB Serial Monitor.
 *
 * Reference board: Arduino UNO R4 Minima
 *   USB monitor : Serial @ 115200
 *   Sensor UART : Serial1 @ 256000 — MCU RX=D0, TX=D1 (cross TX/RX to module)
 *
 * Boards without Serial1 (AVR UNO/Nano): use Mega, UNO R4, ESP32, or RP2040.
 *
 * For modules wired to a digital OUT pin only, see HLK_LD2412_PresenceRead.ino
 * (LD2412_OUT) — not the same as full UART reporting on LD2410.
 */

#include <DiFinders.h>

HLK_LD2410_UART radar;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }

#if defined(HAVE_HWSERIAL1) || defined(SERIAL_PORT_HARDWARE1)
  Serial1.begin(256000);
  radar.begin(Serial1, true);
  Serial.println(F("HLK-LD2410 UART presence"));
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
  if (motion.detected()) {
    Serial.println(F("yes"));
  } else {
    Serial.println(F("no"));
  }
  delay(100);
}
