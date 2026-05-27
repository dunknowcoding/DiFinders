/*
 * VL53L7CX_ThirdPartyRead.ino - 8x8 multizone ToF (NOT in DiFinders mainline)
 *
 * Required third-party driver:
 *   https://github.com/stm32duino/VL53L7CX
 * Local copy: drivers/VL53L7CX  (tag 1.0.3)
 *
 * Compile:
 *   arduino-cli compile --fqbn esp32:esp32:esp32 \
 *     --library . --library drivers/VL53L7CX examples/VL53L7CX_ThirdPartyRead
 */

#include <DiFinders.h>
#include <Wire.h>
#include <vl53l7cx_class.h>

#define MY_LPN_PIN 2
#define MY_I2C_RST_PIN -1

VL53L7CX stTof(&Wire, MY_LPN_PIN, MY_I2C_RST_PIN);

void setup() {
  Serial.begin(115200);
  Wire.begin();

  Serial.println(F("VL53L7CX multizone"));
  Serial.println(F("Driver: https://github.com/stm32duino/VL53L7CX"));

  stTof.begin();
  if (stTof.init_sensor() != 0) {
    Serial.println(F("init failed"));
    while (1) {
      delay(500);
    }
  }
  stTof.vl53l7cx_start_ranging();
}

void loop() {
  uint8_t ready = 0;
  stTof.vl53l7cx_check_data_ready(&ready);
  if (ready) {
    VL53L7CX_ResultsData results;
    if (stTof.vl53l7cx_get_ranging_data(&results) == 0) {
      Serial.print(F("zone0_mm="));
      Serial.println(results.distance_mm[0]);
    }
  }
  delay(100);
}
