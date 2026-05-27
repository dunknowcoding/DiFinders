/*
 * HLK_LD2420_UART_BasicRead.ino — mmWave UART (legacy/modern baud)
 *
 * Reference board: Arduino UNO R4 Minima
 *   USB monitor : Serial @ 115200
 *   Sensor UART : Serial1 @ baud from defaultBaudRate() after profile selection
 */

#include <DiFinders.h>

HLK_LD2420_UART presenceSensor;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }
  Serial.println(F("HLK-LD2420 UART"));

#if defined(HAVE_HWSERIAL1) || defined(SERIAL_PORT_HARDWARE1)
  presenceSensor.useLegacyDefaultBaudRate();
  Serial1.begin(presenceSensor.defaultBaudRate());
  presenceSensor.begin(Serial1);
  presenceSensor.applyBalancedIndoorProfile();
  Serial.print(F("baud="));
  Serial.println(presenceSensor.defaultBaudRate());
  Serial.print(F("profile="));
  Serial.println(presenceSensor.isUsingLegacyDefaultBaudRate() ? F("legacy-256000") : F("modern-115200"));
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
  MotionReading m = presenceSensor.motion();
  Serial.print(F("presence="));
  Serial.println(m.detected() ? 1 : 0);
  Serial.print(F("moving_cm="));
  Serial.print(presenceSensor.movingTargetDistanceCm());
  Serial.print(F(" stationary_cm="));
  Serial.println(presenceSensor.stationaryTargetDistanceCm());
  delay(100);
}
