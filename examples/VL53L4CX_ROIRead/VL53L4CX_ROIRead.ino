/*
 * Reference board: Arduino UNO R4 Minima (tested)
 * USB monitor : Serial @ 115200
 * I2C bus     : Wire - SDA=A4, SCL=A5 on UNO/R4 (fixed pins)
 *
 * Compatibility:
 *   UNO R4 / AVR UNO : Wire on A4/A5, 3.3 V or 5 V per module datasheet
 *   ESP32            : default SDA/SCL GPIO21/22 (board-specific)
 *   RP2040 Pico      : Wire1 optional; default Wire on GP6/GP7 (Pico pinout)
 * Board compatibility (Arduino IDE):
 *   AVR UNO/Nano     : pulse/GPIO only; no hardware Serial1 on classic UNO
 *   UNO R4 Minima/WiFi: full Serial1, 3.3 V I2C, recommended for UART/ToF
 *   Mega 2560        : multiple UART, 5 V pulse sensors
 *   ESP32 / ESP32-S3 / C3 / H2 / P4 : 3.3 V, TWAI/SPI, large UART buffers
 *   RP2040 / Pico 2  : flexible GPIO/UART mux
 *   Nano 33 IoT (SAMD): 3.3 V I2C/UART
 *   Nano 33 BLE (nRF52840): 3.3 V, BLE, Serial1
 *   STM32 / Portenta (Mbed): 3.3 V when supported by core
 * Wiring (typical UNO R4):
 *   I2C: SDA=A4, SCL=A5 (or Wire pins for your board), 3.3 V + GND, level-shift if module is 3.3 V only
 *
 */

#include <DiFinders.h>
#include <Wire.h>
#define MY_TOF_I2C_ADDR DF_ADDR_VL53L1X
#define MY_ROI_WIDTH 8
#define MY_ROI_HEIGHT 8
#define MY_ROI_CENTER 199

VL53L4CX tofSensor;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(100000);
  Wire.setClock(400000);
  if (!tofSensor.begin(Wire, MY_TOF_I2C_ADDR)) {
    Serial.println(F("init failed"));
    while (1) { delay(500); }
  }
  tofSensor.setTimeout(1000);
  tofSensor.setROISize(MY_ROI_WIDTH, MY_ROI_HEIGHT);
  tofSensor.setROICenter(MY_ROI_CENTER);
  tofSensor.setRangeTiming(40, 60);
  tofSensor.startContinuous(60);
  Serial.println(F("VL53L4CX ROIRead ready"));
}

void loop() {
  uint8_t roiWidth = 0;
  uint8_t roiHeight = 0;
  tofSensor.getROISize(roiWidth, roiHeight);
  RangeReading reading = tofSensor.read();
  Serial.print(F("distance_mm="));
  Serial.print(reading.distanceMm);
  Serial.print(F(" ROI: "));
  Serial.print(roiWidth);
  Serial.print(F("x"));
  Serial.print(roiHeight);
  Serial.print(F("value="));
  Serial.println(tofSensor.roiCenter());
  delay(80);
}
