/*
 * VL53L1X_BasicRead.ino — single-zone ToF on I2C (Wire library)
 *
 * UNO R4: SDA=A4, SCL=A5, 3.3 V, address 0x29
 * VL53L1X returns one range per frame (not 8x8 multizone).
 */

#include <DiFinders.h>
#include <Wire.h>

const uint8_t SENSOR_ADDR = 0x29;
const uint8_t BEAM_W = 8;
const uint8_t BEAM_H = 8;
const uint8_t BEAM_CENTER = 199;

VL53L1X tof;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }

  Wire.begin();
  Wire.setClock(100000);

  if (!tof.begin(Wire, SENSOR_ADDR)) {
    Serial.println(F("VL53L1X init failed"));
    while (1) {
      delay(500);
    }
  }

  tof.setTimeout(500);
  tof.useLongDistanceMode();
  tof.setMeasurementTimingBudget(50000);
  tof.setBeamWindow(BEAM_W, BEAM_H, BEAM_CENTER);
  tof.startContinuous(60);

  Serial.println(F("VL53L1X running"));
}

void loop() {
  RangeReading r = tof.read();
  Serial.print(F("distance_mm="));
  Serial.print(r.distanceMm);
  Serial.print(F(" status="));
  Serial.println(static_cast<uint8_t>(r.status));
  delay(80);
}
