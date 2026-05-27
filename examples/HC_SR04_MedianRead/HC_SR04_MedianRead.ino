/*
 * HC_SR04_MedianRead.ino — median-filtered pulse ultrasonic read
 *
 * Reference: Arduino UNO R4 Minima — TRIG pin 2, ECHO pin 3
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
  Serial.println(F("HC-SR04 median ready"));
}

void loop() {
  uint16_t mm = sonar.medianMillimeters();
  Serial.print(F("distance_mm="));
  Serial.println(mm);
  delay(120);
}
