/*
 * VL53L1X_SpiBurstRead.ino — VL53L1X on SPI with interrupt-driven reads
 *
 * On ESP32 (DI_FINDERS_ENABLE_DMA), the driver may use SPI.transferBytes for
 * burst register reads. This is not the same as Arduino Wire DMA (unavailable).
 *
 * SPI.begin() uses your board's default SPI pins. Set MY_CS to your CS pin.
 *   UNO R4 / Mega : CS=10, MOSI=11, MISO=12, SCK=13
 *   ESP32         : CS=5 typical; see board SPI pinout
 *   Pico          : CS=17 typical on SPI0
 *
 * Optional INT: GPIO1 on module -> MY_INT.
 */

#include <DiFinders.h>
#include <SPI.h>

const uint8_t MY_CS = 10;
const uint8_t MY_INT = 7;

VL53L1X_SPI tof;

void setup() {
  Serial.begin(115200);
  SPI.begin();

  if (!tof.begin(MY_CS, 8000000UL)) {
    Serial.println(F("VL53L1X SPI init failed"));
    while (1) {
      delay(500);
    }
  }

  tof.setTimeout(200);
  tof.startContinuous(20);
  tof.attachDataReadyPin(MY_INT, DF_IRQ_FALLING);
  Serial.print(F("dma_mode="));
  Serial.println(dmaModeName());
}

void loop() {
  RangeReading r;
  if (tof.readIfReady(r)) {
    Serial.print(F("distance_mm="));
    Serial.println(r.distanceMm);
  }
}
