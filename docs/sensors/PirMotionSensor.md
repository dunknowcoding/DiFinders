# `PirMotionSensor`

Header: [`src/families/motion/PirMotionSensor.h`](src/families/motion/PirMotionSensor.h)

## Overview

- **Bus:** GPIO pulse or digital OUT
- **Sketch aliases:** `AM312`, `HC_SR501`, `HC_SR505`, `MA312`
- **Representative examples:** `HC_SR501_MotionRead`


## Quick start

```cpp
#include <DiFinders.h>

PirMotionSensor sensor;
// sensor.begin(...);
sensor.read();
```

## Methods

### `PirMotionSensor()`

```cpp
 PirMotionSensor(uint8_t pin, PirModuleProfile profile = PirModuleProfile::Generic, bool activeHigh = true,
                  uint8_t inputMode = INPUT);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint8_t pin, PirModuleProfile profile = PirModuleProfile::Generic, bool activeHigh = true,
                  uint8_t inputMode = INPUT |
| **Returns** | `` |


### `begin()`

```cpp
void begin(uint8_t pin, PirModuleProfile profile = PirModuleProfile::Generic, bool activeHigh = true, uint8_t inputMode = INPUT);
```

Configure pins, bus, or serial stream.

| | |
| --- | --- |
| **Parameters** | Arguments depend on driver (GPIO pins, `Wire&`, `Stream&`, I2C address, SPI CS pin, …). |
| **Returns** | `void` or `bool` — `false` when the bus cannot be opened or the device does not respond. |


### `configureTiming()`

```cpp
void configureTiming(uint16_t debounceMs, uint32_t latchHoldMs);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint16_t debounceMs, uint32_t latchHoldMs |
| **Returns** | `void` |


### `readRawState()`

```cpp
bool readRawState() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `rose()`

```cpp
bool rose() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `fell()`

```cpp
bool fell() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `lastActiveAt()`

```cpp
uint32_t lastActiveAt() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint32_t` |


### `probe()`

```cpp
SensorProbe probe() const;
```

Capability snapshot (bus type, pins, feature flags).

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `SensorProbe` (bus type, pins, feature flags). |


### `lastReading()`

```cpp
const MotionReading& lastReading() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `const MotionReading&` |


### `attachInterruptPin()`

```cpp
bool attachInterruptPin(uint8_t pin, uint8_t mode = DF_IRQ_RISING);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint8_t pin, uint8_t mode = DF_IRQ_RISING |
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


### `interruptPending()`

```cpp
bool interruptPending() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


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

