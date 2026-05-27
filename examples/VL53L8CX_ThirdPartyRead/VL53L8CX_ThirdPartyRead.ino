/*
 * VL53L8CX_ThirdPartyRead.ino - 8x8 multizone ToF (NOT in DiFinders mainline)
 *
 * You must use the STMicroelectronics Arduino driver (ULD-based):
 *   https://github.com/stm32duino/VL53L8CX
 * Vendored copy in this repo: drivers/VL53L8CX  (tag 2.0.4 in drivers/manifest.json)
 * Datasheet: https://www.st.com/en/imaging-and-photonics-solutions/vl53l8cx.html
 *
 * Reference board: Arduino UNO R4 Minima (needs RAM; AVR UNO is too small)
 *   USB monitor : Serial @ 115200 (or 460800 like ST demo if you prefer)
 *   I2C         : Wire SDA=A4, SCL=A5
 *   LPn         : MY_LPN_PIN (default 2)
 *
 * DiFinders VL53L1X/VL53L4CX give one range + SPAD ROI only. For 64 cells use this sketch.
 *
 * Compile with both libraries:
 *   arduino-cli compile --fqbn arduino:renesas_uno:minima \
 *     --library . --library drivers/VL53L8CX examples/VL53L8CX_ThirdPartyRead
 */

#include <DiFinders.h>
#include <Wire.h>
#include <vl53l8cx.h>

#define MY_LPN_PIN 2
#define MY_PWREN_PIN -1

VL53L1X difindersAliasTof;
VL53L8CX stTof(&Wire, MY_LPN_PIN);

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }

  if (MY_PWREN_PIN >= 0) {
    pinMode(MY_PWREN_PIN, OUTPUT);
    digitalWrite(MY_PWREN_PIN, HIGH);
    delay(10);
  }

  Wire.begin();
  Wire.setClock(100000);

  Serial.println(F("VL53L8CX multizone via STM32duino driver"));
  Serial.println(F("Driver: https://github.com/stm32duino/VL53L8CX"));
  stTof.begin();
  uint8_t status = stTof.init();
  if (status != 0) {
    Serial.print(F("VL53L8CX init status="));
    Serial.println(status);
    while (1) {
      delay(500);
    }
  }

  stTof.set_resolution(VL53L8CX_RESOLUTION_4X4);
  status = stTof.start_ranging();
  if (status != 0) {
    Serial.print(F("start_ranging status="));
    Serial.println(status);
  }
}

void loop() {
  uint8_t ready = 0;
  uint8_t status = 0;

  do {
    status = stTof.check_data_ready(&ready);
  } while (!ready && status == 0);

  if (status == 0 && ready) {
    VL53L8CX_ResultsData results;
    status = stTof.get_ranging_data(&results);
    if (status == 0) {
      Serial.print(F("zone0_mm="));
      Serial.println(results.distance_mm[0]);
    }
  }

  delay(50);
}
