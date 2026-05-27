/*
 * HLK_LD2412_UART_BasicRead.ino - mmWave UART presence / distance
 *
 * Reference board: Arduino UNO R4 Minima
 *   USB monitor : Serial @ 115200
 *   Sensor UART : Serial1 @ 115200 - MCU RX=D0, TX=D1
 *
 * Why Serial1 (not Serial): USB debug stays on Serial; LD2412 binary protocol
 *   runs on Serial1 at 115200 on dedicated MCU pins (see A02YYUW_BasicRead.ino).
 */

#include <DiFinders.h>

HLK_LD2412_UART presenceSensor;

void setup() {
  Serial.begin(115200);
  Serial.println(F("HLK-LD2412 UART"));

#if defined(HAVE_HWSERIAL1) || defined(SERIAL_PORT_HARDWARE1)
  Serial1.begin(115200);
  presenceSensor.begin(Serial1);
#else
  Serial.println(F("Need hardware Serial1 (UNO R4, Mega, ESP32, RP2040)."));
#endif
}

void loop() {
#if !defined(HAVE_HWSERIAL1) && !defined(SERIAL_PORT_HARDWARE1)
  delay(500);
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
