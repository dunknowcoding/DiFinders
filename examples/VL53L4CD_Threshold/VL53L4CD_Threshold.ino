/*
 * VL53L4CD_Threshold.ino — signal-rate and sigma thresholds (ST driver limits)
 *
 * Reference board: Arduino UNO R4 Minima — I2C SDA=A4, SCL=A5
 *
 * Low return signal or high sigma marks reading as out-of-range / error.
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

  tof.setSignalThreshold(512);
  tof.setSigmaThreshold(60);
  tof.startContinuous(50);

  Serial.println(F("VL53L4CD signal/sigma thresholds"));
}

void loop() {
  RangeReading r = tof.read();
  Serial.print(F("mm="));
  Serial.print(r.distanceMm);
  Serial.print(F(" signal_kcps="));
  Serial.print(tof.lastSignalRateKcps());
  Serial.print(F(" sigma_mm="));
  Serial.print(tof.lastSigmaMm());
  Serial.print(F(" status="));
  Serial.println(static_cast<uint8_t>(r.status));
  delay(80);
}
