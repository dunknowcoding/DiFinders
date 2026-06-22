<p align="center">
  <img src="docs/assets/difinders-banner.svg" alt="DiFinders — Arduino sensor library" width="720"/>
</p>

<h1 align="center">DiFinders</h1>

<p align="center">
  <strong>Arduino library for marketplace distance, ToF, IR, and mmWave modules.</strong><br/>
  One install · many MCUs · reseller SKUs (AliExpress, Temu, Amazon, eBay) — not only branded breakouts.
</p>

<p align="center">
  <a href="https://github.com/dunknowcoding/DiFinders/releases/tag/v0.1.1"><code>v0.1.1</code></a> ·
  <code>AVR</code> · <code>UNO R4</code> · <code>ESP32</code> · <code>SAMD</code> · <code>STM32</code> · <code>RP2040 / Pico 2</code> · <code>nRF52 / nRF53</code>
</p>

---

## Table of contents

- [Why DiFinders](#why-difinders)
- [Install](#install)
- [Quick start by bus](#quick-start-by-bus)
- [Documentation map](#documentation-map)
- [Supported hardware](#supported-hardware)
- [API at a glance](#api-at-a-glance)
- [Examples](#examples)
- [Board compatibility](#board-compatibility)
- [What's in the ZIP](#whats-in-the-zip)
- [Contributing & license](#contributing--license)

---

## Why DiFinders?

| Benefit | What you get |
| --- | --- |
| **Arduino-native** | Sketch → Include Library → Examples → Upload. No PlatformIO required. |
| **Reseller-first aliases** | `HC_SR04`, `TFMini`, `VL53L1X`, `HLK_LD2410_UART`, `WT53R485`, `Benewake_TF03_CAN`, … each map to one tested driver. |
| **Multi-MCU** | Same sketch patterns on AVR, UNO R4, ESP32, SAMD, STM32, RP2040, ArduinoNRF / Adafruit / Mbed Nordic — CI compiles `Library_CompileSmoke` on four cores. |
| **Unified diagnostics** | `ready()`, `selfTest()`, `health()`, `lastBusError()`; I2C families add `readRegister*` / `dumpRegisters`. |
| **Honest timing** | UART settle, ToF boot waits, ultrasonic µs→mm, Modbus turnaround — tuned for real modules. |
| **English examples** | Wiring blocks, board notes, canonical Serial keys (`distance_mm=`, `motion=`, `status=`). |

Complements Adafruit / DFRobot / SparkFun libraries for the **long tail** of clone and no-name boards.

---

## Install

### Arduino IDE 2.x (recommended)

1. Download [**v0.1.1 source ZIP**](https://github.com/dunknowcoding/DiFinders/archive/refs/tags/v0.1.1.zip) or clone this repo.
2. **Sketch → Include Library → Add .ZIP Library…** → select the `DiFinders` folder (contains `library.properties`).
3. Restart the IDE. Confirm **Sketch → Include Library → DiFinders** appears.
4. Install your board package (e.g. **Arduino UNO R4 Boards**, **esp32** by Espressif).

### Sketchbook folder (git clone)

| OS | Path |
| --- | --- |
| Windows | `Documents\Arduino\libraries\DiFinders` |
| macOS | `~/Documents/Arduino/libraries/DiFinders` |
| Linux | `~/Arduino/libraries/DiFinders` |

Open any example under **File → Examples → DiFinders**.

---

## Quick start by bus

### GPIO pulse ultrasonic (HC-SR04 class)

```cpp
#include <DiFinders.h>

HC_SR04 sonar(12, 11);  // TRIG, ECHO

void setup() {
  Serial.begin(115200);
  sonar.configureTiming(30000);
}

void loop() {
  RangeReading r = sonar.read();
  if (r.status == SensorStatus::Ok || r.status == SensorStatus::OutOfRange) {
    Serial.print(F("distance_mm="));
    Serial.println(r.distanceMm);
  } else {
    Serial.print(F("status="));
    Serial.println(static_cast<uint8_t>(r.status));
  }
  delay(120);
}
```

### I2C ToF (VL53L1X)

```cpp
#include <DiFinders.h>
#include <Wire.h>

VL53L1X tof;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(100000);
  if (!tof.begin(Wire, 0x29)) {
    Serial.println(F("VL53L1X init failed"));
    while (1) delay(500);
  }
}

void loop() {
  RangeReading r = tof.read();
  Serial.print(F("distance_mm="));
  Serial.println(r.distanceMm);
  delay(50);
}
```

### UART (TFMini / A02 / US-100 on Serial1)

```cpp
#include <DiFinders.h>

TFMini lidar;

void setup() {
  Serial.begin(115200);
#if defined(HAVE_HWSERIAL1) || defined(SERIAL_PORT_HARDWARE1)
  Serial1.begin(115200);
  lidar.begin(Serial1);
#endif
}

void loop() {
  RangeReading r = lidar.read();
  Serial.print(F("distance_mm="));
  Serial.println(r.distanceMm);
  delay(100);
}
```

**Rule:** USB debug on `Serial` @ 115200; sensor on `Serial1` at the module baud (see [`USER_GUIDE.md`](docs/USER_GUIDE.md)).

---

## Documentation map

| Document | Contents |
| --- | --- |
| [**User Guide**](docs/USER_GUIDE.md) | Install, 3.3 V vs 5 V, wiring tables, CAN §7, high-rate §8, mmWave appendix, FAQ |
| [**API Index**](docs/API_INDEX.md) | Types, enums, global helpers, driver catalog, reading patterns |
| [**SKU alias index**](docs/ALIAS_INDEX.md) | 100+ marketplace names → bus → family → primary example |
| [**Sensor API**](docs/sensors/) | Per-class methods (`begin`, `read`, `health`, mmWave extras) |
| [**Roadmap**](docs/ROADMAP.md) | Maintainer layer checklist L59–L158 |
| [**Examples**](examples/) | 74 sketches: BasicRead, Interrupt, Threshold, CAN, dual ToF, multi-sensor |

Legacy monolithic API page: [`API_REFERENCE.md`](docs/API_REFERENCE.md) redirects to the split docs above.

---

## Supported hardware

### By bus

| Bus | Examples in library | Typical modules |
| --- | --- | --- |
| GPIO pulse | `HC_SR04_BasicRead`, `JSN_SR04T_BasicRead` | HC-SR04, waterproof JSN, RCWL |
| UART | `TFLuna_BasicRead`, `A02YYUW_BasicRead`, `US100_BasicRead` | TFMini, DYP-A02, US-100, TW10S |
| I2C | `VL53L1X_BasicRead`, `VL6180X_BasicRead`, `GY_US42_I2C_BasicRead` | ST VL53/VL6180, GY-US42 |
| SPI | `VL53L1X_SPI` | VL53L1X breakout with CS |
| RS485 Modbus | `WT53R485_BasicRead` | WT53 / TOF200F Modbus |
| CAN | `Benewake_TF03_CAN_*` | TF03 + MCP2515 or ESP32 TWAI |
| Analog | `GP2Y0A21_BasicRead`, `MB10XXAnalog_BasicRead` | Sharp GP2Y, MaxBotix |
| GPIO digital | `HC_SR501_MotionRead`, `E18_D80NK_BasicRead` | PIR, E18 barrier |
| mmWave UART | `HLK_LD2410_PresenceRead`, `HLK_LD2461_UART_BasicRead` | HLK LD24xx, C4001 open protocol |

Full alias list: [**ALIAS_INDEX.md**](docs/ALIAS_INDEX.md).

---

## API at a glance

```cpp
#include <DiFinders.h>   // pulls DiFindersUserAPI.h — global type names, no namespace
```

| Concept | API |
| --- | --- |
| Distance | `RangeReading r = sensor.read();` → `r.distanceMm`, `r.status` |
| Motion / PIR | `MotionReading m = sensor.read();` or `sensor.motion()` (mmWave UART) |
| Proximity | `ProximityReading p = sensor.read();` → `p.detected()` |
| Bring-up | `sensor.begin(...);` then `if (!sensor.ready())` |
| Debug | `sensor.health()`, `sensor.selfTest()`, `printSensorHealth(Serial, h)` |
| I2C debug | `readRegister8`, `dumpRegisters` (VL53, GY-US42, …) |
| UART hygiene | `flushRx()` after baud change or hot-plug |

`SensorStatus`: `Ok`, `OutOfRange`, `Timeout`, `NotReady`, `Error`, `Disabled` — compare with `static_cast<uint8_t>(r.status)` when printing.

Details: [**API_INDEX.md**](docs/API_INDEX.md).

---

## Examples

| Pattern | Folders | Purpose |
| --- | --- | --- |
| BasicRead | `*_BasicRead` | Minimal `read()` + Serial |
| Motion / presence | `*_MotionRead`, `*_PresenceRead` | PIR or mmWave |
| Interrupt | `*_Interrupt` | GPIO1 / INT pin |
| Threshold / ROI | `*_Threshold`, `*_ROIRead` | Zones and detection gates |
| High rate | `*_HighRateRead`, `*_SpiBurstRead` | Faster polling (not I2C DMA) |
| CAN | `Benewake_TF03_CAN_*` | MCP2515 UNO R4 / ESP32 |
| Multi-sensor | `Platform_MultiRead`, `MultiSensor_QuickRead` | Several drivers on one board |
| CI smoke | `Library_CompileSmoke` | One `#include` per family |

Copy an example folder into your sketchbook and edit `MY_*` pin defines.

---

## Board compatibility

| Platform | Notes |
| --- | --- |
| AVR UNO / Nano | Pulse, analog OK; often **no** `Serial1` — see UART examples' `#if HAVE_HWSERIAL1` |
| UNO R4 Minima / WiFi | **Recommended** reference — 3.3 V, hardware `Serial1` |
| ESP32 | `beginSensorUart()` for large RX buffer; TWAI CAN, SPI burst |
| RP2040 / Pico 2 | Flexible UART pins; 3.3 V |
| Mega 2560 | Multiple hardware UARTs |
| SAMD / STM32 | 3.3 V logic; per-core pin maps |
| **ArduinoNRF (nRF52)** | ProMicro / SuperMini / nice!nano / XIAO clones — 3.3 V, `Serial1`, `Wire.begin(sda,scl)`; compile-smoke verified on ProMicro nRF52840 |
| **Adafruit nRF52 / Mbed Nano 33 BLE** | Feather nRF52840, Nano 33 BLE — detected via `ARDUINO_NRF52_*` / `ARDUINO_NANO33BLE`; 3.3 V, 400 kHz I2C default |
| **nRF5340 (application core)** | Adafruit / Nordic nRF5340 targets — detected via `ARDUINO_ARCH_NRF53`; same 3.3 V sensor wiring assumptions |

Call `printBoardInfo(Serial);` once in `setup()` to log detected family and logic level.

---

## What's in the ZIP

| Path | Shipped to end users |
| --- | --- |
| `src/`, `examples/`, `docs/`, `keywords.txt`, `library.properties` | Yes |
| `.github/` (CI + maintainer scripts) | In git only; not required in IDE |
| `extras/`, `drivers/`, `scripts/` | **No** — local/gitignored maintainer trees |

---

## Contributing & license

- [**CONTRIBUTING.md**](CONTRIBUTING.md) — PR flow, new SKU = `DiFinders.h` + `.github/scripts/alias_examples.yaml`
- [**Issues**](https://github.com/dunknowcoding/DiFinders/issues) — include FQBN, photo, and listing title
- **MIT** — [LICENSE](LICENSE)

Maintained by [**dunknowcoding**](https://github.com/dunknowcoding) for [**NiusRobotLab**](https://www.youtube.com/@NiusRobotLab).

If this library saved you from a blurry datasheet at 2 a.m., star the repo and open an issue with your exact SKU text.
