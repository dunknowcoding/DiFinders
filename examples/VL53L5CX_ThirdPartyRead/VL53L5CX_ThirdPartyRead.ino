/*
 * VL53L5CX_ThirdPartyRead.ino - 8x8 multizone ToF (NOT in DiFinders mainline)
 *
 * Required third-party driver:
 *   https://github.com/stm32duino/VL53L5CX
 * Local copy: drivers/VL53L5CX  (tag 1.2.3)
 *
 * Upstream library targets stm32,sam in library.properties; use STM32 or Due-class
 * boards, or ESP32 if you port pins. DiFinders does not implement VL53L5CX ranging.
 *
 * Compile:
 *   arduino-cli compile --fqbn esp32:esp32:esp32 \
 *     --library . --library drivers/VL53L5CX examples/VL53L5CX_ThirdPartyRead
 */

#include <DiFinders.h>
#include <Wire.h>
#include <vl53l5cx_class.h>

#define MY_LPN_PIN 2
#define MY_I2C_RST_PIN -1

VL53L5CX stTof(&Wire, MY_LPN_PIN, MY_I2C_RST_PIN);

void setup() {
  Serial.begin(115200);
  Wire.begin();

  Serial.println(F("VL53L5CX multizone"));
  Serial.println(F("Driver: https://github.com/stm32duino/VL53L5CX"));

  stTof.begin();
  if (stTof.init_sensor() != 0) {
    Serial.println(F("init failed"));
    while (1) {
      delay(500);
    }
  }
  stTof.vl53l5cx_start_ranging();
}

void loop() {
  uint8_t ready = 0;
  stTof.vl53l5cx_check_data_ready(&ready);
  if (ready) {
    VL53L5CX_ResultsData results;
    if (stTof.vl53l5cx_get_ranging_data(&results) == 0) {
      Serial.print(F("zone0_mm="));
      Serial.println(results.distance_mm[0]);
    }
  }
  delay(100);
}
