/*
 * GP2Y0E03_AnalogRead.ino — GP2Y0E03 I2C shift register + analog VOUT
 *
 * Reference: UNO R4 — I2C + analog A0, Serial @ 115200
 */

#include <Wire.h>
#include <DiFinders.h>

GP2Y0E03 sharp;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }
  Wire.begin();
  sharp.begin();
  sharp.beginAnalog(A0, df_adc(DF_ADC_REF), df_adc());
  sharp.calibrateAnalog(480, 290, 500, 3000);
  Serial.println(F("GP2Y0E03 I2C + analog ready"));
}

void loop() {
  RangeReading i2c = sharp.read();
  RangeReading analog = sharp.readAnalog();
  Serial.print(F("distance_mm="));
  Serial.print(i2c.distanceMm);
  Serial.print(F(" analog_mm="));
  Serial.println(analog.distanceMm);
  delay(150);
}
