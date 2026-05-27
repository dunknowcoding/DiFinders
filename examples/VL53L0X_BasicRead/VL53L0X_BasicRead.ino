/*
 * VL53L0X_BasicRead.ino — single-zone ToF
 *
 * Reference board: Arduino UNO R4 Minima — I2C SDA=A4, SCL=A5
 *
 * Beam / SPAD: VL53L0X has no user ROI API in DiFinders. During begin(), the driver
 * auto-calibrates and programs the SPAD enable map once (which SPADs are active).
 * You cannot move a rectangular window from the sketch — re-power/re-begin after changes.
 *
 * NOT a multizone matrix sensor (no per-point distances).
 */

#include <DiFinders.h>
#include <Wire.h>

#define MY_TOF_ADDR 0x29

VL53L0X tof;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(100000);

  if (!tof.begin(Wire, MY_TOF_ADDR)) {
    Serial.println(F("VL53L0X init failed"));
    while (1) {
      delay(500);
    }
  }

  tof.setTimeout(500);
  Serial.println(F("VL53L0X basic — SPAD mask fixed at init"));
}

void loop() {
  RangeReading r = tof.read();
  Serial.print(F("distance_mm="));
  Serial.print(r.distanceMm);
  Serial.print(F(" status="));
  Serial.println(static_cast<uint8_t>(r.status));
  delay(120);
}
