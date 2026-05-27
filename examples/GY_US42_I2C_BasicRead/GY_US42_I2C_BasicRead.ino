/*
 * GY_US42_I2C_BasicRead.ino — GY-US42 / GY-US42V2 I2C (default address 0x70)
 *
 * Reference: UNO R4 — SDA=A4, SCL=A5, Serial Monitor @ 115200
 */

#include <Wire.h>
#include <DiFinders.h>

GY_US42 sonar;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
  }
  Wire.begin();
  sonar.begin();
  Serial.println(F("GY-US42 I2C basic read"));
}

void loop() {
  const RangeReading sample = sonar.read();
  Serial.print(F("distance_mm="));
  Serial.print(sample.distanceMm);
  Serial.print(F(" status="));
  Serial.println(static_cast<uint8_t>(sample.status));
  delay(120);
}
