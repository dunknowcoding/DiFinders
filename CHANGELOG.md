# Changelog

> **Note:** This file is a local development log. The published Arduino Library Manager
> version stays at **0.1.0** until the first GitHub release; sections below (0.4.x) record
> internal milestones during bring-up.

## 0.1.1 — Nordic nRF52 / nRF53 support (2026-06-22)

### Added

- **ArduinoNRF board package** support (`ARDUINO_ARCH_NRF52`): ProMicro, SuperMini, nice!nano, XIAO, and other variants in the [ArduinoNRF](https://github.com/dunknowcoding/ArduinoNRF) core.
- **Official Nordic ecosystem** detection for Adafruit nRF52 BSP, Arduino Mbed Nano 33 BLE (`ARDUINO_NANO33BLE`), and nRF5340 application cores (`ARDUINO_ARCH_NRF53`).
- **`DiFindersNrfCompat.h`** — shared nRF52/nRF53 board-family macros used by board helpers.

### Fixed

- **`min`/`max` macro clash** with `<math.h>` on Nordic cores (LD2450/LD2461 radar headers).
- **Portable `String` parsing** in `AsciiUartRangeSensor` (`isdigit`, `c_str()` instead of Arduino-only helpers).

### Changed

- Default I2C clock remains **400 kHz** on all Nordic targets; 3.3 V logic assumed.
- Compile-smoke verified on **ArduinoNRF ProMicro nRF52840** (no hardware test in this release).

## 0.1.0 — first public release (2026-05-27)

- **Library version** `0.1.0` in `library.properties` for the initial GitHub / ZIP release.
- **Examples:** English-only sketches using `read()` + `Serial.print` and `SensorStatus` checks.
- **Docs:** [USER_GUIDE.md](docs/USER_GUIDE.md), [API_INDEX.md](docs/API_INDEX.md), [ALIAS_INDEX.md](docs/ALIAS_INDEX.md), per-sensor pages under [docs/sensors/](docs/sensors/).
- **Multi-sensor example:** `Platform_MultiRead` (renamed from `Platform_ProbeRead`).
- **Documentation:** expanded [README.md](README.md) and [API_INDEX.md](docs/API_INDEX.md); maintainer tooling under `.github/scripts/` (not in Arduino ZIP).

## 0.4.2 — 2026-05-27 (internal)

- **Examples (English only):** all sketches use `read()` + `Serial.print` with `distance_mm=` / `status=` keys; removed legacy `difinders_*`, `DF_PROBE`, `df_ok`, `df_code`, and placeholder `F("msg")`/`F("mm")` strings.
- **Rewritten:** `WT53R485_BasicRead`, `HLK_LD2420_UART_BasicRead`, `HLK_LD2461_UART_BasicRead`, `Platform_MultiRead`, `MultiSensor_QuickRead`, `VL6180X_BasicRead`, `HC_SR501_MotionRead`, `E18_D80NK_BasicRead`.
- **Diagnostics:** `LD2410RadarSensor::ready()`; `GyUs42I2cSensor` and `TF03CanSensor` gain `health()` / `lastBusError()`.
- **Docs:** [docs/ALIAS_INDEX.md](docs/ALIAS_INDEX.md); USER_GUIDE §2.3 capability matrix; README quick-start uses `SensorStatus`; doc generator Quick start branches by sensor kind.
- **Scripts:** `scripts/normalize_examples_en.py`, `scripts/fix_placeholders.py`, `scripts/generate_alias_index.py`.

## 0.4.1 — 2026-05-27

- **Documentation:** split API into [docs/API_INDEX.md](docs/API_INDEX.md) + [docs/sensors/](docs/sensors/) (per-method parameters/returns); [docs/API_REFERENCE.md](docs/API_REFERENCE.md) is a redirect. [USER_GUIDE.md](docs/USER_GUIDE.md) §7 CAN bus, §8 high-throughput (UART/SPI). Removed `docs/hardware/`.
- **`.gitignore`:** ignore entire `extras/` and `drivers/` trees for a clean library checkout.
- **`DigitalIrBarrierSensor`:** `ready()`, `health()`, `lastBusError()` aligned with diagnostics API.
- **Sketch API cleanup:** removed `DF_PROBE`, `DF_READ`, `df_ok`, `df_code`, `DF_SELFTEST`; all examples use `read()` + `Serial.print` and `SensorStatus` checks.
- **Examples:** standard Arduino APIs only (`Serial.begin`, `Serial1.begin`, `Wire.begin`, `SPI.begin`); removed `df_serial` / `df_uart` wrappers and redundant `pinMode` on SPI CS (handled in `sensor.begin(cs)`).
- Renamed misleading `*_DMA` sketches to `*_HighRateRead`, `VL53L1X_SpiBurstRead`, `HLK_LD2410_UartHighRateRead` with honest comments (no Wire I2C DMA).
- Fixed `HLK_LD2410_PresenceRead` to use UART (`HLK_LD2410_UART`); clarified digital-OUT vs UART examples.
- Cleaned duplicate/confusing sketch headers and inline tutorial comments.

## 0.4.0 — 2026-05-27

- **Unified diagnostics API** on all family drivers: `health()`, `lastBusError()`, I2C `readRegister*` / `dumpRegisters`, UART `flushRx()` / `faultCount()` where applicable (`DiFindersDiagnostics.h`).
- **Timing audit (10 rounds):** documented in `extras/timing_audit_rounds.md`; fixes for pulse µs→mm, ToF boot/poll backoff, US-100 measure delay, Modbus turnaround, GY-US42 settle, UART cooperative waits.
- **Documentation rebuilt (English):** [README.md](README.md), [docs/USER_GUIDE.md](docs/USER_GUIDE.md), [docs/API_REFERENCE.md](docs/API_REFERENCE.md); removed legacy `docs/*.md` split files.
- **Examples:** standardized board compatibility and wiring blocks in sketch headers.
- **Build:** restored compile after header repair scripts; `Library_CompileSmoke` verified on AVR UNO.

## 0.3.10 — 2026-05-27

- **CAN bus:** `DiFindersCan.h`, `Benewake_TF03_CAN` / `TF03CanSensor`, [docs/CAN_BUS.md](docs/CAN_BUS.md) (MCP2515, SN65HVD230, TJA1050, libraries), examples `Benewake_TF03_CAN_MCP2515_ESP32`, `Benewake_TF03_CAN_SN65HVD230_ESP32`, `Benewake_TF03_CAN_MCP2515_UnoR4`, `extras/compile_can_examples.py`.

## 0.3.9 — 2026-05-27

- **Board matrix:** `DiFindersBoard.h` (`printBoardInfo`, `sensorUartPort`, ESP32-H2/P4, nRF52840, Nano lines) and [docs/BOARD_MATRIX.md](docs/BOARD_MATRIX.md); expanded `compile-matrix.ps1` / CI.
- **DMA / high throughput:** `DI_FINDERS_ENABLE_DMA`, SPI burst on ESP32, UART RX buffer helper, `supportsDmaTransfer` in `probe()`, examples `VL53L1X_DMA`, `VL53L0X_DMA`, `VL53L4CD_DMA`, `HLK_LD2410_DMA`, [docs/DMA.md](docs/DMA.md).

- **Third-party multizone ToF:** `drivers/VL53L5CX`, `VL53L7CX`, `VL53L8CX` (STM32duino), `docs/THIRD_PARTY_DRIVERS.md`, `*_ThirdPartyRead` examples, `extras/fetch_third_party_drivers.py`, `extras/compile_third_party_examples.py`, CI job `third-party-compile`.
- Fix `attachGpioInterrupt` on UNO R4 (`PinStatus` cast for Renesas core).

- Examples and docs are **English only**; removed Chinese section titles and mojibake in headers.
- UART sketches explain **why `Serial1` is used instead of `Serial`**; `DF_PROBE` runs only after `begin(Serial1)` (no removed `df_uart`/`df_serial` wrappers).
- [docs/NAMING.md](docs/NAMING.md): expanded `Serial` vs `Serial1` table and guard pattern.

- All examples: `//` comments on `DF_PROBE`, `DF_READ`, `df_ok`/`df_code`, `HAVE_HWSERIAL1` guards, etc. (`extras/annotate_confusing_sketch_lines.py`).
- [docs/EXAMPLES.md](docs/EXAMPLES.md): sketch macro quick reference table.

- Documented beam vs multizone limits in [docs/BEAM_AND_MATRIX.md](docs/BEAM_AND_MATRIX.md).
- `VL53L1X` / `VL53L4CX`: `setBeamWindow()`, `useFullBeamWindow()`, `spadCenterFromGrid()`; `supportsBeamRoi` in `probe()`.
- `VL53L1X_BasicRead`, `VL53L4CX_BasicRead`, `VL53L0X_BasicRead`, `VL53L4CD_BasicRead` explain emission/reception control.

## 0.3.8 — 2026-05-27

- Distance **threshold/window** API: `VL53L0X`, `VL53L1X` (`setDistanceWindowMm`); VL53L4CD signal/sigma thresholds; LD2410 gate config.
- **Temperature** in `US100_BasicRead` and `TFLuna_BasicRead` (removed standalone `US100_TemperatureRead`).
- New examples: `VL53L0X_Threshold`, `VL53L1X_Threshold`, `VL53L4CD_Threshold`, `TCRT5000_Threshold`, `HLK_LD2410_Threshold`, `GP2Y0D810_Threshold`.
- All examples: UNO R4 Minima reference wiring, cross-board compatibility comments, inline `//` notes; UART `Serial1` fixes.
- `SensorProbe` flags: `supportsTemperatureReading`, `supportsThresholdConfig`.
- [docs/EXAMPLES.md](docs/EXAMPLES.md) feature index (DMA documented as N/A on Arduino path).

## 0.3.7 — 2026-05-27

- Documentation refresh for [github.com/dunknowcoding/DiFinders](https://github.com/dunknowcoding/DiFinders): professional README, docs index, English compatibility/catalog guides.
- Attribution: **dunknowcoding** · **NiusRobotLab** ([YouTube](https://www.youtube.com/@NiusRobotLab)).
- Removed “Pico first” / single-board priority language; all Arduino-compatible targets documented as peer platforms.

## 0.3.6 — 2026-05-27

- Removed sketch wrappers that duplicated Arduino APIs: `df_serial`, `df_uart`, `df_show`, `df_wire`, `df_spi`, and legacy `difinders_serial` / `difinders_sensor_uart` / `difinders_print`.
- Removed baud, loop-delay, and I2C-clock macros (`DF_BAUD_*`, `DF_LOOP_MS`, `DF_SERIAL_*`, `DF_I2C_HZ`, …); examples use numeric literals.
- Kept library-specific helpers: `df_ok`, `df_code`, `df_hit`, `df_adc`, `DF_READ`, `DF_PROBE`, pin/address/status macros.

## 0.3.5 — 2026-05-27

- GPIO interrupt helpers (`DiFindersGpioInterrupt.h`): AVR-safe ISRs, `attachInterruptPin` / `attachDataReadyPin` / `attachEchoInterrupt`.
- SPI transport (`DiFindersSpi.h`) and `VL53L1XSpiSensor` (`VL53L1X_SPI`, `TOF400C_SPI` aliases).
- New examples: `HC_SR501_Interrupt`, `HC_SR04_Interrupt`, `VL53L0X_Interrupt`, `VL53L1X_Interrupt`, `VL6180X_Interrupt`, `VL53L4CD_Interrupt`, `VL53L1X_SPI`.
- Constants: `DF_PIN_TOF_INT`, `DF_PIN_SPI_CS`, `DF_SPI_HZ`; helper `df_spi()`.

## 0.3.4 — 2026-05-27

- Sketch naming: short `DF_*` macros and `df_*` overloaded helpers (`docs/NAMING.md`, `docs/API.md`).
- English-only examples with block comments (wiring + API); `extras/englishify_examples.py`.
- Legacy `DIFINDERS_*` / `difinders_*` aliases retained.

## 0.3.3 — 2026-05-27

- Sketch helpers consolidated into overloaded `difinders_*` names (later shortened to `df_*` in 0.3.4).
- Full macro comments in `DiFindersConstants.h`; UART examples use `difinders_sensor_uart()` overloads.

## 0.3.2 — 2026-05-27

- UART examples: removed `#if HAVE_HWSERIAL1` / `sensorSerial`; use library UART helpers.
- User API layer: constants + helpers + global export; English-oriented examples.

## 0.3.1 — 2026-05-27

- `LD2410RadarSensor` now parses HLK-LD2410S compact F4 frames in addition to LD2410 `0xAA` reports.
- Added `HLK_LD2410S_OUT`, `HLK_LD2412_OUT`, `GY_US42_UART`, `GY_US42_Pulse`, `TOF050F`, `TOF200F`, `TOF200F_Modbus`.
- `GP2Y0E03` gains optional analog `VOUT` read path with two-point calibration.
- New examples: `TOFSenseF2P_UART_BasicRead`, `GY_US42_UART/Pulse`, `GP2Y0E03_AnalogRead`, presence reads for LD2410S/LD2412.
- `compile-matrix.ps1` adds `arduino:mbed_nano:nano33ble`; fixed `TW10S` Uno `Serial1` guard (0.3.0).

## 0.3.0 — 2026-05-27

- GitHub-ready layout: LICENSE, CI workflow, `.gitignore`, hardware docs.
- New drivers: `LD2412RadarSensor`, `GyUs42I2cSensor`, `Gp2Y0E03Sensor`, `DigitalIrBarrierSensor`.
- New aliases: `HLK_LD2412`, `HLK_LD2410S`, `GY_US42`, `GP2Y0E03`, `E18_D80NK`, `DYP_A01`, `HY_SRF05`, `Benewake_TF03`, `US015`.
- New examples and `extras/verify-rounds.ps1` (20-round `arduino-cli` smoke).
- `docs/hardware/` with silkscreen notes and asset fetch script.

## 0.2.0

- Initial public sensor families (ultrasonic, UART laser, ST ToF, HLK mmWave, Modbus/NoopLoop).
