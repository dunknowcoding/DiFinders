/*
 * E18_D80NK_BasicRead.ino — infrared barrier (digital OUT)
 *
 * Reference board: Arduino UNO R4 Minima
 *   Brown=VCC, Blue=GND, Black=OUT -> pin 4 (active-low when blocked)
 */

#include <DiFinders.h>

const uint8_t OUT_PIN = 4;

E18_D80NK barrier(OUT_PIN);

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }
  barrier.begin(OUT_PIN);
  Serial.println(F("E18-D80NK ready"));
}

void loop() {
  barrier.read();
  Serial.print(F("detected="));
  Serial.println(barrier.detected() ? 1 : 0);
  delay(100);
}
