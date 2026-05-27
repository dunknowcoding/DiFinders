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
#define MY_TOF_I2C_ADDR DF_ADDR_VL6180X

VL6180X tofSensor;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(100000);
  if (!tofSensor.begin(Wire, MY_TOF_I2C_ADDR)) {
    Serial.println(F("init failed"));
    while (1) { delay(500); }
  }
  tofSensor.setTimeout(500);
  Serial.println(F("VL6180X AmbientRead ready"));
}

void loop() {
  uint16_t ambient = tofSensor.readAmbient();
  Serial.print(F("distance_mm="));
  Serial.println(ambient);
  delay(120);
}
