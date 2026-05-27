# `MmwavePresenceSensor`

Header: [`src/families/motion/MmwavePresenceSensor.h`](src/families/motion/MmwavePresenceSensor.h)

## Overview

- **Bus:** GPIO pulse or digital OUT
- **Sketch aliases:** `HLK_LD1125H`, `HLK_LD1155H`, `HLK_LD2410`, `HLK_LD2410C`, `HLK_LD2410S_OUT`, `HLK_LD2412_OUT`
- **Representative examples:** `HLK_LD2410_PresenceRead`, `HLK_LD2410S_PresenceRead`, `HLK_LD2412_PresenceRead`


## Quick start

```cpp
#include <DiFinders.h>

MmwavePresenceSensor sensor;
// Serial1.begin(baud); sensor.begin(Serial1);
sensor.read();
Serial.println(sensor.presenceDetected());
```

## Methods

### `begin()`

```cpp
void begin();
```

Configure pins, bus, or serial stream.

| | |
| --- | --- |
| **Parameters** | Arguments depend on driver (GPIO pins, `Wire&`, `Stream&`, I2C address, SPI CS pin, …). |
| **Returns** | `void` or `bool` — `false` when the bus cannot be opened or the device does not respond. |


### `setDebounceMs()`

```cpp
void setDebounceMs(uint16_t debounceMs);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint16_t debounceMs |
| **Returns** | `void` |


### `setPresenceHoldMs()`

```cpp
void setPresenceHoldMs(uint16_t holdMs);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint16_t holdMs |
| **Returns** | `void` |


### `read()`

```cpp
bool read();
```

Perform one measurement or refresh GPIO/UART/CAN state.

| | |
| --- | --- |
| **Parameters** | None for most drivers; some GPIO drivers accept `uint32_t nowMs`. |
| **Returns** | `RangeReading`, `ProximityReading`, `MotionReading`, or `bool` success — see class. |


### `presence()`

```cpp
bool presence() const;
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


### `motion()`

```cpp
MotionReading motion();
```

Last motion/presence reading structure.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `MotionReading` (`detected()`, `rose`, `fell`, `lastActiveMs`). |


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
SelfTestReport selfTest() const;
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

