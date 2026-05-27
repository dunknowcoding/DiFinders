/*
 * HLK_LD2410_Threshold.ino — gate distance / sensitivity (UART configuration)
 *
 * Reference board: Arduino UNO R4 Minima
 *   Sensor UART: Serial1 @ 256000 (legacy LD2410 firmware; try 115200 if no data)
 *   MCU RX=D0, TX=D1
 *
 * Adjusts max detection gates then prints presence and distances.
 */

#include <DiFinders.h>

HLK_LD2410_UART radar;

void setup() {
  Serial.begin(115200);

#if defined(HAVE_HWSERIAL1) || defined(SERIAL_PORT_HARDWARE1)
  Serial1.begin(256000);
  radar.begin(Serial1);
  delay(200);
  radar.setMaxValues(8, 8, 5);
  radar.setGateSensitivity(0, 50, 40);
  Serial.println(F("LD2410 gate/threshold config sent"));
#else
  Serial.println(F("Need Serial1."));
#endif
}

void loop() {
#if !defined(HAVE_HWSERIAL1) && !defined(SERIAL_PORT_HARDWARE1)
  return;
#endif
  radar.read();
  Serial.print(F("presence="));
  Serial.print(radar.presenceDetected() ? 1 : 0);
  Serial.print(F(" move_cm="));
  Serial.print(radar.movingTargetDistanceCm());
  Serial.print(F(" still_cm="));
  Serial.println(radar.stationaryTargetDistanceCm());
  delay(200);
}
