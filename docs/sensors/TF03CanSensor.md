# `TF03CanSensor`

Header: [`src/families/ultrasonic/TF03CanSensor.h`](src/families/ultrasonic/TF03CanSensor.h)

## Overview

- **Bus:** CAN
- **Sketch aliases:** `Benewake_TF03_CAN`, `TF03_CAN`
- **Representative examples:** `Benewake_TF03_CAN_MCP2515_ESP32`, `Benewake_TF03_CAN_MCP2515_UnoR4`, `Benewake_TF03_CAN_SN65HVD230_ESP32`


## Quick start

```cpp
#include <DiFinders.h>

TF03CanSensor sensor;
// sensor.begin(...);  // see examples/
if (sensor.ready()) {
  RangeReading r = sensor.read();
  Serial.println(r.distanceMm);
}
```

## Methods

### `ready()`

```cpp
bool ready() const;
```

Whether the driver finished `begin()` and can accept reads.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` — `true` when configured (not always a live target). |


### `setTransmitId()`

```cpp
void setTransmitId(uint32_t id);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint32_t id |
| **Returns** | `void` |


### `transmitId()`

```cpp
uint32_t transmitId() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint32_t` |


### `setMinStrength()`

```cpp
void setMinStrength(uint16_t minStrength);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint16_t minStrength |
| **Returns** | `void` |


### `onCanFrame()`

```cpp
bool onCanFrame(const CanFrame& frame);
```

Feed one CAN 2.0 frame from MCP2515/TWAI into the parser.

| | |
| --- | --- |
| **Parameters** | `CanFrame` or `(id, data, dlc, extended)`. |
| **Returns** | `bool` — `true` if the frame was accepted and parsed. |


### `read()`

```cpp
RangeReading read() const;
```

Perform one measurement or refresh GPIO/UART/CAN state.

| | |
| --- | --- |
| **Parameters** | None for most drivers; some GPIO drivers accept `uint32_t nowMs`. |
| **Returns** | `RangeReading`, `ProximityReading`, `MotionReading`, or `bool` success — see class. |


### `lastStrength()`

```cpp
uint16_t lastStrength() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t` |


### `framesAccepted()`

```cpp
uint32_t framesAccepted() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint32_t` |


### `framesRejected()`

```cpp
uint32_t framesRejected() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint32_t` |


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


### `lastBusError()`

```cpp
uint8_t lastBusError() const;
```

Last bus error code (I2C) or `0` on GPIO/UART/CAN.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint8_t` — for I2C, internal `lastI2cStatus_` after last transaction. |


### `health()`

```cpp
SensorHealth health() const;
```

Unified runtime health for diagnostics.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `SensorHealth` (`ready`, `lastStatus`, `busErrorCode`, `faultCount`, …). |

