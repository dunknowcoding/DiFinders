/*
 * VL53L4CD_HighRateRead.ino — continuous I2C polling (not Wire DMA)
 *
 * Uses startContinuous() and readIfReady() with no extra delay in loop.
 * For ESP32 SPI burst throughput see VL53L1X_SpiBurstRead.ino.
 */

#include <DiFinders.h>
#include <Wire.h>

const uint8_t MY_ADDR = 0x29;

VL53L4CD tof;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000);

  if (!tof.begin(Wire, MY_ADDR)) {
    Serial.println(F("VL53L4CD init failed"));
    while (1) {
      delay(500);
    }
  }

  tof.setTimeout(200);
  tof.startContinuous(10);
  Serial.println(F("VL53L4CD high rate"));
}

void loop() {
  RangeReading r;
  if (tof.readIfReady(r)) {
    Serial.print(F("distance_mm="));
    Serial.println(r.distanceMm);
  }
}
