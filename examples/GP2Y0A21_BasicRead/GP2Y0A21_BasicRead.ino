/*
 * GP2Y0A21_BasicRead.ino - analog infrared distance (Sharp GP2Y0A21YK)
 *
 * Reference board: Arduino UNO R4 Minima - analog input MY_ANALOG_PIN (default A0)
 * No UART: only Serial is used for USB monitor output.
 */

#include <DiFinders.h>

#define MY_ANALOG_PIN DF_PIN_ANA

// df_adc: board ADC reference millivolts and full-scale counts for distance math
GP2Y0A21 irSensor(MY_ANALOG_PIN, df_adc(DF_ADC_REF), df_adc());

void setup() {
  Serial.begin(115200);
  Serial.println(F("GP2Y0A21 analog IR"));
}

void loop() {
  RangeReading reading = irSensor.read();
  Serial.print(F("distance_mm="));
  Serial.println(reading.distanceMm);
  delay(120);
}
