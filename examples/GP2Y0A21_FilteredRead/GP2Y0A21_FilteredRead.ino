/*
 * GP2Y0A21_FilteredRead.ino
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
#define MY_ANALOG_PIN DF_PIN_ANA

  // df_adc(DF_ADC_REF), df_adc(): ADC reference mV and max counts for this MCU
GP2Y0A21 irSensor(MY_ANALOG_PIN, df_adc(DF_ADC_REF), df_adc());

void setup() {
  Serial.begin(115200);
  Serial.println(F("GP2Y0A21 FilteredRead ready"));
}

void loop() {
  uint16_t avgMm = irSensor.averageMillimeters(5);
  uint16_t medMm = irSensor.medianMillimeters();
  int nearFlag = 0;
  if (irSensor.detectWithin(120, 500)) {
    nearFlag = 1;
  }
  Serial.print(F("avg_mm="));
  Serial.print(avgMm);
  Serial.print(F(" median_mm="));
  Serial.print(medMm);
  Serial.print(F(" near="));
  Serial.println(nearFlag);
  delay(100);
}
