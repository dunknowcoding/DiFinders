/*
 * WT53R485_BasicRead.ino — Modbus RS485 distance (WT53 family)
 *
 * Reference board: Arduino UNO R4 Minima
 *   USB monitor : Serial @ 115200
 *   Sensor UART : Serial1 @ 115200 (RS485 adapter TX/RX to module)
 */

#include <DiFinders.h>

WT53R485 distanceSensor;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }
  Serial.println(F("WT53R RS485 Modbus"));

#if defined(HAVE_HWSERIAL1) || defined(SERIAL_PORT_HARDWARE1)
  Serial1.begin(115200);
  distanceSensor.begin(Serial1);
  distanceSensor.setDeviceAddress(0x01);
  distanceSensor.setDistanceRegister(0x0000, 1, 0x04);
  distanceSensor.setScaling(1, 1);
  Serial.println(F("WT53R485 ready"));
#else
  Serial.println(F("Need hardware Serial1 (UNO R4, Mega, ESP32, RP2040)."));
#endif
}

void loop() {
#if !defined(HAVE_HWSERIAL1) && !defined(SERIAL_PORT_HARDWARE1)
  delay(1000);
  return;
#endif

  RangeReading r = distanceSensor.read();
  if (r.status == SensorStatus::Ok || r.status == SensorStatus::OutOfRange) {
    Serial.print(F("distance_mm="));
    Serial.println(r.distanceMm);
  } else {
    Serial.print(F("status="));
    Serial.println(static_cast<uint8_t>(r.status));
  }
  delay(100);
}
