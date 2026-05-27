/*
 * TW10S_UART_BasicRead.ino — ASCII UART laser distance
 *
 * Serial  = USB monitor @ 115200
 * Serial1 = TW10S @ 19200 (UNO R4: MCU RX=D0, TX=D1 — cross TX/RX)
 */

#include <DiFinders.h>

TW10S_UART distanceSensor;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }
  Serial.println(F("TW10S UART"));

#if defined(HAVE_HWSERIAL1) || defined(SERIAL_PORT_HARDWARE1)
  Serial1.begin(19200);
  distanceSensor.begin(Serial1);
  distanceSensor.setPrefix("D");
  distanceSensor.setUnitMillimeters(1.0f);
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
  delay(100);
}
