# DiFinders User Guide

DiFinders is an **Arduino IDE board library** for distance, proximity, and motion modules commonly sold on **AliExpress, Temu, Amazon, eBay**, and similar marketplaces — including modules that do **not** ship with an Adafruit, DFRobot, or SparkFun driver.

---

## 1. Install the library

1. Open **Arduino IDE 2.x** (or 1.8.x).
2. **Sketch → Include Library → Add .ZIP Library…** and select the `DiFinders` folder, **or** clone into your sketchbook:
   - Windows: `Documents/Arduino/libraries/DiFinders`
   - macOS: `~/Documents/Arduino/libraries/DiFinders`
3. Restart the IDE. Confirm **Sketch → Include Library** lists **DiFinders**.
4. Install board packages you need (AVR, **Arduino UNO R4**, **ESP32**, **SAMD**, **STM32**, **Raspberry Pi Pico / Pico 2**).

No separate “board support package” is required beyond your target MCU core.

---

## 2. How to design firmware with DiFinders

### 2.1 One include, typed families

```cpp
#include <DiFinders.h>

HC_SR04 sonar(12, 11);
VL53L1X tof;
```

- **Base drivers** live in `src/families/` (`PulseUltrasonicSensor`, `VL53L1XSensor`, `LD2410RadarSensor`, …).
- **Aliases** (`HC_SR04`, `TFMini`, `E18_D80NK`, …) are thin subclasses for reseller SKUs.
- **`#include <DiFinders.h>`** pulls in global alias names (`HC_SR04`, `VL53L1X`, …) via `DiFindersUserAPI.h` — no `DiFinders::` prefix in sketches.
- Sketches use **`sensor.read()`** and **`Serial.print`** — compare `reading.status` to `SensorStatus::Ok` / `OutOfRange` when needed.

### 2.2 Recommended bring-up flow

1. **Wire power and logic levels** (3.3 V vs 5 V — see §4).
2. **`begin(...)`** with the correct bus (`Wire`, `Serial1`, GPIO pins).
3. **`ready()`** — never assume `begin()` implies a live sensor.
4. **`selfTest()`** or **`health()`** before tuning filters in your app.
5. **`read()`** in `loop()` and print with **`Serial`**; use **`lastReading()`** to avoid duplicate I2C/UART traffic.
6. For bring-up/debug: **`readRegister*`** / **`dumpRegisters`** (I2C) or **`flushRx()`** (UART).

Standard distance print pattern (all `BasicRead` examples follow this):

```cpp
RangeReading r = sensor.read();
if (r.status == SensorStatus::Ok || r.status == SensorStatus::OutOfRange) {
  Serial.print(F("distance_mm="));
  Serial.println(r.distanceMm);
} else {
  Serial.print(F("status="));
  Serial.println(static_cast<uint8_t>(r.status));
}
```

### 2.3 Diagnostics API (by sensor kind)

| API | Range (I2C/UART pulse) | mmWave UART | CAN (`TF03CanSensor`) | GPIO proximity/PIR |
| --- | --- | --- | --- | --- |
| `ready()` | Yes | Yes (`serial_` set) | Always `true` | After `begin(pin)` |
| `read()` | `RangeReading` | `bool` (parse frame) | `RangeReading` (after `onCanFrame`) | `bool` / motion struct |
| `health()` | Most I2C/UART families | Yes | Yes | Yes (barrier, PIR, …) |
| `lastBusError()` | I2C families | `0` | `0` | `0` |
| Modbus RS485 | `ready()` when `serial_` set; `setTransmitEnablePin()` for DE/RE | — | — | — |
| `selfTest()` / `probe()` | Where implemented | Yes | Yes | Yes |

Optional debug: `printSensorHealth(Serial, sensor.health());` — not required in basic sketches.

Inherited drivers (`LD2412` → `LD2410`, `GyUs42Pulse` → `PulseUltrasonic`) use the **base class** diagnostics API.

mmWave: after `begin(Serial1)`, call `read()` in `loop()`, then `motion()` / `movingTargetDistanceCm()` / `presenceDetected()`.

---

## 3. Bus-specific wiring

### 3.1 I2C (VL53L0X/L1X/L4CD/L4CX, VL6180X, GY-US42 I2C, GP2Y0E03)

| Signal | UNO / Mega | ESP32 | Notes |
| --- | --- | --- | --- |
| SDA | SDA | GPIO 21 (typ.) | External **4.7 kΩ** pull-ups on long cables |
| SCL | SCL | GPIO 22 (typ.) | `Wire.setClock(400000)` after `Wire.begin()` if supported |
| XSHUT | Any GPIO | Any GPIO | Required for multi-sensor address assignment |
| GPIO1 | Optional INT | Optional INT | Use interrupt examples on AVR/ESP32 |

**VL53 multi-sensor:** hold each sensor in reset via XSHUT, assign unique 0x29… addresses, then release one at a time.

### 3.2 UART (US-100, TFMini, A02YYUW, HLK-LD24xx, Modbus)

- **Common GND** between MCU and module.
- **Level shifting** if the module is 3.3 V TTL and the MCU is 5 V (UNO) — resistive divider or dedicated translator.
- **Baud:** LD2410 often **256000**; LD2412/LD2420 often **115200** — match `Serial.begin()` to `defaultBaudRate()` / module label.
- After hot-plug or noise bursts, call **`flushRx()`** before the next command.

### 3.3 GPIO pulse (HC-SR04, JSN-SR04T, GY-US42 pulse)

| Role | Wiring |
| --- | --- |
| TRIG | MCU output |
| ECHO | MCU input (5 V tolerant pin on 3.3 V boards, or divider) |
| VCC | 5 V on most “HC-SR04” boards; 3.3 V on some JSN-SR04T |

Use **`configureTiming()`** for timeout and trigger pulse width on difficult mounts.

### 3.4 Analog (Sharp IR, MaxBotix, TCRT5000)

- **`analogRead`** path: stable **ADC reference**, average 3–5 samples in noisy environments.
- **`df_adc()`** helper scales raw counts to millivolts when you use library helpers.

---

## 4. MCU-specific notes

| Platform | Tips |
| --- | --- |
| **AVR UNO/Nano** | Prefer `SoftwareSerial` only when necessary; hardware `Serial1` on Mega. ISR-safe GPIO drivers for HC-SR04. |
| **UNO R4 Minima/WiFi** | 3.3 V logic — confirm sensor I/O tolerance. |
| **ESP32 / S3 / C3** | Map UART pins in `beginSensorUart()` for large RX buffers. See `HLK_LD2410_UartHighRateRead`, `VL53L1X_SpiBurstRead` (SPI burst, not Wire DMA). |
| **RP2040 / Pico 2** | `Serial1` pin selection via core defines; compile smoke tested. |
| **STM32 / SAMD** | `Wire`/`Serial` as per variant; check `printBoardInfo()` at boot. |

Call **`printBoardInfo(Serial)`** once in `setup()` to log detected family, logic level, and DMA support.

### 4.1 Logic level decision (3.3 V vs 5 V)

```text
Sensor module label says 3.3 V only?
  yes -> MCU I/O must be 3.3 V (UNO R4, ESP32, Pico, SAMD). No 5 V on signal pins.
  no  -> Often 5 V tolerant on VCC; check ECHO/TRIG/UART tolerance:
         UNO R4 / ESP32 / 3.3 V boards -> divider or 3.3 V module variant on ECHO & UART
         AVR UNO 5 V -> many pulse modules OK; confirm datasheet for UART RX pin
I2C (VL53, GY-US42)?
  -> 3.3 V modules on 5 V UNO: use level shifter or 3.3 V board
CAN / RS485 transceiver?
  -> Match transceiver IO to MCU; separate 5 V sensor supply is common
```

When unsure, measure idle voltage on **TX/RX/ECHO** with a multimeter before connecting to a 3.3 V pin.

---

## 5. Marketplace modules — practical notes

| Module class | Gotchas |
| --- | --- |
| **VL53L1X “mini” boards** | Verify chip (L0X vs L1X). Wrong driver = bogus IDs. |
| **US-100** | Allow **~80 ms** after trigger byte before read (tunable via `setMeasureDelay()`). |
| **TFMini / A02** | Binary frame sync — use `flushRx()` after baud changes. |
| **HLK-LD2410 vs LD2412** | **Different default baud**; wrong baud looks like “dead sensor”. |
| **WT53 / Modbus** | RS485 A/B polarity and **DE/RE** on MAX485-style boards; call `setTransmitEnablePin(dePin)` before `read()` (see `WT53R485_BasicRead`). |
| **GP2Y0E03** | I2C mode needs **shift register** read once in `begin()`. |

Third-party ST VL53L5/L7/L8 stacks are optional under `drivers/` — see examples `VL53L5CX_ThirdPartyRead` and compile script `extras/compile_third_party_examples.py`.

---

## 6. Examples map

| Folder prefix | Topic |
| --- | --- |
| `HC_SR04_*`, `JSN_*` | Pulse ultrasonic |
| `VL53L*` | ST ToF I2C/SPI, ROI, dual sensor |
| `US100_*`, `TFLuna_*`, `A02YYUW_*` | UART ranging |
| `HLK_LD24*` | mmWave UART |
| `OpenProtocol_Mmwave_BasicRead` | DFRobot C4001 / MR24HPC1 open UART protocol |
| `TW10S_UART_BasicRead` | ASCII UART laser (TW10SUART alias) |
| `Benewake_TF03_CAN_*` | CAN TF03 |
| `*_HighRateRead` / `*_SpiBurstRead` | Faster polling or ESP32 SPI/UART buffers (not I2C DMA) |
| `Library_CompileSmoke` | Includes one header per family — CI compile guard |
| `Platform_MultiRead` | Several sensors on one board — `read()` + Serial |

Each example header documents **board compatibility** and **wiring** when pins are required.

`MultiSensor_QuickRead` reads two sensors quickly on one board; `Platform_MultiRead` is a broader multi-family smoke demo (ultrasonic + IR + ToF + mmWave OUT).

---

## Appendix A — mmWave: UART vs GPIO presence

| Module / alias | Bus | Example sketch | What you get |
| --- | --- | --- | --- |
| `HLK_LD2410_UART`, `HLK_LD2410B` | UART 256000 | `HLK_LD2410_PresenceRead` | Distance + motion energy over serial |
| `HLK_LD2410` (GPIO OUT) | Digital OUT | `HLK_LD2410_Threshold` | Pin high/low only — not full UART telemetry |
| `HLK_LD2411_UART` | UART | `HLK_LD2411_UART_BasicRead` | LD2411 protocol |
| `HLK_LD2412_UART` | UART 115200 | `HLK_LD2412_UART_BasicRead` | UART reporting |
| `HLK_LD2412_OUT` | Digital OUT | `HLK_LD2412_PresenceRead` | OT pin only |
| `HLK_LD2420_UART` | UART | `HLK_LD2420_UART_BasicRead` | LD2420 frames |
| `HLK_LD2461_UART` | UART | `HLK_LD2461_UART_BasicRead` | Multi-target tracking |
| `HLK_LD2450`, `HLK_LD2451` | UART | `HLK_LD2461_UART_BasicRead` | Same family API (no dedicated BasicRead yet) |
| `DFRobot_C4001`, `MR24HPC1` | UART 115200 | `OpenProtocol_Mmwave_BasicRead` | Open protocol presence |
| `HC_SR501` | GPIO | `HC_SR501_MotionRead` | PIR motion (not mmWave) |

**Rule:** If the board has a **4-pin UART** header, use a `*_UART_*` or `*_PresenceRead` sketch on **`Serial1`**, not the OT pin example.

---

## Appendix B — FAQ

| Problem | Things to check |
| --- | --- |
| Serial Monitor garbage | USB `Serial` baud 115200; sensor on `Serial1` at **module** baud (`defaultBaudRate()`). |
| Always `status=3` (Timeout) | Wrong baud, swapped TX/RX, missing GND, or `flushRx()` after replug. |
| I2C init failed | Address 0x29 conflict, weak pull-ups, 5 V sensor on 3.3 V MCU without shifter. |
| HC-SR04 reads 0 | ECHO divider on 3.3 V board; `configureTiming()` too short. |
| Modbus always errors | RS485 A/B swap; **DE/RE** wired to `setTransmitEnablePin()`; 120 Ω termination. |
| CAN no frames | 120 Ω termination, 500 kbit/s, MCP2515 crystal macro, `onCanFrame()` before `read()`. |
| Two VL53 on one bus | Unique I2C addresses via **XSHUT** sequence (`VL53L1X_DualSensorRead`). |

---

## 7. CAN bus (Benewake TF03 and MCP2515 / TWAI)

DiFinders does **not** replace your CAN controller library. It parses **measurement frames** once your stack delivers a `CanFrame`.

### 7.1 When to use CAN

- TF03 (and similar) modules configured for **CAN output** (not UART) via Benewake tools or serial configuration.
- MCU connects through **MCP2515 + TJA1050** (SPI, common on shields) or **ESP32 TWAI** / **SN65HVD230** transceiver.

### 7.2 Wiring checklist

| Item | Detail |
| --- | --- |
| Termination | **120 Ω** at each end of the bus (often a jumper on shields) |
| GND | Common ground between MCU, transceiver, and sensor |
| Voltage | TF03 often **5 V**; ESP32 GPIO/SPI is **3.3 V** — use a 3.3 V MCP2515 module or level shifters |
| Bit rate | **500 kbit/s** typical; MCP2515 **8 MHz vs 16 MHz crystal** macro must match the PCB |
| IDs | Default measurement ID **0x03** (11-bit standard); set `tf03.setTransmitId()` if you changed it |

### 7.3 Sketch pattern

```cpp
#include <DiFinders.h>
// #include <mcp2515.h>  // or use ESP32 TWAI — see examples/

Benewake_TF03_CAN tf03;

void loop() {
  // After MCP2515/TWAI receive:
  CanFrame frame;
  // fill frame from controller …
  if (tf03.onCanFrame(frame)) {
    RangeReading r = tf03.read();
    Serial.println(r.distanceMm);
  }
}
```

### 7.4 Examples

| Sketch | Hardware |
| --- | --- |
| `Benewake_TF03_CAN_MCP2515_ESP32` | ESP32 + MCP2515 (`autowp-mcp2515` library) |
| `Benewake_TF03_CAN_MCP2515_UnoR4` | UNO R4 + MCP2515 |
| `Benewake_TF03_CAN_SN65HVD230_ESP32` | ESP32 native TWAI + SN65HVD230 |

Helpers: `canFrameFromRaw()`, `canControllerLabel()`, `TF03CanSensor::onCanFrame()`.

---

## 8. High throughput (SPI burst & UART buffers — not I2C DMA)

Arduino **`Wire` does not offer user-facing I2C DMA** on most cores. DiFinders uses the name **high-rate** for sketches that maximize throughput without claiming false DMA.

| Sketch folder | What actually happens |
| --- | --- |
| `VL53L0X_HighRateRead` / `VL53L4CD_HighRateRead` | `startContinuous()` + tight `readIfReady()` loop (I2C polling) |
| `VL53L1X_SpiBurstRead` | **SPI** VL53L1X; on ESP32, driver may use `SPI.transferBytes` burst (`dmaModeName()` reports `esp32-spi-burst`) |
| `HLK_LD2410_UartHighRateRead` | `beginSensorUart(256000)` enlarges **ESP32 UART RX buffer** to reduce dropped mmWave frames |

### 8.1 When to enable

- `DI_FINDERS_ENABLE_DMA` is **1** by default in `DiFindersConfig.h` on supported cores.
- Call `printBoardInfo(Serial)` once — `dma_mode=` shows `none`, `esp32-spi-burst/uart-rxbuf`, etc.

### 8.2 SPI high-rate (VL53L1X on SPI)

```cpp
#include <SPI.h>
VL53L1X_SPI tof;
SPI.begin();
tof.begin(CS_PIN, 8000000UL);
tof.startContinuous(20);
```

Use **`VL53L1X_SpiBurstRead`** on ESP32/STM32; use **`VL53L1X_SPI`** for standard interrupt-driven reads. CS is handled inside `begin(cs)` — do not duplicate `pinMode(CS)` in the sketch unless your board requires it.

### 8.3 UART high-rate (HLK-LD2410)

```cpp
#if defined(ARDUINO_ARCH_ESP32)
  beginSensorUart(256000);
  radar.begin(*sensorUartPort(), true);
#else
  Serial1.begin(256000);
  radar.begin(Serial1, true);
#endif
```

---

## 9. Further reading

- **[API Index](API_INDEX.md)** — global API + links to each sensor driver.
- **[sensors/](sensors/)** — detailed method parameters per class.

Maintainer: **dunknowcoding / NiusRobotLab** — issues welcome with module photos and exact SKU text.
