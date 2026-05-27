/*
 * Reference board: Arduino UNO R4 Minima (tested)
 * USB monitor : Serial @ 115200 (built-in USB)
 * Sensor pins : see MY_* below (change for your board)
 *
 * Compatibility:
 *   UNO R4 Minima/WiFi : default pins D2/D3 work as digital I/O
 *   AVR UNO/Nano       : D2 Trig, D3 Echo - 5 V logic, same wiring
 *   ESP32              : use any safe GPIO (avoid strapping pins); 3.3 V logic for Echo if module is 3.3 V
 *   RP2040 Pico        : any GPx; prefer 3.3 V-compatible modules on 3.3 V boards
 * Board compatibility (Arduino IDE):
 *   AVR UNO/Nano     : pulse/GPIO only; no hardware Serial1 on classic UNO
 *   UNO R4 Minima/WiFi: full Serial1, 3.3 V I2C, recommended for UART/ToF
 *   Mega 2560        : multiple UART, 5 V pulse sensors
 *   ESP32 / ESP32-S3 / C3 / H2 / P4 : 3.3 V, TWAI/SPI, large UART buffers
 *   RP2040 / Pico 2  : flexible GPIO/UART mux
 *   Nano 33 IoT (SAMD): 3.3 V I2C/UART
 *   Nano 33 BLE (nRF52840): 3.3 V, BLE, Serial1
 *   STM32 / Portenta (Mbed): 3.3 V when supported by core
 * Wiring: TRIG + ECHO to GPIO (see MY_TRIG/MY_ECHO); 5 V modules may need divider on ECHO to 3.3 V MCU.
 *
 */

#include <DiFinders.h>
#define MY_TRIG_PIN DF_PIN_TRIG
#define MY_ECHO_PIN DF_PIN_ECHO

JSN_SR04T distanceSensor(MY_TRIG_PIN, MY_ECHO_PIN);

void setup() {
  Serial.begin(115200);
  Serial.println(F("JSN SR04T BasicRead ready"));
}

void loop() {
  uint16_t mm = distanceSensor.medianMillimeters();
  Serial.print(F("distance_mm="));
  Serial.println(mm);
  delay(120);
}
