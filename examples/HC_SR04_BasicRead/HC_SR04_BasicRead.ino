/*
 * HC_SR04_BasicRead.ino — trigger/echo ultrasonic distance
 *
 * Reference: Arduino UNO R4 Minima
 *   TRIG -> pin 2, ECHO -> pin 3, 5 V and GND to module
 *   Serial Monitor @ 115200 on USB
 *
 * ESP32/RP2040: change pins; use a divider on ECHO if the module outputs 5 V.
 */

#include <DiFinders.h>

const uint8_t TRIG_PIN = 2;
const uint8_t ECHO_PIN = 3;

HC_SR04 sonar(TRIG_PIN, ECHO_PIN);

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }
  sonar.configureTiming(30000);
  Serial.println(F("HC-SR04 ready"));
}

void loop() {
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
