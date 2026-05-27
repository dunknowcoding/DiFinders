/*
 * Benewake_TF03_CAN_MCP2515_ESP32.ino
 *
 * Sensor : Benewake TF03 (CAN output mode, not UART)
 * Module : MCP2515 + TJA1050 (or MCP2551) SPI CAN shield / breakout
 * MCU    : ESP32 (3.3 V logic — use a 3.3 V MCP2515 module or level-shifted SPI)
 *
 * Third-party library (Arduino Library Manager):
 *   Name: "autowp-mcp2515"
 *   URL : https://github.com/autowp/arduino-mcp2515
 *
 * TF03 must be configured for CAN (BW_TFDS / factory). Default measurement ID = 0x03.
 * Bus: 500 kbit/s typical; match MY_CAN_BITRATE and crystal on the MCP2515 board.
 *
 * Wiring (example — adjust to your module silkscreen):
 *   MCP2515 CS  -> GPIO MY_MCP2515_CS
 *   MCP2515 INT -> GPIO MY_MCP2515_INT (optional)
 *   MCP2515 SCK/MOSI/MISO -> ESP32 VSPI (default SPI pins)
 *   CAN_H / CAN_L -> TF03 Green/White + 120 ohm termination at bus ends
 *   TF03 VCC 5 V, GND common with ESP32
 *
 * See docs/USER_GUIDE.md §7 for SN65HVD230 + native TWAI variant (no MCP2515).
 */

#include <SPI.h>
#include <mcp2515.h>

#include <DiFinders.h>

/* MCP2515 chip select and optional interrupt */
#define MY_MCP2515_CS 5
#define MY_MCP2515_INT 4

/*
 * MCP2515 oscillator on the PCB (read silkscreen / schematic):
 *   MCP_8MHZ  — most blue "CAN-BUS Shield" clones
 *   MCP_16MHZ — some Seed / DFRobot boards
 */
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
  Serial.print(F("can_controller="));
  Serial.println(canControllerLabel());
  Serial.println(F("Benewake TF03 over MCP2515 (ESP32)"));

  SPI.begin();
  canController.reset();
  if (canController.setBitrate(MY_CAN_BITRATE, MY_MCP2515_OSC) != MCP2515::ERROR_OK) {
    Serial.println(F("MCP2515 setBitrate failed — check crystal macro MY_MCP2515_OSC"));
    while (1) {
      delay(500);
    }
  }
  if (canController.setNormalMode() != MCP2515::ERROR_OK) {
    Serial.println(F("MCP2515 setNormalMode failed"));
    while (1) {
      delay(500);
    }
  }

#if defined(MY_MCP2515_INT)
  pinMode(MY_MCP2515_INT, INPUT_PULLUP);
#endif

  Serial.print(F("listening_can_id=0x"));
  Serial.println(tf03.transmitId(), HEX);
}

void loop() {
  if (canController.readMessage(&rawFrame) != MCP2515::ERROR_OK) {
    delay(2);
    return;
  }

  if (!tf03.onCanFrame(rawFrame.can_id, rawFrame.data, rawFrame.can_dlc, false)) {
    return;
  }

  RangeReading r = tf03.read();
  if (r.status == SensorStatus::Ok || r.status == SensorStatus::OutOfRange) {
    Serial.print(F("distance_mm="));
    Serial.print(r.distanceMm);
    Serial.print(F(" strength="));
    Serial.println(tf03.lastStrength());
  } else {
    Serial.print(F("status="));
    Serial.println(static_cast<uint8_t>(r.status));
  }
}
