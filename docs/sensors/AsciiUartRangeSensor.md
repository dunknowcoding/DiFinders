# `AsciiUartRangeSensor`

Header: [`src/families/tof/AsciiUartRangeSensor.h`](src/families/tof/AsciiUartRangeSensor.h)

## Overview

- **Bus:** UART
- **Sketch aliases:** `TW10SUART`, `TW10S_UART`
- **Representative examples:** `TW10S_UART_BasicRead`


## Quick start

```cpp
#include <DiFinders.h>

AsciiUartRangeSensor sensor;
// sensor.begin(...);  // see examples/
if (sensor.ready()) {
  RangeReading r = sensor.read();
  Serial.println(r.distanceMm);
}
```

## Methods

### `AsciiUartRangeSensor()`

```cpp
explicit AsciiUartRangeSensor(Stream& stream);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | Stream& stream |
| **Returns** | `explicit` |


### `begin()`

```cpp
void begin(Stream& stream);
```

Configure pins, bus, or serial stream.

| | |
| --- | --- |
| **Parameters** | Arguments depend on driver (GPIO pins, `Wire&`, `Stream&`, I2C address, SPI CS pin, …). |
| **Returns** | `void` or `bool` — `false` when the bus cannot be opened or the device does not respond. |


### `setTimeout()`

```cpp
void setTimeout(uint16_t timeoutMs);
```

Set I/O or command timeout for blocking reads.

| | |
| --- | --- |
| **Parameters** | Timeout in milliseconds. |
| **Returns** | `void`. |


### `setDefaultBaudRate()`

```cpp
void setDefaultBaudRate(uint32_t baudRate);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint32_t baudRate |
| **Returns** | `void` |


### `defaultBaudRate()`

```cpp
uint32_t defaultBaudRate() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint32_t` |


### `setUnitMillimeters()`

```cpp
void setUnitMillimeters(float scale);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | float scale |
| **Returns** | `void` |


### `setFrameDelimiters()`

```cpp
void setFrameDelimiters(char startChar, char endChar);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | char startChar, char endChar |
| **Returns** | `void` |


### `setPrefix()`

```cpp
void setPrefix(const String& prefix);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | const String& prefix |
| **Returns** | `void` |


### `allowSignedValues()`

```cpp
void allowSignedValues(bool enabled);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | bool enabled |
| **Returns** | `void` |


### `read()`

```cpp
bool read();
```

Perform one measurement or refresh GPIO/UART/CAN state.

| | |
| --- | --- |
| **Parameters** | None for most drivers; some GPIO drivers accept `uint32_t nowMs`. |
| **Returns** | `RangeReading`, `ProximityReading`, `MotionReading`, or `bool` success — see class. |


### `lastReading()`

```cpp
const RangeReading& lastReading() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `const RangeReading&` |


### `millimeters()`

```cpp
uint16_t millimeters() const;
```

Convenience read returning distance in millimetres.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t` distance (may trigger a new measurement). |


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
SelfTestReport selfTest() const;
```

Quick functional check after wiring.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `SelfTestReport` with `passed`, `status`, `value`, `detail`. |


### `ready()`

```cpp
bool ready() const;
```

Whether the driver finished `begin()` and can accept reads.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` — `true` when configured (not always a live target). |


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

