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
#define MY_LEFT_XSHUT 2
#define MY_RIGHT_XSHUT 3
#define MY_LEFT_ADDR 0x30
#define MY_RIGHT_ADDR 0x31

VL53L0X tofSensorLeft;
VL53L0X tofSensorRight;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(100000);
  tofSensorLeft.attachShutdownPin(MY_LEFT_XSHUT);
  tofSensorRight.attachShutdownPin(MY_RIGHT_XSHUT);
  tofSensorLeft.powerOff();
  tofSensorRight.powerOff();
  delay(20);

  tofSensorLeft.powerOn();
  delay(20);
  if (!tofSensorLeft.begin(Wire, DF_ADDR_VL53L0X)) {
    Serial.println(F("init failed"));
    while (1) { delay(500); }
  }
  if (!tofSensorLeft.setAddress(MY_LEFT_ADDR)) {
    Serial.println(F("L"));
    while (1) { delay(500); }
  }
  delay(5);

  tofSensorRight.powerOn();
  delay(20);
  if (!tofSensorRight.begin(Wire, DF_ADDR_VL53L0X)) {
    Serial.println(F("init failed"));
    while (1) { delay(500); }
  }
  if (!tofSensorRight.setAddress(MY_RIGHT_ADDR)) {
    Serial.println(F("R"));
    while (1) { delay(500); }
  }
  delay(5);
  Serial.println(F("VL53L0X AddressAssign ready"));
}

void loop() {
  RangeReading leftReading = tofSensorLeft.read();
  delay(5);
  RangeReading rightReading = tofSensorRight.read();
  Serial.print(F("distance_mm="));
  Serial.print(leftReading.distanceMm);
  Serial.print(F("distance_mm="));
  Serial.println(rightReading.distanceMm);
  delay(100);
}
