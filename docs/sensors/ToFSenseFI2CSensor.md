# `ToFSenseFI2CSensor`

Header: [`src/families/tof/ToFSenseFI2CSensor.h`](src/families/tof/ToFSenseFI2CSensor.h)

## Overview

- **Bus:** I2C
- **Sketch aliases:** `TOFSenseF2P`, `TOFSenseF2P_I2C`
- **Representative examples:** `TOFSenseF2P_I2C_BasicRead`


## Quick start

```cpp
#include <DiFinders.h>

ToFSenseFI2CSensor sensor;
// sensor.begin(...);  // see examples/
if (sensor.ready()) {
  RangeReading r = sensor.read();
  Serial.println(r.distanceMm);
}
```

## Methods

### `begin()`

```cpp
bool begin(TwoWire& wire = Wire, uint8_t address = 0x08);
```

Configure pins, bus, or serial stream.

| | |
| --- | --- |
| **Parameters** | Arguments depend on driver (GPIO pins, `Wire&`, `Stream&`, I2C address, SPI CS pin, …). |
| **Returns** | `void` or `bool` — `false` when the bus cannot be opened or the device does not respond. |


### `ready()`

```cpp
bool ready() const;
```

Whether the driver finished `begin()` and can accept reads.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` — `true` when configured (not always a live target). |


### `setTimeout()`

```cpp
void setTimeout(uint16_t timeoutMs);
```

Set I/O or command timeout for blocking reads.

| | |
| --- | --- |
| **Parameters** | Timeout in milliseconds. |
| **Returns** | `void`. |


### `timeout()`

```cpp
uint16_t timeout() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t` |


### `startMeasurement()`

```cpp
bool startMeasurement();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `read()`

```cpp
RangeReading read();
```

Perform one measurement or refresh GPIO/UART/CAN state.

| | |
| --- | --- |
| **Parameters** | None for most drivers; some GPIO drivers accept `uint32_t nowMs`. |
| **Returns** | `RangeReading`, `ProximityReading`, `MotionReading`, or `bool` success — see class. |


### `readIfReady()`

```cpp
RangeReading readIfReady();
```

Non-blocking read when continuous mode has a fresh sample.

| | |
| --- | --- |
| **Parameters** | `RangeReading& out`. |
| **Returns** | `bool` — `true` when `out` was updated. |


### `millimeters()`

```cpp
uint16_t millimeters();
```

Convenience read returning distance in millimetres.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t` distance (may trigger a new measurement). |


### `signalStrength()`

```cpp
uint16_t signalStrength();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t` |


### `lastSignalStrength()`

```cpp
uint16_t lastSignalStrength() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t` |


### `deviceStatus()`

```cpp
uint16_t deviceStatus() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t` |


### `lastStatusText()`

```cpp
const char* lastStatusText() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `const char*` |


### `address()`

```cpp
uint8_t address() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint8_t` |


### `lastI2cStatus()`

```cpp
uint8_t lastI2cStatus() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint8_t` |


### `probe()`

```cpp
SensorProbe probe() const;
```

Capability snapshot (bus type, pins, feature flags).

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `SensorProbe` (bus type, pins, feature flags). |


### `selfTest()`

```cpp
SelfTestReport selfTest();
```

Quick functional check after wiring.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `SelfTestReport` with `passed`, `status`, `value`, `detail`. |


### `lastReading()`

```cpp
const RangeReading& lastReading() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `const RangeReading&` |


### `lastBusError()`

```cpp
uint8_t lastBusError() const;
```

Last bus error code (I2C) or `0` on GPIO/UART/CAN.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint8_t` — for I2C, internal `lastI2cStatus_` after last transaction. |


### `readRegister8()`

```cpp
bool readRegister8(uint16_t reg, uint8_t& value);
```

Read one 8-bit register (I2C).

| | |
| --- | --- |
| **Parameters** | `reg` address; `uint8_t& value` output. |
| **Returns** | `bool` — `false` on I2C error. |


### `readRegister16()`

```cpp
bool readRegister16(uint16_t reg, uint16_t& value);
```

Read one 16-bit register (I2C).

| | |
| --- | --- |
| **Parameters** | `reg` address; `uint16_t& value` output. |
| **Returns** | `bool` — `false` on I2C error. |


### `writeRegister8()`

```cpp
bool writeRegister8(uint16_t reg, uint8_t value);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint16_t reg, uint8_t value |
| **Returns** | `bool` |


### `writeRegister16()`

```cpp
bool writeRegister16(uint16_t reg, uint16_t value);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint16_t reg, uint16_t value |
| **Returns** | `bool` |


### `dumpRegisters()`

```cpp
bool dumpRegisters(Print& out, uint16_t startReg, uint8_t count);
```

Print hex register dump to a `Print` stream.

| | |
| --- | --- |
| **Parameters** | `Print& out`, `startReg`, `count`. |
| **Returns** | `bool`. |


### `health()`

```cpp
SensorHealth health() const;
```

Unified runtime health for diagnostics.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `SensorHealth` (`ready`, `lastStatus`, `busErrorCode`, `faultCount`, …). |

