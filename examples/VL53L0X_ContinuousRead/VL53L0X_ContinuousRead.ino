/*
 * VL53L0X_ContinuousRead.ino — continuous ranging on I2C
 *
 * Reference board: Arduino UNO R4 Minima
 *   USB : Serial @ 115200
 *   I2C : SDA=A4, SCL=A5 (3.3 V module — level-shift on 5 V AVR if needed)
 *
 * Same Wire API as Arduino IDE I2C examples: Wire.begin(), then sensor.begin(Wire).
 */

#include <DiFinders.h>
#include <Wire.h>

const uint8_t MY_ADDR = 0x29;

VL53L0X tof;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }

  Wire.begin();
  Wire.setClock(100000);

  if (!tof.begin(Wire, MY_ADDR)) {
    Serial.println(F("VL53L0X init failed"));
    while (1) {
      delay(500);
    }
  }

  tof.setTimeout(500);
  tof.startContinuous(50);
  Serial.println(F("VL53L0X continuous"));
}

void loop() {
  RangeReading r = tof.read();
  Serial.print(F("distance_mm="));
  Serial.print(r.distanceMm);
  Serial.print(F(" status="));
  Serial.println(static_cast<uint8_t>(r.status));
  delay(50);
}
