/*
 * VL53L1X_HighSpeedRead.ino — measure sample rate (I2C continuous + readIfReady)
 *
 * Not hardware I2C DMA (Arduino Wire has no user DMA). Tight timing budget + short period.
 */

#include <DiFinders.h>
#include <Wire.h>

const uint8_t MY_ADDR = 0x29;

VL53L1X tof;

uint32_t windowStartMs = 0;
uint32_t sampleCount = 0;
uint32_t timeoutCount = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000);

  if (!tof.begin(Wire, MY_ADDR)) {
    Serial.println(F("init failed"));
    while (1) {
      delay(500);
    }
  }

  tof.setTimeout(80);
  tof.useShortDistanceMode();
  tof.setMeasurementTimingBudget(15000);
  tof.startContinuous(15);
  windowStartMs = millis();
  Serial.println(F("VL53L1X high rate stats"));
}

void loop() {
  RangeReading reading;
  if (tof.readIfReady(reading)) {
    sampleCount++;
    if (reading.status == SensorStatus::Timeout) {
      timeoutCount++;
    }
  }
  if (tof.timeoutOccurred()) {
    timeoutCount++;
  }

  if ((millis() - windowStartMs) >= 1000) {
    Serial.print(F("samples_per_s="));
    Serial.print(sampleCount);
    Serial.print(F(" timeouts="));
    Serial.print(timeoutCount);
    Serial.print(F(" last_mm="));
    Serial.println(tof.lastReading().distanceMm);
    sampleCount = 0;
    timeoutCount = 0;
    windowStartMs = millis();
  }
}
