# `OpenProtocolMmwaveSensor`

Header: [`src/families/motion/OpenProtocolMmwaveSensor.h`](src/families/motion/OpenProtocolMmwaveSensor.h)

## Overview

- **Bus:** UART
- **Sketch aliases:** `DFRobotMmWaveC4001`, `DFRobot_C4001`, `MR24HPC1`
- **Representative examples:** `OpenProtocol_Mmwave_BasicRead`


## Quick start

```cpp
#include <DiFinders.h>

OpenProtocolMmwaveSensor sensor;
// Serial1.begin(baud); sensor.begin(Serial1);
sensor.read();
Serial.println(sensor.presenceDetected());
```

## Methods

### `OpenProtocolMmwaveSensor()`

```cpp
explicit OpenProtocolMmwaveSensor(Stream& serial);
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


### `setTimeout()`

```cpp
void setTimeout(uint16_t timeoutMs);
```

Set I/O or command timeout for blocking reads.

| | |
| --- | --- |
| **Parameters** | Timeout in milliseconds. |
| **Returns** | `void`. |


### `defaultBaudRate()`

```cpp
uint32_t defaultBaudRate() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint32_t` |


### `read()`

```cpp
bool read();
```

Perform one measurement or refresh GPIO/UART/CAN state.

| | |
| --- | --- |
| **Parameters** | None for most drivers; some GPIO drivers accept `uint32_t nowMs`. |
| **Returns** | `RangeReading`, `ProximityReading`, `MotionReading`, or `bool` success — see class. |


### `sendRawCommand()`

```cpp
bool sendRawCommand(const uint8_t* bytes, size_t length);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | const uint8_t* bytes, size_t length |
| **Returns** | `bool` |


### `enableUnderlyingMessage()`

```cpp
bool enableUnderlyingMessage(bool enabled);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | bool enabled |
| **Returns** | `bool` |


### `reset()`

```cpp
bool reset();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `queryDeviceId()`

```cpp
bool queryDeviceId();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `queryFirmwareVersion()`

```cpp
bool queryFirmwareVersion();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `presenceDetected()`

```cpp
bool presenceDetected() const;
```

True when the radar reports any target (moving or stationary).

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool`. |


### `movingDetected()`

```cpp
bool movingDetected() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `stationaryDetected()`

```cpp
bool stationaryDetected() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `bodySign()`

```cpp
uint8_t bodySign() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint8_t` |


### `directionState()`

```cpp
uint8_t directionState() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint8_t` |


### `lastFrameType()`

```cpp
uint8_t lastFrameType() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint8_t` |


### `lastFrameSubtype()`

```cpp
uint8_t lastFrameSubtype() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint8_t` |


### `lastFrameLength()`

```cpp
uint8_t lastFrameLength() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint8_t` |


### `lastFrame()`

```cpp
const uint8_t* lastFrame() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `const uint8_t*` |


### `lastReading()`

```cpp
const MotionReading& lastReading() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `const MotionReading&` |


### `motion()`

```cpp
MotionReading motion() const;
```

Last motion/presence reading structure.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `MotionReading` (`detected()`, `rose`, `fell`, `lastActiveMs`). |


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

