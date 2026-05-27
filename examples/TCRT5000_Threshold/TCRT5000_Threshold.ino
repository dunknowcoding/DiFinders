/*
 * TCRT5000_Threshold.ino — software reflectivity threshold (permille)
 *
 * Reference board: Arduino UNO R4 Minima
 *   Digital OUT : pin 4  (MY_LINE_DIG)
 *   Analog OUT  : A1    (MY_LINE_ANA) — optional, for threshold tuning
 *   Emitter ctl : pin 5  (MY_LINE_EMIT) — optional jumper on module
 *
 * Compatibility:
 *   AVR/UNO R4: A1 analog input, D4/D5 digital
 *   ESP32/RP2040: use 3.3 V ADC range; adjust threshold permille after calibration
 */

#include <DiFinders.h>

#define MY_LINE_DIG 4
#define MY_LINE_ANA A1
#define MY_LINE_EMIT 5

TCRT5000 lineSense(MY_LINE_DIG, MY_LINE_ANA, MY_LINE_EMIT);

void setup() {
  Serial.begin(115200);
  lineSense.setCalibrationWindow(50, 900);
  lineSense.setAnalogThresholdPermille(350);
  Serial.println(F("TCRT5000 threshold 350 permille"));
}

void loop() {
  ProximityReading p = lineSense.read();
  Serial.print(F("detected="));
  Serial.print(df_hit(p) ? 1 : 0);
  Serial.print(F(" strength="));
  Serial.println(p.strengthPermille);
  delay(100);
}
