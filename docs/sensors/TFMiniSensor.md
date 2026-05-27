# `TFMiniSensor`

Header: [`src/families/ultrasonic/TFMiniSensor.h`](src/families/ultrasonic/TFMiniSensor.h)

## Overview

- **Bus:** UART
- **Sketch aliases:** `Benewake_TF03`, `TFLuna`, `TFMini`
- **Representative examples:** `TFLuna_BasicRead`


## Quick start

```cpp
#include <DiFinders.h>

TFMiniSensor sensor;
// sensor.begin(...);  // see examples/
if (sensor.ready()) {
  RangeReading r = sensor.read();
  Serial.println(r.distanceMm);
}
```

## Methods

### `TFMiniSensor()`

```cpp
explicit TFMiniSensor(Stream& serial);
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


### `setMinStrength()`

```cpp
void setMinStrength(uint16_t minStrength);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint16_t minStrength |
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


### `strength()`

```cpp
uint16_t strength();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t` |


### `temperatureC()`

```cpp
int16_t temperatureC();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `int16_t` |


### `lastStrength()`

```cpp
uint16_t lastStrength() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t` |


### `lastTemperatureC()`

```cpp
int16_t lastTemperatureC() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `int16_t` |


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


### `lastFrameTimestampMs()`

```cpp
uint32_t lastFrameTimestampMs() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint32_t` |


### `frameErrorCount()`

```cpp
uint16_t frameErrorCount() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t` |


### `droppedFrameCount()`

```cpp
uint16_t droppedFrameCount() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t` |


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


### `faultCount()`

```cpp
uint32_t faultCount() const;
```

Accumulated framing or parse errors (UART drivers).

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint32_t`. |


### `health()`

```cpp
SensorHealth health() const;
```

Unified runtime health for diagnostics.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `SensorHealth` (`ready`, `lastStatus`, `busErrorCode`, `faultCount`, …). |

