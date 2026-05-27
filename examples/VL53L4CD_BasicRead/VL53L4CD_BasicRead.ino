/*
 * VL53L4CD_BasicRead.ino — single-zone ToF (no SPAD ROI / no matrix)
 *
 * Reference board: Arduino UNO R4 Minima — I2C SDA=A4, SCL=A5
 *
 * VL53L4CD: one measurement zone only. No setBeamWindow() — cannot select SPAD
 * subsets or per-point ranges. Use VL53L1X or VL53L4CX for beam-window control,
 * For 8x8 multizone use VL53L8CX + https://github.com/stm32duino/VL53L8CX
 * (drivers/VL53L8CX, sketch VL53L8CX_ThirdPartyRead).
 */

#include <DiFinders.h>
#include <Wire.h>

#define MY_TOF_ADDR 0x29

VL53L4CD tof;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(100000);

  if (!tof.begin(Wire, MY_TOF_ADDR)) {
    Serial.println(F("VL53L4CD init failed"));
    while (1) {
      delay(500);
    }
  }

  tof.setTimeout(500);
  tof.startContinuous(50);
  Serial.println(F("VL53L4CD basic — single zone, no beam ROI"));
}

void loop() {
  RangeReading r = tof.read();
  Serial.print(F("distance_mm="));
  Serial.print(r.distanceMm);
  Serial.print(F(" status="));
  Serial.println(static_cast<uint8_t>(r.status));
  delay(80);
}
