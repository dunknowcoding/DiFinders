#pragma once

/**
 * DiFindersConstants.h — wiring defaults and sensor-specific symbols only.
 * Use literal numbers in sketches for baud, delay, and I2C clock (115200, 120, 100000, …).
 * Override pins in .ino with MY_* aliases when needed.
 */

/* Default pins (optional; use numeric pins in .ino if you prefer) */
#define DF_PIN_TRIG 2
#define DF_PIN_ECHO 3
#if defined(ARDUINO_ARCH_ESP32)
#define DF_PIN_ANA 34
#define DF_PIN_GP2E03_ANA 34
#elif defined(ARDUINO_ARCH_RP2040)
#define DF_PIN_ANA 26
#define DF_PIN_GP2E03_ANA 26
#else
#define DF_PIN_ANA A0
#define DF_PIN_GP2E03_ANA A0
#endif
#define DF_PIN_DIG 4
#define DF_PIN_LINE_DIG 4
#if defined(ARDUINO_ARCH_ESP32)
#define DF_PIN_LINE_ANA 35
#elif defined(ARDUINO_ARCH_RP2040)
#define DF_PIN_LINE_ANA 27
#else
#define DF_PIN_LINE_ANA A1
#endif
#define DF_PIN_LINE_EMIT 5
#define DF_PIN_PIR 6
#define DF_PIN_MW 4
#define DF_PIN_E18 4
#define DF_PIN_TOF_INT 7
#define DF_PIN_SPI_CS 10
#define DF_PIN_NONE 0xFF

/* I2C 7-bit addresses (chip defaults; not Arduino core macros) */
#define DF_ADDR_VL53L0X 0x29
#define DF_ADDR_VL53L1X 0x29
#define DF_ADDR_VL6180X 0x29
#define DF_ADDR_GY_US42 0x70
#define DF_ADDR_GP2Y0E03 0x40
#define DF_ADDR_TOFSENSE 0x08

/* df_adc() overload selectors */
#define DF_ADC_MAX 0U
#define DF_ADC_REF 1U

/* Optional median helper — prefer sensor.medianMillimeters() in new sketches */
#define DF_MEDIAN_MM(S) ((S).medianMillimeters())
