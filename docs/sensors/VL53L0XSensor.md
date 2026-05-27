# `VL53L0XSensor`

Header: [`src/families/tof/VL53L0XSensor.h`](src/families/tof/VL53L0XSensor.h)

## Overview

- **Bus:** I2C
- **Sketch aliases:** `TOF200C`, `TOF200F`, `VL53L0X`
- **Representative examples:** `VL53L0X_BasicRead`, `VL53L0X_ContinuousRead`


## Quick start

```cpp
#include <DiFinders.h>

VL53L0XSensor sensor;
// sensor.begin(...);  // see examples/
if (sensor.ready()) {
  RangeReading r = sensor.read();
  Serial.println(r.distanceMm);
}
```

## Methods

### `begin()`

```cpp
bool begin(TwoWire& wire = Wire, uint8_t address = 0x29, bool io2v8 = true);
```

Configure pins, bus, or serial stream.

| | |
| --- | --- |
| **Parameters** | Arguments depend on driver (GPIO pins, `Wire&`, `Stream&`, I2C address, SPI CS pin, …). |
| **Returns** | `void` or `bool` — `false` when the bus cannot be opened or the device does not respond. |


### `attachShutdownPin()`

```cpp
void attachShutdownPin(uint8_t shutdownPin, bool activeHigh = true);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint8_t shutdownPin, bool activeHigh = true |
| **Returns** | `void` |


### `powerOn()`

```cpp
void powerOn();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `void` |


### `powerOff()`

```cpp
void powerOff();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `void` |


### `powered()`

```cpp
bool powered() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


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


### `timeoutOccurred()`

```cpp
bool timeoutOccurred();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `setAddress()`

```cpp
bool setAddress(uint8_t newAddress);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint8_t newAddress |
| **Returns** | `bool` |


### `address()`

```cpp
uint8_t address() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint8_t` |


### `setSignalRateLimitMcps()`

```cpp
void setSignalRateLimitMcps(float limitMcps);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | float limitMcps |
| **Returns** | `void` |


### `startContinuous()`

```cpp
void startContinuous(uint32_t periodMs = 0);
```

Start free-running measurements (ToF / UART streaming).

| | |
| --- | --- |
| **Parameters** | Period in ms or µs depending on driver. |
| **Returns** | `void`. |


### `stopContinuous()`

```cpp
void stopContinuous();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
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


### `readSingle()`

```cpp
RangeReading readSingle();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `RangeReading` |


### `readContinuous()`

```cpp
RangeReading readContinuous();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `RangeReading` |


### `millimeters()`

```cpp
uint16_t millimeters();
```

Convenience read returning distance in millimetres.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t` distance (may trigger a new measurement). |


### `dataReady()`

```cpp
bool dataReady();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `readIfReady()`

```cpp
bool readIfReady(RangeReading& reading);
```

Non-blocking read when continuous mode has a fresh sample.

| | |
| --- | --- |
| **Parameters** | `RangeReading& out`. |
| **Returns** | `bool` — `true` when `out` was updated. |


### `attachDataReadyPin()`

```cpp
bool attachDataReadyPin(uint8_t pin, uint8_t mode = DF_IRQ_FALLING);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint8_t pin, uint8_t mode = DF_IRQ_FALLING |
| **Returns** | `bool` |


### `detachDataReadyPin()`

```cpp
void detachDataReadyPin();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `void` |


### `takeDataReadyEvent()`

```cpp
bool takeDataReadyEvent();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `setDistanceWindowMm()`

```cpp
bool setDistanceWindowMm(uint16_t innerMm, uint16_t outerMm, uint8_t windowMode = 2);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint16_t innerMm, uint16_t outerMm, uint8_t windowMode = 2 |
| **Returns** | `bool` |


### `isOutsideDistanceWindow()`

```cpp
bool isOutsideDistanceWindow(uint16_t distanceMm, uint16_t innerMm, uint16_t outerMm) const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint16_t distanceMm, uint16_t innerMm, uint16_t outerMm |
| **Returns** | `bool` |


### `deviceStatus()`

```cpp
uint16_t deviceStatus() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t` |


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


### `lastI2cStatus()`

```cpp
uint8_t lastI2cStatus() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint8_t` |


### `lastReading()`

```cpp
const RangeReading& lastReading() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `const RangeReading&` |


### `lastStatusText()`

```cpp
const char* lastStatusText() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `const char*` |


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
bool readRegister8(uint8_t reg, uint8_t& value);
```

Read one 8-bit register (I2C).

| | |
| --- | --- |
| **Parameters** | `reg` address; `uint8_t& value` output. |
| **Returns** | `bool` — `false` on I2C error. |


### `readRegister16()`

```cpp
bool readRegister16(uint8_t reg, uint16_t& value);
```

Read one 16-bit register (I2C).

| | |
| --- | --- |
| **Parameters** | `reg` address; `uint16_t& value` output. |
| **Returns** | `bool` — `false` on I2C error. |


### `writeRegister8()`

```cpp
bool writeRegister8(uint8_t reg, uint8_t value);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint8_t reg, uint8_t value |
| **Returns** | `bool` |


### `writeRegister16()`

```cpp
bool writeRegister16(uint8_t reg, uint16_t value);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint8_t reg, uint16_t value |
| **Returns** | `bool` |


### `dumpRegisters()`

```cpp
bool dumpRegisters(Print& out, uint8_t startReg, uint8_t count);
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

