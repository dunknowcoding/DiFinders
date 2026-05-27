# `VL53L1XSpiSensor`

Header: [`src/families/tof/VL53L1XSpiSensor.h`](src/families/tof/VL53L1XSpiSensor.h)

## Overview

- **Bus:** SPI
- **Sketch aliases:** `TOF400C_SPI`, `VL53L1CB_SPI`, `VL53L1X_SPI`
- **Representative examples:** `VL53L1X_SPI`, `VL53L1X_SpiBurstRead`


## Quick start

```cpp
#include <DiFinders.h>

VL53L1XSpiSensor sensor;
// sensor.begin(...);  // see examples/
if (sensor.ready()) {
  RangeReading r = sensor.read();
  Serial.println(r.distanceMm);
}
```

## Methods

### `begin()`

```cpp
public:
  bool begin(uint8_t csPin, uint32_t hz = 2000000UL);
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


### `setTimeout()`

```cpp
void setTimeout(uint16_t timeoutMs);
```

Set I/O or command timeout for blocking reads.

| | |
| --- | --- |
| **Parameters** | Timeout in milliseconds. |
| **Returns** | `void`. |


### `startContinuous()`

```cpp
void startContinuous(uint32_t periodMs = 60);
```

Start free-running measurements (ToF / UART streaming).

| | |
| --- | --- |
| **Parameters** | Period in ms or µs depending on driver. |
| **Returns** | `void`. |


### `stopContinuous()`

```cpp
void stopContinuous();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `void` |


### `dataReady()`

```cpp
bool dataReady();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `read()`

```cpp
RangeReading read(bool blocking = true);
```

Perform one measurement or refresh GPIO/UART/CAN state.

| | |
| --- | --- |
| **Parameters** | None for most drivers; some GPIO drivers accept `uint32_t nowMs`. |
| **Returns** | `RangeReading`, `ProximityReading`, `MotionReading`, or `bool` success — see class. |


### `readIfReady()`

```cpp
bool readIfReady(RangeReading& reading);
```

Non-blocking read when continuous mode has a fresh sample.

| | |
| --- | --- |
| **Parameters** | `RangeReading& out`. |
| **Returns** | `bool` — `true` when `out` was updated. |


### `millimeters()`

```cpp
uint16_t millimeters();
```

Convenience read returning distance in millimetres.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t` distance (may trigger a new measurement). |


### `attachDataReadyPin()`

```cpp
bool attachDataReadyPin(uint8_t pin, uint8_t mode = DF_IRQ_FALLING);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint8_t pin, uint8_t mode = DF_IRQ_FALLING |
| **Returns** | `bool` |


### `detachDataReadyPin()`

```cpp
void detachDataReadyPin();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `void` |


### `takeDataReadyEvent()`

```cpp
bool takeDataReadyEvent();
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


### `lastReading()`

```cpp
const RangeReading& lastReading() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `const RangeReading&` |


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

