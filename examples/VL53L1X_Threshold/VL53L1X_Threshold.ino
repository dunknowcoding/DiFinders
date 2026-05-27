/*
 * VL53L1X_Threshold.ino — distance window alert (ST threshold interrupt config)
 *
 * Reference board: Arduino UNO R4 Minima
 *   USB debug : Serial @ 115200
 *   I2C       : SDA=A4, SCL=A5
 *   Optional  : GPIO1 on module -> MY_TOF_INT (pin 7) for hardware threshold IRQ
 *
 * Window: alert when distance is OUTSIDE 100–400 mm (software check + optional INT pin).
 *
 * Compatibility:
 *   Any 3.3 V I2C board with Wire; level-shift if module is 3.3 V only and MCU is 5 V.
 *   INT pin: connect only if your breakout exposes GPIO1.
 */

#include <DiFinders.h>
#include <Wire.h>

#define MY_TOF_ADDR 0x29
#define MY_TOF_INT 7
#define WINDOW_INNER_MM 100
#define WINDOW_OUTER_MM 400

VL53L1X tof;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }

  Wire.begin();
  Wire.setClock(100000);

  if (!tof.begin(Wire, MY_TOF_ADDR)) {
    Serial.println(F("VL53L1X init failed"));
    while (1) {
      delay(500);
    }
  }

  tof.setDistanceWindowMm(WINDOW_INNER_MM, WINDOW_OUTER_MM, 2);
  tof.startContinuous(50);
  tof.attachDataReadyPin(MY_TOF_INT, DF_IRQ_FALLING);

  Serial.println(F("VL53L1X threshold window"));
  Serial.print(F("inner_mm="));
  Serial.print(WINDOW_INNER_MM);
  Serial.print(F(" outer_mm="));
  Serial.println(WINDOW_OUTER_MM);
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
