/*
 * Benewake_TF03_CAN_SN65HVD230_ESP32.ino
 *
 * Sensor     : Benewake TF03 (CAN mode)
 * Transceiver: TI SN65HVD230 (3.3 V, ISO 11898-2 high-speed)
 * MCU        : ESP32 built-in TWAI controller (no MCP2515)
 *
 * No extra Arduino library — uses ESP32 core driver/twai.h.
 *
 * Wiring (Espressif TWAI example convention):
 *   ESP32 GPIO MY_TWAI_TX -> SN65HVD230 TXD (pin CTX on some boards)
 *   ESP32 GPIO MY_TWAI_RX -> SN65HVD230 RXD (pin CRX)
 *   SN65HVD230 CANH -> TF03 CAN_H (green)
 *   SN65HVD230 CANL -> TF03 CAN_L (white)
 *   SN65HVD230 Rs   -> GND (high-speed slope)
 *   3.3 V / GND shared; TF03 powered per manual (5 V typical)
 *   120 ohm between CAN_H and CAN_L at each bus end
 *
 * Library: https://github.com/espressif/arduino-esp32 (TWAI built-in)
 */

#include "driver/twai.h"

#include <DiFinders.h>

#define MY_TWAI_TX 22
#define MY_TWAI_RX 21

Benewake_TF03_CAN tf03;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }

  printBoardInfo();
  Serial.print(F("can_controller="));
  Serial.println(canControllerLabel());
  Serial.println(F("Benewake TF03 over SN65HVD230 + ESP32 TWAI"));

  twai_general_config_t generalConfig =
      TWAI_GENERAL_CONFIG_DEFAULT((gpio_num_t)MY_TWAI_TX, (gpio_num_t)MY_TWAI_RX, TWAI_MODE_NORMAL);
  twai_timing_config_t timingConfig = TWAI_TIMING_CONFIG_500KBITS();
  twai_filter_config_t filterConfig = TWAI_FILTER_CONFIG_ACCEPT_ALL();

  if (twai_driver_install(&generalConfig, &timingConfig, &filterConfig) != ESP_OK) {
    Serial.println(F("TWAI driver install failed"));
    while (1) {
      delay(500);
    }
  }
  if (twai_start() != ESP_OK) {
    Serial.println(F("TWAI start failed"));
    while (1) {
      delay(500);
    }
  }
  Serial.print(F("listening_can_id=0x"));
  Serial.println(tf03.transmitId(), HEX);
}

void loop() {
  twai_message_t message;
  if (twai_receive(&message, pdMS_TO_TICKS(10)) != ESP_OK) {
    return;
  }

  const bool extended = (message.flags & TWAI_MSG_FLAG_EXTD) != 0;
  if (!tf03.onCanFrame(message.identifier, message.data, message.data_length_code, extended)) {
    return;
  }

  RangeReading r = tf03.read();
  if (df_ok(r)) {
    Serial.print(F("distance_mm="));
    Serial.print(r.distanceMm);
    Serial.print(F(" strength="));
    Serial.println(tf03.lastStrength());
  } else {
    Serial.print(F("status="));
    Serial.println(static_cast<uint8_t>(r.status));
  }
}
