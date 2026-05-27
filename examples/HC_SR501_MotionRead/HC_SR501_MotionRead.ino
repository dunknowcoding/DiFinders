/*
 * HC_SR501_MotionRead.ino — PIR motion sensor
 *
 * Reference board: Arduino UNO R4 Minima
 *   PIR OUT -> digital pin 6 (adjust per module jumper card)
 */

#include <DiFinders.h>

const uint8_t PIR_PIN = 6;

HC_SR501 motionSensor(PIR_PIN);

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }
  motionSensor.begin(PIR_PIN);
  Serial.println(F("HC_SR501 ready"));
}

void loop() {
  MotionReading reading = motionSensor.read();
  Serial.print(F("motion="));
  Serial.print(reading.detected() ? 1 : 0);
  Serial.print(F(" rose="));
  Serial.print(reading.rose ? 1 : 0);
  Serial.print(F(" fell="));
  Serial.println(reading.fell ? 1 : 0);
  delay(40);
}
