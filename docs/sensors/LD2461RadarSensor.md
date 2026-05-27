# `LD2461RadarSensor`

Header: [`src/families/motion/LD2461RadarSensor.h`](src/families/motion/LD2461RadarSensor.h)

## Overview

- **Bus:** UART
- **Sketch aliases:** `HLK_LD2461`, `HLK_LD2461_UART`
- **Representative examples:** `HLK_LD2461_UART_BasicRead`


## Quick start

```cpp
#include <DiFinders.h>

LD2461RadarSensor sensor;
// Serial1.begin(baud); sensor.begin(Serial1);
sensor.read();
Serial.println(sensor.presenceDetected());
```

## Methods

### `present()`

```cpp
bool present() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `distanceMm()`

```cpp
uint16_t distanceMm() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t` |


### `LD2461RadarSensor()`

```cpp
explicit LD2461RadarSensor(Stream& serial);
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


### `setSensorTimeout()`

```cpp
void setSensorTimeout(uint16_t timeoutMs);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint16_t timeoutMs |
| **Returns** | `void` |


### `setCommandTimeout()`

```cpp
void setCommandTimeout(uint16_t timeoutMs);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint16_t timeoutMs |
| **Returns** | `void` |


### `setTargetFreshnessTimeout()`

```cpp
void setTargetFreshnessTimeout(uint16_t timeoutMs);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint16_t timeoutMs |
| **Returns** | `void` |


### `targetFreshnessTimeout()`

```cpp
uint16_t targetFreshnessTimeout() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t` |


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


### `read()`

```cpp
bool read();
```

Perform one measurement or refresh GPIO/UART/CAN state.

| | |
| --- | --- |
| **Parameters** | None for most drivers; some GPIO drivers accept `uint32_t nowMs`. |
| **Returns** | `RangeReading`, `ProximityReading`, `MotionReading`, or `bool` success — see class. |


### `isConnected()`

```cpp
bool isConnected() const;
```

Whether recent traffic arrived within the driver timeout.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` — may parse a few frames when previously timed out. |


### `occupied()`

```cpp
bool occupied() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `targetCount()`

```cpp
uint8_t targetCount() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint8_t` |


### `freshTargetCount()`

```cpp
uint8_t freshTargetCount() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint8_t` |


### `target()`

```cpp
const Target& target(uint8_t index) const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint8_t index |
| **Returns** | `const Target&` |


### `targetFresh()`

```cpp
bool targetFresh(uint8_t index) const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint8_t index |
| **Returns** | `bool` |


### `lastTargetUpdateMs()`

```cpp
uint32_t lastTargetUpdateMs() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint32_t` |


### `trackingDataFresh()`

```cpp
bool trackingDataFresh() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `nearestTargetDistanceMm()`

```cpp
uint16_t nearestTargetDistanceMm() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t` |


### `reportMode()`

```cpp
ReportMode reportMode() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `ReportMode` |


### `regionsEnabled()`

```cpp
bool regionsEnabled() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `lastCommandCode()`

```cpp
uint8_t lastCommandCode() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint8_t` |


### `lastPayloadLength()`

```cpp
uint8_t lastPayloadLength() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint8_t` |


### `lastPayload()`

```cpp
const uint8_t* lastPayload() const;
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


### `setReportMode()`

```cpp
bool setReportMode(ReportMode mode);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | ReportMode mode |
| **Returns** | `bool` |


### `requestReportMode()`

```cpp
bool requestReportMode();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `requestCoordinates()`

```cpp
bool requestCoordinates();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `requestTargetCount()`

```cpp
bool requestTargetCount();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `requestTrackingSnapshot()`

```cpp
bool requestTrackingSnapshot();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `requestFirmwareVersion()`

```cpp
bool requestFirmwareVersion();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `requestFactoryReset()`

```cpp
bool requestFactoryReset();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `setBaudRate()`

```cpp
bool setBaudRate(BaudRateCode baudRate);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | BaudRateCode baudRate |
| **Returns** | `bool` |


### `setRegionsEnabled()`

```cpp
bool setRegionsEnabled(bool enabled);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | bool enabled |
| **Returns** | `bool` |


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

