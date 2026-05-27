/*
 * GY_US42_UART_BasicRead.ino — GY-US42 in UART mode (A02YYUW frame family)
 *
 * Serial  = USB Serial Monitor @ 115200
 * Serial1 = sensor @ 9600 (UNO R4: MCU RX=D0, TX=D1 — cross TX/RX to module)
 */

#include <DiFinders.h>

GY_US42_UART distanceSensor;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }

#if defined(HAVE_HWSERIAL1) || defined(SERIAL_PORT_HARDWARE1)
  Serial1.begin(9600);
  distanceSensor.begin(Serial1);
  Serial.println(F("GY-US42 UART ready"));
#else
  Serial.println(F("Need hardware Serial1 (UNO R4, Mega, ESP32, RP2040)."));
#endif
}

void loop() {
#if !defined(HAVE_HWSERIAL1) && !defined(SERIAL_PORT_HARDWARE1)
  delay(1000);
  return;
#endif

  RangeReading r = distanceSensor.read();
  if (r.status == SensorStatus::Ok || r.status == SensorStatus::OutOfRange) {
    Serial.print(F("distance_mm="));
    Serial.println(r.distanceMm);
  } else {
    Serial.print(F("status="));
    Serial.println(static_cast<uint8_t>(r.status));
  }
  delay(120);
}
