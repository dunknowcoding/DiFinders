/*
 * VL53L4CD_Interrupt.ino
 *
 * VL53L4CD continuous ranging with INT pin on MY_TOF_INT.
 */

#include <DiFinders.h>
#include <Wire.h>

#define MY_TOF_ADDR DF_ADDR_VL53L1X
#define MY_TOF_INT DF_PIN_TOF_INT

VL53L4CD tofSensor;

void setup() {
  df_serial();
  df_wire();
  Serial.println(F("VL53L4CD interrupt"));
  if (!tofSensor.begin(Wire, MY_TOF_ADDR)) {
    Serial.println(F("init failed"));
    while (1) {
      delay(500);
    }
  }
  tofSensor.setTimeout(500);
  tofSensor.startContinuous(50);
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
