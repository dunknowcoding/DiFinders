/*
 * VL53L0X_HighRateRead.ino — faster loop using continuous mode (I2C, not DMA)
 *
 * Arduino Wire does not expose I2C DMA. This sketch only shortens the loop delay
 * after startContinuous(). Compare with VL53L0X_ContinuousRead.ino (same API).
 *
 * I2C: Wire.begin(), SDA/SCL per board (UNO R4: A4/A5), 3.3 V, address 0x29.
 */

#include <DiFinders.h>
#include <Wire.h>

const uint8_t MY_ADDR = 0x29;

VL53L0X tof;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000);

  if (!tof.begin(Wire, MY_ADDR)) {
    Serial.println(F("VL53L0X init failed"));
    while (1) {
      delay(500);
    }
  }

  tof.setTimeout(200);
  tof.startContinuous(30);
  Serial.println(F("VL53L0X high rate (I2C poll)"));
}

void loop() {
  RangeReading r;
  if (tof.readIfReady(r)) {
    Serial.print(F("distance_mm="));
    Serial.println(r.distanceMm);
  }
}
