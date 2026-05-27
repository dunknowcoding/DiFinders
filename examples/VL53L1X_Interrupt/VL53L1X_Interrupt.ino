/*
 * VL53L1X_Interrupt.ino
 *
 * VL53L1X long-range mode, continuous measurements, MCU interrupt on GPIO1.
 * Uses readIfReady() when the INT line fires or the status register is set.
 */

#include <DiFinders.h>
#include <Wire.h>

#define MY_TOF_ADDR DF_ADDR_VL53L1X
#define MY_TOF_INT DF_PIN_TOF_INT

VL53L1X tofSensor;

void setup() {
  df_serial();
  df_wire();
  Serial.println(F("VL53L1X interrupt"));
  if (!tofSensor.begin(Wire, MY_TOF_ADDR)) {
    Serial.println(F("init failed"));
    while (1) {
      delay(500);
    }
  }
  tofSensor.setTimeout(500);
  tofSensor.useLongDistanceMode();
  tofSensor.setMeasurementTimingBudget(50000);
  tofSensor.startContinuous(60);
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
