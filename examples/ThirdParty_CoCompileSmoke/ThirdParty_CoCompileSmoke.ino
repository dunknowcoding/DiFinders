/*
 * ThirdParty_CoCompileSmoke.ino
 *
 * Link test: DiFinders (VL53L1X single-zone) + STM32duino VL53L8CX (8x8 multizone)
 * in one sketch. No hardware required for linking.
 *
 * Vendor driver (required for VL53L8CX):
 *   https://github.com/stm32duino/VL53L8CX
 *   Local copy: drivers/VL53L8CX  (see drivers/README.md)
 *
 * Compile:
 *   arduino-cli compile --fqbn arduino:renesas_uno:minima \
 *     --library . --library drivers/VL53L8CX examples/ThirdParty_CoCompileSmoke
 */

#include <DiFinders.h>
#include <Wire.h>
#include <vl53l8cx.h>

#define MY_LPN_PIN 2

VL53L1X difindersSingleZone;
VL53L8CX stMultizone(&Wire, MY_LPN_PIN);

void setup() {
  Serial.begin(115200);
  Wire.begin();
  (void)stMultizone;
}

void loop() {
  delay(1000);
}
