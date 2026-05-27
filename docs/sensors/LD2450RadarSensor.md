# `LD2450RadarSensor`

Header: [`src/families/motion/LD2450RadarSensor.h`](src/families/motion/LD2450RadarSensor.h)

## Overview

- **Bus:** UART
- **Sketch aliases:** `HLK_LD2402`, `HLK_LD2450`, `HLK_LD2451`
- **Representative examples:** `HLK_LD2461_UART_BasicRead`


## Quick start

```cpp
#include <DiFinders.h>

LD2450RadarSensor sensor;
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


### `angleDegrees()`

```cpp
float angleDegrees() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `float` |


### `distanceMm()`

```cpp
uint16_t distanceMm() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t` |


### `LD2450RadarSensor()`

```cpp
explicit LD2450RadarSensor(Stream& serial);
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


### `setFlipXAxis()`

```cpp
void setFlipXAxis(bool enabled);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | bool enabled |
| **Returns** | `void` |


### `setFastOffDetection()`

```cpp
void setFastOffDetection(bool enabled);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | bool enabled |
| **Returns** | `void` |


### `setMaxDetectionDistanceMm()`

```cpp
void setMaxDetectionDistanceMm(int16_t distanceMm);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | int16_t distanceMm |
| **Returns** | `void` |


### `setMaxDistanceMarginMm()`

```cpp
void setMaxDistanceMarginMm(int16_t marginMm);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | int16_t marginMm |
| **Returns** | `void` |


### `setTiltAngleLimits()`

```cpp
void setTiltAngleLimits(float minAngleDegrees, float maxAngleDegrees);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | float minAngleDegrees, float maxAngleDegrees |
| **Returns** | `void` |


### `setTiltAngleMargin()`

```cpp
void setTiltAngleMargin(float marginDegrees);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | float marginDegrees |
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


### `target()`

```cpp
const Target& target(uint8_t index) const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint8_t index |
| **Returns** | `const Target&` |


### `multiTargetModeEnabled()`

```cpp
bool multiTargetModeEnabled() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `bluetoothEnabled()`

```cpp
bool bluetoothEnabled() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `firmwareMajorVersion()`

```cpp
uint8_t firmwareMajorVersion() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint8_t` |


### `firmwareMinorVersion()`

```cpp
uint8_t firmwareMinorVersion() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint8_t` |


### `firmwarePatchVersion()`

```cpp
uint32_t firmwarePatchVersion() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint32_t` |


### `requestFirmwareVersion()`

```cpp
bool requestFirmwareVersion();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `requestTrackingMode()`

```cpp
bool requestTrackingMode();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `setTrackingMode()`

```cpp
bool setTrackingMode(bool multiTargetMode);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | bool multiTargetMode |
| **Returns** | `bool` |


### `setBluetoothEnabled()`

```cpp
bool setBluetoothEnabled(bool enabled);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | bool enabled |
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


### `requestRestart()`

```cpp
bool requestRestart();
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

