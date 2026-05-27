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

VL53L4CD tofSensorLeft;
VL53L4CD tofSensorRight;

void stopBothSensors() {
  tofSensorLeft.powerOff();
  tofSensorRight.powerOff();
  delay(20);
}

bool startOneSensor(VL53L4CD& sensor, uint8_t newAddress, uint32_t timingBudgetMs, uint32_t interMeasurementMs) {
  sensor.powerOn();
  delay(20);
  if (!sensor.begin(Wire, DF_ADDR_VL53L1X)) {
    return false;
  }
  if (!sensor.setAddress(newAddress)) {
    return false;
  }
  if (!sensor.setRangeTiming(timingBudgetMs, interMeasurementMs)) {
    return false;
  }
  sensor.startContinuous(interMeasurementMs);
  delay(10);
  return true;
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(100000);
  Wire.setClock(400000);
  tofSensorLeft.attachShutdownPin(MY_LEFT_XSHUT);
  tofSensorRight.attachShutdownPin(MY_RIGHT_XSHUT);
  stopBothSensors();
  if (!startOneSensor(tofSensorLeft, MY_LEFT_ADDR, 20, 25)) {
    Serial.println(F("init failed"));
    while (1) { delay(500); }
  }
  if (!startOneSensor(tofSensorRight, MY_RIGHT_ADDR, 20, 30)) {
    Serial.println(F("init failed"));
    while (1) { delay(500); }
  }
  Serial.println(F("VL53L4CD DualSensorRead ready"));
}

void loop() {
  RangeReading reading;
  if (tofSensorLeft.readIfReady(reading)) {
    Serial.print(F("distance_mm="));
    Serial.println(reading.distanceMm);
  }
  delay(2);
  if (tofSensorRight.readIfReady(reading)) {
    Serial.print(F("distance_mm="));
    Serial.println(reading.distanceMm);
  }
  delay(10);
}
