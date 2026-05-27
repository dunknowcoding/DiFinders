/*
 * TCRT5000_ReflectiveRead.ino
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
#define MY_DIGITAL_PIN DF_PIN_LINE_DIG
#define MY_ANALOG_PIN DF_PIN_LINE_ANA
#define MY_EMITTER_PIN DF_PIN_LINE_EMIT

TCRT5000 irSensor(MY_DIGITAL_PIN, MY_ANALOG_PIN, MY_EMITTER_PIN);

void setup() {
  Serial.begin(115200);
  irSensor.setCalibrationWindow(40, 850);
  irSensor.setAnalogThresholdPermille(250);
  Serial.println(F("TCRT5000 ReflectiveRead ready"));
}

void loop() {
  ProximityReading reading = irSensor.read();
  int active = 0;
  if (reading.detected()) {
    active = 1;
  }
  Serial.print(F(active);
  Serial.print(F(reading.strengthPermille);
  Serial.print(F("value="));
  Serial.println(reading.rawValue);
  delay(50);
}
