/*
 * VL53L0X_Interrupt.ino
 *
 * VL53L0X continuous ranging with optional GPIO1 wired to MY_TOF_INT.
 * On interrupt (or dataReady), readIfReady() avoids blocking waits.
 *
 * I2C: SDA/SCL + 3.3V. Connect sensor GPIO1 to MY_TOF_INT if available.
 */

#include <DiFinders.h>
#include <Wire.h>

#define MY_TOF_ADDR DF_ADDR_VL53L0X
#define MY_TOF_INT DF_PIN_TOF_INT

VL53L0X tofSensor;

void setup() {
  df_serial();
  df_wire();
  Serial.println(F("VL53L0X interrupt"));
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
