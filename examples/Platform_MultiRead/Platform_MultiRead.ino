/*
 * Platform_MultiRead.ino — multi-sensor smoke read on one board
 *
 * Reference board: Arduino UNO R4 Minima
 *   HC-SR04   : TRIG=2, ECHO=3
 *   GP2Y0A21  : analog A0
 *   VL53L0X   : I2C SDA=A4, SCL=A5 @ 0x29
 *   TOFSense  : I2C (default address in driver)
 *   HLK-LD2410: digital OUT pin 8 (not UART in this sketch)
 */

#include <DiFinders.h>
#include <Wire.h>

const uint8_t TRIG_PIN = 2;
const uint8_t ECHO_PIN = 3;
const uint8_t SHARP_PIN = A0;
const uint8_t MMWAVE_OUT_PIN = 8;

HC_SR04 distanceSensor(TRIG_PIN, ECHO_PIN);
GP2Y0A21 irSensor(SHARP_PIN, df_adc(DF_ADC_REF), df_adc());
TOF200C tofSensor;
TOFSenseF2P_I2C tofSenseI2c;
HLK_LD2410 presenceSensor(MMWAVE_OUT_PIN);

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }
  Wire.begin();
  Wire.setClock(100000);
  distanceSensor.configureTiming(30000);
  tofSensor.begin(Wire, 0x29);
  tofSenseI2c.begin();
  presenceSensor.begin(MMWAVE_OUT_PIN);
  Serial.println(F("Platform multi-sensor read"));
}

void loop() {
  RangeReading sonar = distanceSensor.read();
  Serial.print(F("hc_sr04_mm="));
  Serial.println(sonar.distanceMm);

  RangeReading sharp = irSensor.read();
  Serial.print(F("gp2y_mm="));
  Serial.println(sharp.distanceMm);

  RangeReading tof = tofSensor.read();
  Serial.print(F("vl53_mm="));
  Serial.println(tof.distanceMm);

  RangeReading sense = tofSenseI2c.read();
  Serial.print(F("tofsense_mm="));
  Serial.println(sense.distanceMm);

  presenceSensor.read();
  Serial.print(F("ld2410_detect="));
  Serial.println(presenceSensor.detected() ? 1 : 0);

  Serial.println();
  delay(600);
}
