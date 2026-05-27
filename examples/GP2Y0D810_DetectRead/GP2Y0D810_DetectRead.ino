/*
 * GP2Y0D810_DetectRead.ino
 * Board compatibility (Arduino IDE):
 *   AVR UNO/Nano     : pulse/GPIO only; no hardware Serial1 on classic UNO
 *   UNO R4 Minima/WiFi: full Serial1, 3.3 V I2C, recommended for UART/ToF
 *   Mega 2560        : multiple UART, 5 V pulse sensors
 *   ESP32 / ESP32-S3 / C3 / H2 / P4 : 3.3 V, TWAI/SPI, large UART buffers
 *   RP2040 / Pico 2  : flexible GPIO/UART mux
 *   Nano 33 IoT (SAMD): 3.3 V I2C/UART
 *   Nano 33 BLE (nRF52840): 3.3 V, BLE, Serial1
 *   STM32 / Portenta (Mbed): 3.3 V when supported by core
 * Wiring: analog or digital pin per MY_* defines; 3.3 V ADC reference on 3.3 V boards.
 */


#include <DiFinders.h>
#define MY_DIGITAL_PIN DF_PIN_DIG

GP2Y0D810 irSensor(MY_DIGITAL_PIN);

void setup() {
  Serial.begin(115200);
  Serial.println(F("GP2Y0D810 DetectRead ready"));
}

void loop() {
  ProximityReading reading = irSensor.read();
  int detected = 0;
  if (reading.detected()) {
    detected = 1;
  }
  Serial.print(F(detected);
  Serial.print(F("distance_mm="));
  Serial.print(irSensor.minMillimeters());
  Serial.print(F("-"));
  Serial.println(irSensor.maxMillimeters());
  delay(30);
}
