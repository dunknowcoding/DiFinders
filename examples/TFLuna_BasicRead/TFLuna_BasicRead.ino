/*
 * TFLuna_BasicRead.ino ? distance + frame strength + chip temperature
 *
 * Reference board: Arduino UNO R4 Minima
 *   USB debug  : Serial @ 115200
 *   Sensor UART: Serial1 @ 115200 ? RX=D0, TX=D1
 *
 * Compatibility: same as other Serial1 examples (UNO R4, Mega, ESP32, RP2040).
 */

#include <DiFinders.h>

TFMini lidar;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }
  Serial.println(F("TF-Luna / TFMini read"));

#if defined(HAVE_HWSERIAL1) || defined(SERIAL_PORT_HARDWARE1)
  Serial1.begin(115200);
  lidar.begin(Serial1);
#else
  Serial.println(F("Need hardware Serial1."));
#endif
}

void loop() {
#if !defined(HAVE_HWSERIAL1) && !defined(SERIAL_PORT_HARDWARE1)
  return;
#endif

  RangeReading r = lidar.read();
  Serial.print(F("distance_mm="));
  Serial.print(r.distanceMm);
  Serial.print(F(" status="));
  Serial.println(static_cast<uint8_t>(r.status));

  int16_t tempC = lidar.temperatureC();
  if (tempC > -100) {
    Serial.print(F("temperature_C="));
    Serial.println(tempC);
  }

  delay(100);
}
