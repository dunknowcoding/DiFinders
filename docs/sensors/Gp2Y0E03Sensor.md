# `Gp2Y0E03Sensor`

Header: [`src/families/infrared/Gp2Y0E03Sensor.h`](src/families/infrared/Gp2Y0E03Sensor.h)

## Overview

- **Bus:** I2C
- **Sketch aliases:** `GP2Y0E03`
- **Representative examples:** `GP2Y0E03_AnalogRead`


## Quick start

```cpp
#include <DiFinders.h>

Gp2Y0E03Sensor sensor;
// sensor.begin(...);  // see examples/
if (sensor.ready()) {
  RangeReading r = sensor.read();
  Serial.println(r.distanceMm);
}
```

## Methods

### `begin()`

```cpp
void begin(uint8_t address = kDefaultAddress, TwoWire* wire = &Wire);
```

Configure pins, bus, or serial stream.

| | |
| --- | --- |
| **Parameters** | Arguments depend on driver (GPIO pins, `Wire&`, `Stream&`, I2C address, SPI CS pin, …). |
| **Returns** | `void` or `bool` — `false` when the bus cannot be opened or the device does not respond. |


### `beginAnalog()`

```cpp
void beginAnalog(uint8_t analogPin, uint16_t adcReferenceMv = 3300, uint16_t adcMax = 1023);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint8_t analogPin, uint16_t adcReferenceMv = 3300, uint16_t adcMax = 1023 |
| **Returns** | `void` |


### `ready()`

```cpp
bool ready() const;
```

Whether the driver finished `begin()` and can accept reads.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` — `true` when configured (not always a live target). |


### `i2cReady()`

```cpp
bool i2cReady() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `analogReady()`

```cpp
bool analogReady() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `calibrateAnalog()`

```cpp
void calibrateAnalog(uint16_t voutNearMv, uint16_t voutFarMv, uint16_t distNearMm, uint16_t distFarMm);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint16_t voutNearMv, uint16_t voutFarMv, uint16_t distNearMm, uint16_t distFarMm |
| **Returns** | `void` |


### `read()`

```cpp
RangeReading read();
```

Perform one measurement or refresh GPIO/UART/CAN state.

| | |
| --- | --- |
| **Parameters** | None for most drivers; some GPIO drivers accept `uint32_t nowMs`. |
| **Returns** | `RangeReading`, `ProximityReading`, `MotionReading`, or `bool` success — see class. |


### `millimeters()`

```cpp
uint16_t millimeters();
```

Convenience read returning distance in millimetres.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t` distance (may trigger a new measurement). |


### `readAnalog()`

```cpp
RangeReading readAnalog();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `RangeReading` |


### `shiftBit()`

```cpp
uint8_t shiftBit() const;
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


### `lastAnalogReading()`

```cpp
const RangeReading& lastAnalogReading() const;
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

