# `NoopLoopToFSensor`

Header: [`src/families/tof/NoopLoopToFSensor.h`](src/families/tof/NoopLoopToFSensor.h)

## Overview

- **Bus:** UART
- **Sketch aliases:** `CMOSLaserDistanceSensor`, `HighPrecisionLaserRangingModule`, `TOFSenseF2P_UART`, `WT53BRC_TTL`, `WT53R_TTL`
- **Representative examples:** `TOFSenseF2P_UART_BasicRead`


## Quick start

```cpp
#include <DiFinders.h>

NoopLoopToFSensor sensor;
// sensor.begin(...);  // see examples/
if (sensor.ready()) {
  RangeReading r = sensor.read();
  Serial.println(r.distanceMm);
}
```

## Methods

### `NoopLoopToFSensor()`

```cpp
explicit NoopLoopToFSensor(Stream& serial);
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


### `setFrameHeaderSecondByte()`

```cpp
void setFrameHeaderSecondByte(uint8_t headerByte);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint8_t headerByte |
| **Returns** | `void` |


### `frameHeaderSecondByte()`

```cpp
uint8_t frameHeaderSecondByte() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint8_t` |


### `setOkStatusByte()`

```cpp
void setOkStatusByte(uint8_t statusByte);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint8_t statusByte |
| **Returns** | `void` |


### `setOutOfRangeStatusByte()`

```cpp
void setOutOfRangeStatusByte(uint8_t statusByte);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint8_t statusByte |
| **Returns** | `void` |


### `setDistanceCalibration()`

```cpp
void setDistanceCalibration(int16_t offsetMm, uint16_t scalePermille = 1000);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | int16_t offsetMm, uint16_t scalePermille = 1000 |
| **Returns** | `void` |


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


### `timeout()`

```cpp
uint16_t timeout() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t` |


### `setMinSignalStrength()`

```cpp
void setMinSignalStrength(uint16_t minSignalStrength);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint16_t minSignalStrength |
| **Returns** | `void` |


### `minSignalStrength()`

```cpp
uint16_t minSignalStrength() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t` |


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


### `statusByte()`

```cpp
uint8_t statusByte() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint8_t` |


### `lastStatusText()`

```cpp
const char* lastStatusText() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `const char*` |


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

