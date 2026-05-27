/*
 * VL53L4CX_BasicRead.ino — single-zone ToF + SPAD beam window (same idea as VL53L1X)
 *
 * Reference board: Arduino UNO R4 Minima — I2C SDA=A4, SCL=A5
 *
 * NOT multizone 8x8: one distance per read. setBeamWindow() selects a rectangular
 * subset of the 16x16 SPAD array (reception cone), not individual matrix cells.
 */

#include <DiFinders.h>
#include <Wire.h>

#define MY_TOF_ADDR 0x29
#define MY_BEAM_W 8
#define MY_BEAM_H 8
#define MY_BEAM_CENTER 199

VL53L4CX tof;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000);

  if (!tof.begin(Wire, MY_TOF_ADDR)) {
    Serial.println(F("VL53L4CX init failed"));
    while (1) {
      delay(500);
    }
  }

  tof.setTimeout(1000);
  tof.setMeasurementTimingBudget(33333);

  // Narrow SPAD reception window (beam steering via ROI)
  tof.setBeamWindow(MY_BEAM_W, MY_BEAM_H, MY_BEAM_CENTER);

  tof.startContinuous();
  Serial.println(F("VL53L4CX basic + beam window"));
}

void loop() {
  RangeReading r = tof.read();
  Serial.print(F("distance_mm="));
  Serial.print(r.distanceMm);
  Serial.print(F(" status="));
  Serial.println(static_cast<uint8_t>(r.status));
  delay(60);
}
