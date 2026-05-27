/*
 * MultiSensor_QuickRead.ino — pulse + analog IR + reflective line + PIR on one board
 *
 * Reference board: Arduino UNO R4 Minima
 *   HC-SR04 : TRIG=2, ECHO=3
 *   GP2Y0A21: A0
 *   TCRT5000: digital=4, analog=A1, emitter=5
 *   HC-SR501: pin 6
 */

#include <DiFinders.h>

const uint8_t TRIG_PIN = 2;
const uint8_t ECHO_PIN = 3;
const uint8_t SHARP_PIN = A0;
const uint8_t LINE_DIG_PIN = 4;
const uint8_t LINE_ANA_PIN = A1;
const uint8_t LINE_EMIT_PIN = 5;
const uint8_t PIR_PIN = 6;

HC_SR04 distanceSensor(TRIG_PIN, ECHO_PIN);
GP2Y0A21 irSensor(SHARP_PIN, df_adc(DF_ADC_REF), df_adc());
TCRT5000 reflectiveSensor(LINE_DIG_PIN, LINE_ANA_PIN, LINE_EMIT_PIN);
HC_SR501 motionSensor(PIR_PIN);

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }
  distanceSensor.configureTiming(30000);
  reflectiveSensor.setCalibrationWindow(40, 850);
  motionSensor.begin(PIR_PIN);
  Serial.println(F("MultiSensor ready"));
}

void loop() {
  RangeReading sonar = distanceSensor.read();
  RangeReading sharp = irSensor.read();

  Serial.print(F("hc_sr04_mm="));
  Serial.print(sonar.distanceMm);
  Serial.print(F(" gp2y_mm="));
  Serial.println(sharp.distanceMm);

  reflectiveSensor.read();
  motionSensor.read();
  Serial.print(F("line_active="));
  Serial.print(reflectiveSensor.active() ? 1 : 0);
  Serial.print(F(" pir_active="));
  Serial.println(motionSensor.motion().detected() ? 1 : 0);

  delay(120);
}
