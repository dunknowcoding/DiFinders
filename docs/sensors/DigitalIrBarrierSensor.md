# `DigitalIrBarrierSensor`

Header: [`src/families/infrared/DigitalIrBarrierSensor.h`](src/families/infrared/DigitalIrBarrierSensor.h)

## Overview

- **Bus:** GPIO pulse or digital OUT
- **Sketch aliases:** `E18_D60NK`, `E18_D80NK`, `IR_Photoelectric_Switch`
- **Representative examples:** `E18_D80NK_BasicRead`


## Quick start

```cpp
#include <DiFinders.h>

DigitalIrBarrierSensor sensor;
// sensor.begin(...);  // see examples/
if (sensor.ready()) {
  RangeReading r = sensor.read();
  Serial.println(r.distanceMm);
}
```

## Methods

### `begin()`

```cpp
void begin(uint8_t outputPin, DigitalIrBarrierProfile profile = DigitalIrBarrierProfile::E18_D80NK);
```

Configure pins, bus, or serial stream.

| | |
| --- | --- |
| **Parameters** | Arguments depend on driver (GPIO pins, `Wire&`, `Stream&`, I2C address, SPI CS pin, …). |
| **Returns** | `void` or `bool` — `false` when the bus cannot be opened or the device does not respond. |


### `setActiveLow()`

```cpp
void setActiveLow(bool activeLow);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | bool activeLow |
| **Returns** | `void` |


### `setDebounceMs()`

```cpp
void setDebounceMs(uint16_t debounceMs);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint16_t debounceMs |
| **Returns** | `void` |


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


### `detected()`

```cpp
bool detected() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `proximity()`

```cpp
ProximityReading proximity() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `ProximityReading` |


### `lastReading()`

```cpp
const ProximityReading& lastReading() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `const ProximityReading&` |


### `attachInterruptPin()`

```cpp
bool attachInterruptPin(uint8_t pin, uint8_t mode = DF_IRQ_CHANGE);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint8_t pin, uint8_t mode = DF_IRQ_CHANGE |
| **Returns** | `bool` |


### `detachInterruptPin()`

```cpp
void detachInterruptPin();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `void` |


### `takeInterruptEvent()`

```cpp
bool takeInterruptEvent();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
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


### `health()`

```cpp
SensorHealth health() const;
```

Unified runtime health for diagnostics.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `SensorHealth` (`ready`, `lastStatus`, `busErrorCode`, `faultCount`, …). |

