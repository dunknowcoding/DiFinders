# `LD2410RadarSensor`

Header: [`src/families/motion/LD2410RadarSensor.h`](src/families/motion/LD2410RadarSensor.h)

## Overview

- **Bus:** UART
- **Sketch aliases:** `HLK_LD2410B`, `HLK_LD2410C_UART`, `HLK_LD2410S`, `HLK_LD2410S_UART`, `HLK_LD2410_UART`
- **Representative examples:** `HLK_LD2410_UartHighRateRead`, `HLK_LD2410S_PresenceRead`, `HLK_LD2410S_UART_BasicRead`, `HLK_LD2410_PresenceRead`


## Quick start

```cpp
#include <DiFinders.h>

LD2410RadarSensor sensor;
// sensor.begin(...);  // see examples/
if (sensor.ready()) {
  RangeReading r = sensor.read();
  Serial.println(r.distanceMm);
}
```

## Methods

### `LD2410RadarSensor()`

```cpp
explicit LD2410RadarSensor(Stream& serial);
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


### `ready()`

```cpp
bool ready() const;
```

Whether the driver finished `begin()` and can accept reads.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` — `true` when configured (not always a live target). |


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


### `stationaryTargetDetected()`

```cpp
bool stationaryTargetDetected() const;
```

True when a stationary target is reported in the last data frame.

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


### `stationaryTargetDistanceCm()`

```cpp
uint16_t stationaryTargetDistanceCm() const;
```

Distance to the stationary target from the last radar frame.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t` centimetres (0 if no stationary target). |


### `movingTargetDistanceCm()`

```cpp
uint16_t movingTargetDistanceCm() const;
```

Distance to the moving target from the last radar frame.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t` centimetres (0 if no moving target). |


### `stationaryTargetEnergy()`

```cpp
uint8_t stationaryTargetEnergy() const;
```

Stationary-target energy / confidence (0–100).

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint8_t`. |


### `movingTargetEnergy()`

```cpp
uint8_t movingTargetEnergy() const;
```

Moving-target energy / confidence (0–100, driver-clamped).

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint8_t`. |


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


### `firmwareBugfixVersion()`

```cpp
uint32_t firmwareBugfixVersion() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint32_t` |


### `maxGate()`

```cpp
uint8_t maxGate() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint8_t` |


### `maxMovingGate()`

```cpp
uint8_t maxMovingGate() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint8_t` |


### `maxStationaryGate()`

```cpp
uint8_t maxStationaryGate() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint8_t` |


### `idleTimeoutSeconds()`

```cpp
uint16_t idleTimeoutSeconds() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t` |


### `motionSensitivity()`

```cpp
uint8_t motionSensitivity(uint8_t gate) const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint8_t gate |
| **Returns** | `uint8_t` |


### `stationarySensitivity()`

```cpp
uint8_t stationarySensitivity(uint8_t gate) const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint8_t gate |
| **Returns** | `uint8_t` |


### `requestFirmwareVersion()`

```cpp
bool requestFirmwareVersion();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `requestCurrentConfiguration()`

```cpp
bool requestCurrentConfiguration();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
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


### `requestStartEngineeringMode()`

```cpp
bool requestStartEngineeringMode();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `requestEndEngineeringMode()`

```cpp
bool requestEndEngineeringMode();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `setMaxValues()`

```cpp
bool setMaxValues(uint8_t movingGate, uint8_t stationaryGate, uint16_t inactivitySeconds);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint8_t movingGate, uint8_t stationaryGate, uint16_t inactivitySeconds |
| **Returns** | `bool` |


### `setGateSensitivity()`

```cpp
bool setGateSensitivity(uint8_t gate, uint8_t movingSensitivity, uint8_t stationarySensitivity);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint8_t gate, uint8_t movingSensitivity, uint8_t stationarySensitivity |
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

