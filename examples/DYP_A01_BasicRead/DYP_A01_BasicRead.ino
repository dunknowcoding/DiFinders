/*
 * DYP_A01_BasicRead.ino - UART distance (same frame family as A02YYUW)
 *
 * Reference board: Arduino UNO R4 Minima
 *   USB monitor : Serial @ 115200
 *   Sensor UART : Serial1 @ 9600 - MCU RX=D0, TX=D1
 *
 * Why Serial1 (not Serial): see A02YYUW_BasicRead.ino (USB on Serial, module on Serial1).
 */

#include <DiFinders.h>

DYP_A01 distanceSensor;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }
  Serial.println(F("DYP-A01 UART"));

#if defined(HAVE_HWSERIAL1) || defined(SERIAL_PORT_HARDWARE1)
  Serial1.begin(9600);
  distanceSensor.begin(Serial1);
#else
  Serial.println(F("Need hardware Serial1 (UNO R4, Mega, ESP32, RP2040)."));
#endif
}

void loop() {
#if !defined(HAVE_HWSERIAL1) && !defined(SERIAL_PORT_HARDWARE1)
  delay(1000);
  return;
#endif

  RangeReading reading = distanceSensor.read();
  Serial.print(F("distance_mm="));
  Serial.print(reading.distanceMm);
  Serial.print(F(" status="));
  Serial.println(static_cast<uint8_t>(reading.status));
  delay(120);
}
