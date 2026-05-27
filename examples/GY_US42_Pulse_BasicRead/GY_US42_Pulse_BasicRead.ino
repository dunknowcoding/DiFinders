/*
 * GY_US42_Pulse_BasicRead.ino — GY-US42 in pulse (HC-SR04 style) mode
 *
 * Reference: UNO R4 — TRIG pin 2, ECHO pin 3
 */

#include <DiFinders.h>

GY_US42_Pulse sonar(2, 3);

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }
  Serial.println(F("GY-US42 pulse ready"));
}

void loop() {
  RangeReading sample = sonar.read();
  if (sample.status == SensorStatus::Ok || sample.status == SensorStatus::OutOfRange) {
    Serial.print(F("distance_mm="));
    Serial.println(sample.distanceMm);
  } else {
    Serial.print(F("status="));
    Serial.println(static_cast<uint8_t>(sample.status));
  }
  delay(120);
}
