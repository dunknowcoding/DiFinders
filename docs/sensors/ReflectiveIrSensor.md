# `ReflectiveIrSensor`

Header: [`src/families/infrared/ReflectiveIrSensor.h`](src/families/infrared/ReflectiveIrSensor.h)

## Overview

- **Bus:** GPIO pulse or digital OUT
- **Sketch aliases:** `IR_ObstacleAvoidance`, `LM393_IR`, `TCRT5000`, `WWZMDiB_IR`
- **Representative examples:** `TCRT5000_ReflectiveRead`


## Quick start

```cpp
#include <DiFinders.h>

ReflectiveIrSensor sensor;
// sensor.begin(...);  // see examples/
if (sensor.ready()) {
  RangeReading r = sensor.read();
  Serial.println(r.distanceMm);
}
```

## Methods

### `ReflectiveIrSensor()`

```cpp
 ReflectiveIrSensor(uint8_t digitalPin, uint8_t analogPin = kUnassignedPin, uint8_t emitterPin = kUnassignedPin);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint8_t digitalPin, uint8_t analogPin = kUnassignedPin, uint8_t emitterPin = kUnassignedPin |
| **Returns** | `` |


### `begin()`

```cpp
void begin(uint8_t digitalPin, uint8_t analogPin = kUnassignedPin, uint8_t emitterPin = kUnassignedPin);
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


### `setProfile()`

```cpp
void setProfile(ReflectiveIrProfile profile);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | ReflectiveIrProfile profile |
| **Returns** | `void` |


### `setDigitalActiveLow()`

```cpp
void setDigitalActiveLow(bool activeLow);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | bool activeLow |
| **Returns** | `void` |


### `setCalibrationWindow()`

```cpp
void setCalibrationWindow(uint16_t rawLow, uint16_t rawHigh);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint16_t rawLow, uint16_t rawHigh |
| **Returns** | `void` |


### `enableEmitter()`

```cpp
void enableEmitter(bool enabled);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | bool enabled |
| **Returns** | `void` |


### `emitterEnabled()`

```cpp
bool emitterEnabled() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `readDigital()`

```cpp
bool readDigital();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `readAnalogRaw()`

```cpp
uint16_t readAnalogRaw();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t` |


### `readAmbientCompensatedRaw()`

```cpp
uint16_t readAmbientCompensatedRaw();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t` |


### `read()`

```cpp
ProximityReading read();
```

Perform one measurement or refresh GPIO/UART/CAN state.

| | |
| --- | --- |
| **Parameters** | None for most drivers; some GPIO drivers accept `uint32_t nowMs`. |
| **Returns** | `RangeReading`, `ProximityReading`, `MotionReading`, or `bool` success — see class. |


### `active()`

```cpp
bool active();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `strength()`

```cpp
uint16_t strength();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t` |


### `raw()`

```cpp
uint16_t raw();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t` |


### `compensated()`

```cpp
uint16_t compensated();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t` |


### `setAnalogThresholdPermille()`

```cpp
void setAnalogThresholdPermille(uint16_t thresholdPermille);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint16_t thresholdPermille |
| **Returns** | `void` |


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
const ProximityReading& lastReading() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `const ProximityReading&` |


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

