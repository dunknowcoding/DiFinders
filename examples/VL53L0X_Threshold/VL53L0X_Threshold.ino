/*
 * VL53L0X_Threshold.ino — programmable distance window (mm)
 *
 * Reference board: Arduino UNO R4 Minima — I2C SDA=A4, SCL=A5
 * Optional INT: module GPIO -> pin 7 (MY_TOF_INT)
 *
 * Alerts when distance is outside 80–300 mm.
 */

#include <DiFinders.h>
#include <Wire.h>

#define MY_TOF_ADDR 0x29
#define MY_TOF_INT 7
#define WINDOW_INNER_MM 80
#define WINDOW_OUTER_MM 300

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

  tof.setDistanceWindowMm(WINDOW_INNER_MM, WINDOW_OUTER_MM, 2);
  tof.startContinuous(50);
  tof.attachDataReadyPin(MY_TOF_INT, DF_IRQ_FALLING);

  Serial.println(F("VL53L0X threshold"));
}

void loop() {
  RangeReading r;
  if (tof.readIfReady(r)) {
    bool alert = tof.isOutsideDistanceWindow(r.distanceMm, WINDOW_INNER_MM, WINDOW_OUTER_MM);
    Serial.print(F("mm="));
    Serial.print(r.distanceMm);
    Serial.print(F(" alert="));
    Serial.println(alert ? 1 : 0);
  }
  delay(20);
}
