# `ModbusDistanceSensor`

Header: [`src/families/tof/ModbusDistanceSensor.h`](src/families/tof/ModbusDistanceSensor.h)

## Overview

- **Bus:** RS485 Modbus
- **Sketch aliases:** `TOF200F_Modbus`, `WT53R485`, `WT53R_485`
- **Representative examples:** `WT53R485_BasicRead`


## Quick start

```cpp
#include <DiFinders.h>

ModbusDistanceSensor sensor;
// sensor.begin(...);  // see examples/
if (sensor.ready()) {
  RangeReading r = sensor.read();
  Serial.println(r.distanceMm);
}
```

## Methods

### `ModbusDistanceSensor()`

```cpp
explicit ModbusDistanceSensor(Stream& serial);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | Stream& serial |
| **Returns** | `explicit` |


### `begin()`

```cpp
void begin(Stream& serial);
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


### `setResponseDelay()`

```cpp
void setResponseDelay(uint16_t responseDelayMs);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint16_t responseDelayMs |
| **Returns** | `void` |


### `setDeviceAddress()`

```cpp
void setDeviceAddress(uint8_t deviceAddress);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint8_t deviceAddress |
| **Returns** | `void` |


### `deviceAddress()`

```cpp
uint8_t deviceAddress() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint8_t` |


### `setDistanceRegister()`

```cpp
void setDistanceRegister(uint16_t registerAddress, uint8_t registerCount = 1, uint8_t functionCode = 0x04);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint16_t registerAddress, uint8_t registerCount = 1, uint8_t functionCode = 0x04 |
| **Returns** | `void` |


### `setScaling()`

```cpp
void setScaling(uint16_t numerator, uint16_t denominator = 1);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint16_t numerator, uint16_t denominator = 1 |
| **Returns** | `void` |


### `setTransmitEnablePin()`

```cpp
void setTransmitEnablePin(uint8_t pin, bool activeHigh = true);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint8_t pin, bool activeHigh = true |
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


### `rawValue()`

```cpp
uint32_t rawValue() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint32_t` |


### `lastExceptionCode()`

```cpp
uint8_t lastExceptionCode() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint8_t` |


### `frameErrorCount()`

```cpp
uint16_t frameErrorCount() const;
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


### `flushRx()`

```cpp
void flushRx();
```

Discard stale bytes in the UART RX buffer.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `void`. |


### `health()`

```cpp
SensorHealth health() const;
```

Unified runtime health for diagnostics.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `SensorHealth` (`ready`, `lastStatus`, `busErrorCode`, `faultCount`, …). |

