/*
 * VL53L1X_SPI.ino — VL53L1X / VL53L1CB on SPI (I2C modules: VL53L1X_BasicRead.ino)
 *
 * Uses the Arduino SPI library (SPI.begin) and the library driver for chip-select.
 *
 * SPI wiring (hardware SPI bus — check your board pinout):
 *   Arduino UNO R4 / Mega : CS=10, MOSI=11, MISO=12, SCK=13
 *   ESP32 DevKit (VSPI)   : CS=5 (change MY_CS if needed), default MOSI/MISO/SCK
 *   Raspberry Pi Pico     : CS=17 typical, SPI0 default pins on pin diagram
 *
 * Module: 3.3 V, GND, CS, SCK, MISO, MOSI; optional GPIO1 -> MY_INT for data-ready.
 */

#include <DiFinders.h>
#include <SPI.h>

const uint8_t MY_CS = 10;
const uint8_t MY_INT = 7;

VL53L1X_SPI tof;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }

  SPI.begin();

  if (!tof.begin(MY_CS, 8000000UL)) {
    Serial.println(F("VL53L1X SPI init failed"));
    while (1) {
      delay(500);
    }
  }

  tof.setTimeout(500);
  tof.startContinuous(60);
  tof.attachDataReadyPin(MY_INT, DF_IRQ_FALLING);
  Serial.println(F("VL53L1X SPI ready"));
}

void loop() {
  RangeReading reading;
  if (tof.takeDataReadyEvent() || tof.dataReady()) {
    if (tof.readIfReady(reading)) {
      Serial.print(F("distance_mm="));
      Serial.println(reading.distanceMm);
    }
  }
  delay(5);
}
