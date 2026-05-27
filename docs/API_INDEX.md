# DiFinders API Index

Complete navigation for sketch authors. Per-driver method lists live under [`sensors/`](sensors/); marketplace names under [`ALIAS_INDEX.md`](ALIAS_INDEX.md).

```cpp
#include <DiFinders.h>   // DiFinders.h + DiFindersUserAPI.h — global names, no DiFinders:: prefix
```

---

## Recommended workflow

1. Pick the **sketch type** from your module silkscreen or listing (or find it in [ALIAS_INDEX.md](ALIAS_INDEX.md)).
2. Open the matching **`examples/*_BasicRead`** folder and copy wiring / baud / pins.
3. Call **`begin(...)`** with the correct bus (`Wire`, `Serial1`, GPIO pins, `Stream&`).
4. Check **`ready()`** before assuming live data.
5. In `loop()`, call **`read()`** (or mmWave **`read()`** then **`motion()`**).
6. Print with **`Serial`** using `distance_mm=` / `motion=` / `status=` keys.
7. Optional: **`health()`**, **`selfTest()`**, I2C **`dumpRegisters`**, UART **`flushRx()`**.

---

## Core value types

### `RangeReading`

| Field | Type | Meaning |
| --- | --- | --- |
| `distanceMm` | `uint16_t` | Millimetres (clamped 0–65535) |
| `status` | `SensorStatus` | Measurement outcome |
| `timestampMs` | `uint32_t` | `millis()` at capture |
| `rawValue` | `uint16_t` | Driver-specific raw (ADC, UART mm, …) |
| `valid()` | `bool` | `true` only when `status == Ok` |

### `MotionReading` / `ProximityReading`

| Field | Meaning |
| --- | --- |
| `detected()` | `true` when `state == Active` |
| `rose` / `fell` | Edge flags (PIR, digital inputs) |
| `strengthPermille` | Proximity strength where applicable |

### `SensorProbe` / `SelfTestReport` / `SensorHealth`

| Type | Use |
| --- | --- |
| `SensorProbe` | `probe()` — bus type, pins, feature flags (`supportsContinuousMode`, `supportsBeamRoi`, …) |
| `SelfTestReport` | `selfTest()` — quick pass/fail after wiring |
| `SensorHealth` | `health()` — `ready`, `lastStatus`, `busErrorCode`, `faultCount` |

### `SensorStatus` enum

| Value | Typical meaning |
| --- | --- |
| `Ok` | Valid measurement |
| `OutOfRange` | Beyond configured max range (may still have a numeric reading) |
| `Timeout` | No response in time (UART/I2C/pulse) |
| `NotReady` | `begin()` not called or bus not configured |
| `Error` | Checksum, Modbus exception, framing error |
| `Disabled` | Driver or feature disabled |

Print for debug: `Serial.println(static_cast<uint8_t>(r.status));`

### `SensorBusType`

`GpioPulse`, `GpioDigital`, `Analog`, `Uart`, `I2c`, `Rs485`, `Spi`, `Can` — returned in `probe().busType`; human name via `sensorBusTypeName()`.

### `CanFrame`

Used with **`TF03CanSensor::onCanFrame()`** after your MCP2515/TWAI stack receives a frame: `id`, `dlc`, `data[8]`, `extended`.

---

## Sketch helpers (`DiFindersHelpers.h`)

| Symbol | Returns | When to use |
| --- | --- | --- |
| `df_adc(DF_ADC_REF)` | mV reference | Sharp / MaxBotix analog constructors |
| `df_adc()` | ADC max counts | 1023 (AVR) or 4095 (12-bit) |
| `df_hit(motion/proximity)` | `bool` | Quick “active” check |

**Removed from sketches (do not use):** `DF_PROBE`, `DF_READ`, `df_code`, `difinders_*`.

---

## Global helpers

### Board (`DiFindersBoard.h`)

| Function | Description |
| --- | --- |
| `detectBoardFamily()` | `BoardFamily` enum |
| `boardFamilyName()` | `"esp32"`, `"avr"`, `"renesas_uno"`, … |
| `boardIs3v3Logic()` | `true` on UNO R4, ESP32, RP2040, … |
| `printBoardInfo(Print&)` | One-line MCU summary |
| `hasHardwareSerial1()` | Whether `Serial1` exists |
| `sensorUartPort()` | Default `HardwareSerial*` for sensor UART |
| `applyDefaultI2cClock()` | Sets `Wire` clock (100–400 kHz) |

### UART (`DiFindersUart.h`)

| Function | Description |
| --- | --- |
| `beginSensorUart(baud, outPort)` | `Serial1.begin` + optional ESP32 RX buffer resize |
| `configureSensorUartHighThroughput(uart, rxBytes)` | Enlarge RX ring for burst mmWave / ToF UART |

### Diagnostics (`DiFindersDiagnostics.h`)

| Function | Description |
| --- | --- |
| `makeHealth(...)` | Build `SensorHealth` |
| `printSensorHealth(Print&, health)` | Serial one-liner |

### CAN (`DiFindersCan.h`)

| Function | Description |
| --- | --- |
| `canFrameFromRaw(...)` | Fill `CanFrame` from controller API |
| `canControllerLabel()` | `"mcp2515-spi"`, `"esp32-twai"`, … |

### Throughput label (`DiFindersDma.h`)

| Function | Description |
| --- | --- |
| `dmaPlatformSupported()` | ESP32 / RP2040 / STM32 paths |
| `dmaModeName()` | Human label — **not** Wire I2C DMA |
| `busSupportsDma(bus)` | SPI/UART only |

---

## Common driver methods

Most classes in [`sensors/`](sensors/) implement a subset:

| Method | Returns | Notes |
| --- | --- | --- |
| `begin(...)` | `void` / `bool` | Pins, `Wire&`, `Stream&`, address, SPI CS |
| `ready()` | `bool` | Configured; not always “target in range” |
| `read()` | `RangeReading` / `bool` / motion types | Primary measurement |
| `millimeters()` | `uint16_t` | Convenience (may call `read()` internally) |
| `lastReading()` | const ref | Avoid double bus traffic |
| `probe()` | `SensorProbe` | Capabilities |
| `selfTest()` | `SelfTestReport` | Post-wiring check |
| `health()` | `SensorHealth` | Runtime diagnostics |
| `lastBusError()` | `uint8_t` | I2C status byte; `0` on GPIO/UART/CAN |
| `flushRx()` | `void` | UART discard stale bytes |
| `faultCount()` | counter | UART framing errors |
| `readRegister*` / `dumpRegisters` | I2C | VL53, GY-US42, … |

### mmWave UART extras (`LD2410RadarSensor`, `LD2461RadarSensor`, …)

| Method | Description |
| --- | --- |
| `motion()` | Latest `MotionReading` after `read()` |
| `presenceDetected()` | Any target |
| `movingTargetDistanceCm()` | Moving target range |
| `enableUnderlyingMessage(bool)` | Open-protocol modules (`OpenProtocolMmwaveSensor`) |

### Modbus (`ModbusDistanceSensor` / `WT53R485`)

| Method | Description |
| --- | --- |
| `setTransmitEnablePin(pin)` | RS485 DE/RE for MAX485 boards |
| `setDeviceAddress()` / `setDistanceRegister()` | Modbus map |
| `read()` | `RangeReading` after request/response |

### CAN (`TF03CanSensor`)

| Method | Description |
| --- | --- |
| `onCanFrame(id, data, dlc, ext)` | Feed frames from MCP2515/TWAI |
| `read()` | `RangeReading` after frame parsed |
| `setTransmitId()` | If module ID was reconfigured |

---

## Reading patterns (copy-paste)

### Distance (ultrasonic, ToF, UART lidar)

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

### mmWave UART

```cpp
radar.read();
MotionReading m = radar.motion();
Serial.print(F("motion="));
Serial.println(m.detected() ? 1 : 0);
```

### PIR / digital barrier

```cpp
MotionReading m = pir.read();
Serial.print(F("motion="));
Serial.println(m.detected() ? 1 : 0);
```

---

## Sensor drivers (by family)

Each link goes to method-level documentation. **Examples** lists primary sketches from the alias map.

### Ultrasonic (pulse & UART)

- [`PulseUltrasonicSensor`](sensors/PulseUltrasonicSensor.md) — Examples: `HC_SR04_BasicRead`, `JSN_SR04T_BasicRead`
- [`TFMiniSensor`](sensors/TFMiniSensor.md) — `TFLuna_BasicRead`
- [`A02YYUWSensor`](sensors/A02YYUWSensor.md) — `A02YYUW_BasicRead`, `DYP_A01_BasicRead`
- [`US100Sensor`](sensors/US100Sensor.md) — `US100_BasicRead`
- [`GyUs42I2cSensor`](sensors/GyUs42I2cSensor.md) — `GY_US42_I2C_BasicRead`
- [`GyUs42PulseSensor`](sensors/GyUs42PulseSensor.md) — `GY_US42_Pulse_BasicRead`
- [`MaxBotixAnalogSensor`](sensors/MaxBotixAnalogSensor.md) — `MB10XXAnalog_BasicRead`
- [`TF03CanSensor`](sensors/TF03CanSensor.md) — `Benewake_TF03_CAN_MCP2515_UnoR4`, …
- [`ModbusDistanceSensor`](sensors/ModbusDistanceSensor.md) — `WT53R485_BasicRead`

### Time-of-flight (ST & UART)

- [`VL53L0XSensor`](sensors/VL53L0XSensor.md) — `VL53L0X_BasicRead`
- [`VL53L1XSensor`](sensors/VL53L1XSensor.md) — `VL53L1X_BasicRead`, `VL53L1X_DualSensorRead`
- [`VL53L1XSpiSensor`](sensors/VL53L1XSpiSensor.md) — `VL53L1X_SPI`, `VL53L1X_SpiBurstRead`
- [`VL53L4CDSensor`](sensors/VL53L4CDSensor.md) — `VL53L4CD_BasicRead`
- [`VL53L4CXSensor`](sensors/VL53L4CXSensor.md) — `VL53L4CX_BasicRead`
- [`VL6180XSensor`](sensors/VL6180XSensor.md) — `VL6180X_BasicRead`
- [`ToFSenseFI2CSensor`](sensors/ToFSenseFI2CSensor.md) — `TOFSenseF2P_I2C_BasicRead`
- [`NoopLoopToFSensor`](sensors/NoopLoopToFSensor.md) — `TOFSenseF2P_UART_BasicRead`
- [`AsciiUartRangeSensor`](sensors/AsciiUartRangeSensor.md) — `TW10S_UART_BasicRead`

### Infrared & reflective

- [`DigitalIrBarrierSensor`](sensors/DigitalIrBarrierSensor.md) — `E18_D80NK_BasicRead`
- [`ReflectiveIrSensor`](sensors/ReflectiveIrSensor.md) — `TCRT5000_ReflectiveRead`
- [`SharpAnalogDistanceSensor`](sensors/SharpAnalogDistanceSensor.md) — `GP2Y0A21_BasicRead`
- [`SharpDigitalDistanceSensor`](sensors/SharpDigitalDistanceSensor.md) — `GP2Y0D810_DetectRead`
- [`Gp2Y0E03Sensor`](sensors/Gp2Y0E03Sensor.md) — `GP2Y0E03_BasicRead`

### mmWave & motion

- [`LD2410RadarSensor`](sensors/LD2410RadarSensor.md) — `HLK_LD2410_PresenceRead`, `HLK_LD2410_UartHighRateRead`
- [`LD2411RadarSensor`](sensors/LD2411RadarSensor.md) — `HLK_LD2411_UART_BasicRead`
- [`LD2412RadarSensor`](sensors/LD2412RadarSensor.md) — `HLK_LD2412_UART_BasicRead`, `HLK_LD2412_PresenceRead`
- [`LD2420RadarSensor`](sensors/LD2420RadarSensor.md) — `HLK_LD2420_UART_BasicRead`
- [`LD2450RadarSensor`](sensors/LD2450RadarSensor.md) — `HLK_LD2461_UART_BasicRead` (tracking family)
- [`LD2461RadarSensor`](sensors/LD2461RadarSensor.md) — `HLK_LD2461_UART_BasicRead`
- [`MmwavePresenceSensor`](sensors/MmwavePresenceSensor.md) — digital OUT presence sketches
- [`OpenProtocolMmwaveSensor`](sensors/OpenProtocolMmwaveSensor.md) — `OpenProtocol_Mmwave_BasicRead`
- [`PirMotionSensor`](sensors/PirMotionSensor.md) — `HC_SR501_MotionRead`

---

## Related documentation

| Link | Topic |
| --- | --- |
| [User Guide](USER_GUIDE.md) | Install, wiring, CAN, high-throughput, FAQ |
| [SKU alias index](ALIAS_INDEX.md) | Bus column, primary + also-see examples |
| [README](../README.md) | Overview and install |
| [Roadmap](ROADMAP.md) | Release phases |

Maintainer doc regeneration (not in IDE ZIP): `.github/scripts/generate_alias_index.py`, `alias_examples.yaml`.
