/*
 * HLK_LD2412_PresenceRead.ino — digital OUT pin (not UART firmware)
 *
 * Some LD2412 breakouts expose a simple occupied/high GPIO (OT pin).
 * For UART distance/presence frames use HLK_LD2412_UART_BasicRead.ino instead.
 *
 * Wiring: module OT/OUT -> MY_PIN, GND common, 3.3 V power on 3.3 V boards.
 */

#include <DiFinders.h>

const uint8_t MY_PIN = 4;

HLK_LD2412_OUT presence(MY_PIN);

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }
  presence.begin();
  Serial.println(F("HLK-LD2412 digital OUT"));
}

void loop() {
  presence.read();
  Serial.print(F("occupied="));
  Serial.println(presence.presence() ? 1 : 0);
  delay(100);
}
