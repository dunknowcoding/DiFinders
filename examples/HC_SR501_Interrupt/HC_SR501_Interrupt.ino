/*
 * HC_SR501_Interrupt.ino
 *
 * HC-SR501 PIR on DF_PIN_PIR with attachInterruptPin() so the main loop
 * only prints when the OUT line changes (no polling delay).
 *
 * Wiring: PIR OUT -> MY_PIR_PIN, VCC 5V, GND common.
 * Edit MY_* only; see docs/NAMING.md.
 */

#include <DiFinders.h>

#define MY_PIR_PIN DF_PIN_PIR

HC_SR501 motionSensor(MY_PIR_PIN);

void setup() {
  df_serial();
  Serial.println(F("HC_SR501 interrupt demo"));
  motionSensor.attachInterruptPin(MY_PIR_PIN, DF_IRQ_RISING);
}

void loop() {
  if (!motionSensor.takeInterruptEvent()) {
    return;
  }
  MotionReading reading = motionSensor.read();
  int motion = 0;
  if (reading.detected()) {
    motion = 1;
  }
  Serial.print(F("motion="));
  Serial.println(motion);
}
