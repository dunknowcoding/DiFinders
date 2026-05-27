/*
 * A02YYUW_BasicRead.ino — UART ultrasonic (DYP A02 family)
 *
 * Serial  = USB Serial Monitor @ 115200
 * Serial1 = sensor @ 9600 (UNO R4: MCU RX=D0, TX=D1 — cross TX/RX to module)
 */

#include <DiFinders.h>

A02YYUW sonar;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }

#if defined(HAVE_HWSERIAL1) || defined(SERIAL_PORT_HARDWARE1)
  Serial1.begin(9600);
  sonar.begin(Serial1);
  Serial.println(F("A02YYUW ready"));
#else
  Serial.println(F("Need hardware Serial1 (UNO R4, Mega, ESP32, RP2040)."));
#endif
}

void loop() {
#if !defined(HAVE_HWSERIAL1) && !defined(SERIAL_PORT_HARDWARE1)
  delay(1000);
  return;
#endif

  RangeReading r = sonar.read();
  if (r.status == SensorStatus::Ok || r.status == SensorStatus::OutOfRange) {
    Serial.print(F("distance_mm="));
    Serial.println(r.distanceMm);
  } else {
    Serial.print(F("status="));
    Serial.println(static_cast<uint8_t>(r.status));
  }
  delay(120);
}
