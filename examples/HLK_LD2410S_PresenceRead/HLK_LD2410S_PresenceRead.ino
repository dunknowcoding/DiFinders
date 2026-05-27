/*
 * HLK_LD2410S_PresenceRead.ino — LD2410S digital OUT pin
 *
 * Reads the module OT output (high = target on many boards).
 * For UART mode use HLK_LD2410S_UART_BasicRead.ino.
 */

#include <DiFinders.h>

const uint8_t MY_PIN = 4;

HLK_LD2410S_OUT presence(MY_PIN);

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }
  presence.begin();
  Serial.println(F("HLK-LD2410S digital OUT"));
}

void loop() {
  presence.read();
  Serial.print(F("occupied="));
  Serial.println(presence.presence() ? 1 : 0);
  delay(100);
}
