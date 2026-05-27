#include <Wire.h>
#include <DiFinders.h>

GP2Y0E03 sharp;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
  }
  Wire.begin();
  sharp.begin();
  Serial.println(F("GP2Y0E03 I2C basic read"));
}

void loop() {
  const RangeReading sample = sharp.read();
  Serial.print(F("distance_mm="));
  Serial.print(sample.distanceMm);
  Serial.print(F(" shift="));
  Serial.println(sharp.shiftBit());
  delay(120);
}
