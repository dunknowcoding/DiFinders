# `LD2411RadarSensor`

Header: [`src/families/motion/LD2411RadarSensor.h`](src/families/motion/LD2411RadarSensor.h)

## Overview

- **Bus:** UART
- **Sketch aliases:** `HLK_LD2411`, `HLK_LD2411S`, `HLK_LD2411S_UART`, `HLK_LD2411_UART`
- **Representative examples:** `HLK_LD2411_UART_BasicRead`


## Quick start

```cpp
#include <DiFinders.h>

LD2411RadarSensor sensor;
// Serial1.begin(baud); sensor.begin(Serial1);
sensor.read();
Serial.println(sensor.presenceDetected());
```

## Methods

### `LD2411RadarSensor()`

```cpp
explicit LD2411RadarSensor(Stream& serial);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | Stream& serial |
| **Returns** | `explicit` |


### `begin()`

```cpp
void begin(Stream& serial, bool waitForRadar = false);
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


### `setCommandTimeout()`

```cpp
void setCommandTimeout(uint16_t timeoutMs);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint16_t timeoutMs |
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
bool isConnected();
```

Whether recent traffic arrived within the driver timeout.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` — may parse a few frames when previously timed out. |


### `presenceDetected()`

```cpp
bool presenceDetected() const;
```

True when the radar reports any target (moving or stationary).

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool`. |


### `movingTargetDetected()`

```cpp
bool movingTargetDetected() const;
```

True when a moving target is reported in the last data frame.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool`. |


### `stationaryTargetDetected()`

```cpp
bool stationaryTargetDetected() const;
```

True when a stationary target is reported in the last data frame.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool`. |


### `targetType()`

```cpp
uint8_t targetType() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint8_t` |


### `detectedDistanceCm()`

```cpp
uint16_t detectedDistanceCm() const;
```

Primary reported distance in centimetres (LD2410-style frames).

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t`. |


### `bluetoothEnabled()`

```cpp
bool bluetoothEnabled() const;
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


### `requestRestart()`

```cpp
bool requestRestart();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
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

