/*
 * GP2Y0D810_Threshold.ino — factory distance window (20–100 mm) digital alert
 *
 * Reference board: Arduino UNO R4 Minima
 *   Sensor OUT : digital pin 4 (MY_DIG) — active when target in window
 *
 * The GP2Y0D810 is a fixed-threshold Sharp digital sensor; software prints window bounds.
 */

#include <DiFinders.h>

#define MY_DIG 4

GP2Y0D810 windowSensor(MY_DIG);

void setup() {
  Serial.begin(115200);
  Serial.print(F("window_mm "));
  Serial.print(windowSensor.minMillimeters());
  Serial.print(F("-"));
  Serial.println(windowSensor.maxMillimeters());
}

void loop() {
  ProximityReading p = windowSensor.read();
  Serial.print(F("in_window="));
  Serial.println(df_hit(p) ? 1 : 0);
  delay(150);
}
