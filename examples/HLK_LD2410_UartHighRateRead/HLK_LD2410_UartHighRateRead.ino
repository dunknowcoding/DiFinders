/*
 * HLK_LD2410_UartHighRateRead.ino — LD2410 at 256000 baud with large RX buffer (ESP32)
 *
 * On ESP32, beginSensorUart() increases the hardware UART RX buffer so radar
 * frames are less likely to be dropped at high rate. This is not peripheral DMA.
 *
 * Serial  = USB monitor @ 115200
 * Serial1 = sensor @ 256000 (UNO R4: RX=D0, TX=D1)
 */

#include <DiFinders.h>

HLK_LD2410_UART radar;

void setup() {
  Serial.begin(115200);

#if defined(HAVE_HWSERIAL1) || defined(SERIAL_PORT_HARDWARE1)
#if defined(ARDUINO_ARCH_ESP32)
  if (!beginSensorUart(256000)) {
    Serial.println(F("beginSensorUart failed"));
    while (1) {
      delay(1000);
    }
  }
  radar.begin(*sensorUartPort(), true);
#else
  Serial1.begin(256000);
  radar.begin(Serial1, true);
#endif
  Serial.print(F("uart_mode="));
  Serial.println(dmaModeName());
#else
  Serial.println(F("Need Serial1 (UNO R4, Mega, ESP32, RP2040)."));
#endif
}

void loop() {
#if !defined(HAVE_HWSERIAL1) && !defined(SERIAL_PORT_HARDWARE1)
  return;
#endif

  radar.read();
  Serial.print(F("motion="));
  if (radar.motion().detected()) {
    Serial.println(F("yes"));
  } else {
    Serial.println(F("no"));
  }
  delay(20);
}
