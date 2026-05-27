/*
 * Benewake_TF03_CAN_MCP2515_UnoR4.ino
 *
 * Sensor : Benewake TF03 (CAN mode)
 * Module : MCP2515 + TJA1050 SPI CAN module (5 V tolerant SPI on UNO R4)
 * MCU    : Arduino UNO R4 Minima / WiFi (Renesas RA4M1, 3.3 V I/O)
 *
 * Third-party library:
 *   autowp-mcp2515 — https://github.com/autowp/arduino-mcp2515
 *
 * UNO R4 SPI (default): SCK=D13, MOSI=D11, MISO=D12. Route CS/INT as below.
 * Level-shift TF03 LVTTL (3.3 V) if required; CAN_H/CAN_L are differential only.
 *
 * See docs/USER_GUIDE.md §7 for module list and termination notes.
 */

#include <SPI.h>
#include <mcp2515.h>

#include <DiFinders.h>

#define MY_MCP2515_CS 10
#define MY_MCP2515_INT 2
#define MY_MCP2515_OSC MCP_8MHZ
#define MY_CAN_BITRATE CAN_500KBPS

MCP2515 canController(MY_MCP2515_CS);
struct can_frame rawFrame;

Benewake_TF03_CAN tf03;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }

  printBoardInfo();
  Serial.println(F("Benewake TF03 over MCP2515 (UNO R4)"));

  SPI.begin();
  canController.reset();
  if (canController.setBitrate(MY_CAN_BITRATE, MY_MCP2515_OSC) != MCP2515::ERROR_OK) {
    Serial.println(F("MCP2515 bitrate failed"));
    while (1) {
      delay(500);
    }
  }
  canController.setNormalMode();

  pinMode(MY_MCP2515_INT, INPUT_PULLUP);
}

void loop() {
  if (canController.readMessage(&rawFrame) != MCP2515::ERROR_OK) {
    delay(5);
    return;
  }

  if (!tf03.onCanFrame(rawFrame.can_id, rawFrame.data, rawFrame.can_dlc, false)) {
    return;
  }

  RangeReading r = tf03.read();
  Serial.print(F("distance_mm="));
  Serial.print(r.distanceMm);
  Serial.print(F(" status="));
  Serial.println(static_cast<uint8_t>(r.status));
}
