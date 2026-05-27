/*
 * HLK_LD2461_UART_BasicRead.ino — multi-target mmWave UART tracking
 *
 * Reference board: Arduino UNO R4 Minima
 *   USB monitor : Serial @ 115200
 *   Sensor UART : Serial1 @ 115200 (module default; cross TX/RX)
 */

#include <DiFinders.h>

HLK_LD2461_UART presenceSensor;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }
  Serial.println(F("HLK-LD2461 UART tracking"));

#if defined(HAVE_HWSERIAL1) || defined(SERIAL_PORT_HARDWARE1)
  presenceSensor.setDefaultBaudRate(115200);
  presenceSensor.setTargetFreshnessTimeout(600);
  Serial1.begin(presenceSensor.defaultBaudRate());
  presenceSensor.begin(Serial1);
  presenceSensor.requestTrackingSnapshot();
  Serial.println(F("HLK_LD2461 ready"));
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

  Serial.print(F("targets="));
  Serial.print(presenceSensor.targetCount());
  Serial.print(F(" fresh="));
  Serial.print(presenceSensor.freshTargetCount());
  Serial.print(F(" occupied="));
  Serial.print(presenceSensor.occupied() ? 1 : 0);
  Serial.print(F(" nearest_mm="));
  Serial.print(presenceSensor.nearestTargetDistanceMm());
  Serial.print(F("status="));
  Serial.println(static_cast<uint8_t>(m.status));

  presenceSensor.requestTrackingSnapshot();
  delay(150);
}
