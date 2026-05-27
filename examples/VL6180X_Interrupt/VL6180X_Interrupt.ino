/*
 * VL6180X_Interrupt.ino
 *
 * VL6180X (TOF050) with GPIO interrupt on MY_TOF_INT and readIfReady().
 * configureDefault() already sets interrupt-friendly GPIO config.
 */

#include <DiFinders.h>
#include <Wire.h>

#define MY_TOF_ADDR DF_ADDR_VL6180X
#define MY_TOF_INT DF_PIN_TOF_INT

VL6180X tofSensor;

void setup() {
  df_serial();
  df_wire();
  Serial.println(F("VL6180X interrupt"));
  if (!tofSensor.begin(Wire, MY_TOF_ADDR)) {
    Serial.println(F("init failed"));
    while (1) {
      delay(500);
    }
  }
  tofSensor.configureDefault();
  tofSensor.setTimeout(500);
  tofSensor.startContinuous(100);
  tofSensor.attachDataReadyPin(MY_TOF_INT, DF_IRQ_FALLING);
}

void loop() {
  RangeReading reading;
  if (tofSensor.takeDataReadyEvent() || tofSensor.dataReady()) {
    if (tofSensor.readIfReady(reading)) {
      df_show(reading);
    }
  }
  delay(5);
}
