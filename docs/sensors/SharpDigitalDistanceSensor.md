# `SharpDigitalDistanceSensor`

Header: [`src/families/infrared/SharpDigitalDistanceSensor.h`](src/families/infrared/SharpDigitalDistanceSensor.h)

## Overview

- **Bus:** GPIO pulse or digital OUT
- **Sketch aliases:** `GP2Y0D805`, `GP2Y0D810`, `GP2Y0D815`
- **Representative examples:** `GP2Y0D810_DetectRead`


## Quick start

```cpp
#include <DiFinders.h>

SharpDigitalDistanceSensor sensor;
// sensor.begin(...);
sensor.read();
```

## Methods

### `SharpDigitalDistanceSensor()`

```cpp
 SharpDigitalDistanceSensor(uint8_t outputPin, SharpDigitalModel model, bool activeLow = true);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint8_t outputPin, SharpDigitalModel model, bool activeLow = true |
| **Returns** | `` |


### `begin()`

```cpp
void begin(uint8_t outputPin, SharpDigitalModel model, bool activeLow = true, uint8_t inputMode = INPUT);
```

Configure pins, bus, or serial stream.

| | |
| --- | --- |
| **Parameters** | Arguments depend on driver (GPIO pins, `Wire&`, `Stream&`, I2C address, SPI CS pin, …). |
| **Returns** | `void` or `bool` — `false` when the bus cannot be opened or the device does not respond. |


### `attachEnablePin()`

```cpp
void attachEnablePin(uint8_t enablePin, bool activeHigh = true);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint8_t enablePin, bool activeHigh = true |
| **Returns** | `void` |


### `enable()`

```cpp
void enable();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `void` |


### `disable()`

```cpp
void disable();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `void` |


### `enabled()`

```cpp
bool enabled() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `detected()`

```cpp
bool detected() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `read()`

```cpp
ProximityReading read();
```

Perform one measurement or refresh GPIO/UART/CAN state.

| | |
| --- | --- |
| **Parameters** | None for most drivers; some GPIO drivers accept `uint32_t nowMs`. |
| **Returns** | `RangeReading`, `ProximityReading`, `MotionReading`, or `bool` success — see class. |


### `range()`

```cpp
SharpDigitalRange range() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `SharpDigitalRange` |


### `minMillimeters()`

```cpp
uint16_t minMillimeters() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t` |


### `maxMillimeters()`

```cpp
uint16_t maxMillimeters() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t` |


### `lastReading()`

```cpp
const ProximityReading& lastReading() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `const ProximityReading&` |


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

