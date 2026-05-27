/*
 * US100_BasicRead.ino - UART distance (mm) + on-chip temperature (deg C)
 *
 * Reference board: Arduino UNO R4 Minima
 *   USB monitor : Serial @ 115200
 *   Sensor UART : Serial1 @ 9600 - MCU RX=D0, TX=D1 (cross TX/RX to module)
 *
 * Why Serial1 (not Serial): same as A02YYUW_BasicRead - USB debug on Serial,
 *   sensor protocol on Serial1 at 9600. US-100 also needs Serial1 for the
 *   temperature command without disturbing monitor traffic.
 */

#include <DiFinders.h>

US100 sonar;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }
  Serial.println(F("US-100 distance + temperature"));

#if defined(HAVE_HWSERIAL1) || defined(SERIAL_PORT_HARDWARE1)
  Serial1.begin(9600);
  sonar.begin(Serial1);
  sonar.setTimeout(200);
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
  if (df_ok(r)) {
    Serial.print(F("distance_mm="));
    Serial.println(r.distanceMm);
  } else {
    Serial.print(F("distance_status="));
    Serial.println(static_cast<uint8_t>(r.status));
  }

  int16_t tempC = sonar.readTemperatureC();
  if (tempC > -100) {
    Serial.print(F("temperature_C="));
    Serial.println(tempC);
  }

  delay(500);
}
