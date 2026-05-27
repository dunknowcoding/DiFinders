/*
 * VL6180X_BasicRead.ino — single-zone ToF (proximity + range)
 *
 * Reference board: Arduino UNO R4 Minima — I2C SDA=A4, SCL=A5 @ 0x29
 */

#include <DiFinders.h>
#include <Wire.h>

#define MY_TOF_I2C_ADDR DF_ADDR_VL6180X

VL6180X tofSensor;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }
  Wire.begin();
  Wire.setClock(100000);

  if (!tofSensor.begin(Wire, MY_TOF_I2C_ADDR)) {
    Serial.println(F("VL6180X init failed"));
    while (1) {
      delay(500);
    }
  }

  tofSensor.setTimeout(500);
  tofSensor.setScaling(1);
  Serial.println(F("VL6180X ready"));
}

void loop() {
  RangeReading r = tofSensor.read();
  if (r.status == SensorStatus::Ok || r.status == SensorStatus::OutOfRange) {
    Serial.print(F("distance_mm="));
    Serial.println(r.distanceMm);
  } else {
    Serial.print(F("status="));
    Serial.println(static_cast<uint8_t>(r.status));
  }
  delay(80);
}
