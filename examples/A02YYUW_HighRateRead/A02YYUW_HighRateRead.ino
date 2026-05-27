/*
 * A02YYUW_HighRateRead.ino — sample rate / drop statistics over 1 s
 *
 * Serial  = USB @ 115200
 * Serial1 = A02YYUW @ 9600
 */

#include <DiFinders.h>

A02YYUW distanceSensor;

uint32_t windowStartMs = 0;
uint32_t sampleCount = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }
  Serial.println(F("A02YYUW high-rate stats"));

#if defined(HAVE_HWSERIAL1) || defined(SERIAL_PORT_HARDWARE1)
  Serial1.begin(9600);
  distanceSensor.begin(Serial1);
  distanceSensor.setTimeout(200);
  windowStartMs = millis();
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
  if (reading.status == SensorStatus::Ok || reading.status == SensorStatus::OutOfRange) {
    sampleCount++;
  }

  if ((millis() - windowStartMs) >= 1000) {
    Serial.print(F("hz="));
    Serial.print(sampleCount);
    Serial.print(F(" last_mm="));
    Serial.print(distanceSensor.lastReading().distanceMm);
    Serial.print(F(" frame_err="));
    Serial.print(distanceSensor.frameErrorCount());
    Serial.print(F(" drop="));
    Serial.print(distanceSensor.droppedFrameCount());
    Serial.print(F(" age_ms="));
    Serial.println(millis() - distanceSensor.lastFrameTimestampMs());
    sampleCount = 0;
    windowStartMs = millis();
  }
}
