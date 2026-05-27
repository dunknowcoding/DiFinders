/*
 * HC_SR04_Interrupt.ino
 *
 * HC-SR04 ranging with echo-pin CHANGE interrupts instead of pulseIn().
 * Sequence: startPing() -> wait takeEchoEvent() -> completePing().
 *
 * Wiring: Trig -> MY_TRIG, Echo -> MY_ECHO (5V tolerant board recommended).
 */

#include <DiFinders.h>

#define MY_TRIG DF_PIN_TRIG
#define MY_ECHO DF_PIN_ECHO

HC_SR04 sonar(MY_TRIG, MY_ECHO);

void setup() {
  df_serial();
  Serial.println(F("HC_SR04 interrupt echo"));
  sonar.attachEchoInterrupt(DF_IRQ_CHANGE);
}

void loop() {
  sonar.startPing();
  unsigned long waitStart = millis();
  while (!sonar.takeEchoEvent()) {
    if ((millis() - waitStart) > 35UL) {
      Serial.println(F("echo timeout"));
      delay(DF_LOOP_MS);
      return;
    }
  }
  RangeReading reading = sonar.completePing();
  df_show(reading);
  delay(DF_LOOP_MS);
}
