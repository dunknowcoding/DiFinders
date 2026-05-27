/*
 * HLK_LD2410S_UART_BasicRead.ino - mmWave UART presence / distance
 *
 * Reference board: Arduino UNO R4 Minima - Serial1 @ 115200
 * Why Serial1 (not Serial): see A02YYUW_BasicRead.ino.
 */

#include <DiFinders.h>

HLK_LD2410S_UART presenceSensor;

void setup() {
  Serial.begin(115200);
  Serial.println(F("HLK-LD2410S UART"));

#if defined(HAVE_HWSERIAL1) || defined(SERIAL_PORT_HARDWARE1)
  Serial1.begin(115200);
  presenceSensor.begin(Serial1);
#else
  Serial.println(F("Need hardware Serial1 (UNO R4, Mega, ESP32, RP2040)."));
#endif
}

void loop() {
#if !defined(HAVE_HWSERIAL1) && !defined(SERIAL_PORT_HARDWARE1)
  return;
#endif

  presenceSensor.read();
  Serial.print(F("motion="));
  if (df_hit(presenceSensor.motion())) {
    Serial.println(F("yes"));
  } else {
    Serial.println(F("no"));
  }
  delay(100);
}
