# `PulseUltrasonicSensor`

Header: [`src/families/ultrasonic/PulseUltrasonicSensor.h`](src/families/ultrasonic/PulseUltrasonicSensor.h)

## Overview

- **Bus:** GPIO pulse or digital OUT
- **Sketch aliases:** `AJ_SR04M`, `HC_SR04`, `HY_SRF05`, `JSN_SR04T`, `MultiSensor_QuickRead`, `Platform_MultiRead`, `RCWL1601`, `RCWL1655`, `RCWL1670`, `SRF05`
- **Representative examples:** `HC_SR04_BasicRead`, `JSN_SR04T_BasicRead`, `MultiSensor_QuickRead`, `Platform_MultiRead`


## Quick start

```cpp
#include <DiFinders.h>

PulseUltrasonicSensor sensor;
// sensor.begin(...);  // see examples/
if (sensor.ready()) {
  RangeReading r = sensor.read();
  Serial.println(r.distanceMm);
}
```

## Methods

### `PulseUltrasonicSensor()`

```cpp
 PulseUltrasonicSensor(uint8_t triggerPin, uint8_t echoPin, uint8_t echoPinMode = INPUT);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint8_t triggerPin, uint8_t echoPin, uint8_t echoPinMode = INPUT |
| **Returns** | `` |


### `begin()`

```cpp
void begin(uint8_t triggerPin, uint8_t echoPin, uint8_t echoPinMode = INPUT);
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


### `configureTiming()`

```cpp
void configureTiming(uint32_t timeoutUs, uint16_t triggerPulseUs = 10, uint16_t settleUs = 2);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint32_t timeoutUs, uint16_t triggerPulseUs = 10, uint16_t settleUs = 2 |
| **Returns** | `void` |


### `configureEnvironment()`

```cpp
void configureEnvironment(uint32_t soundSpeedMmPerSecond);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint32_t soundSpeedMmPerSecond |
| **Returns** | `void` |


### `setMinDistanceMm()`

```cpp
void setMinDistanceMm(uint16_t minDistanceMm);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint16_t minDistanceMm |
| **Returns** | `void` |


### `singleShot()`

```cpp
RangeReading singleShot();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `RangeReading` |


### `read()`

```cpp
RangeReading read();
```

Perform one measurement or refresh GPIO/UART/CAN state.

| | |
| --- | --- |
| **Parameters** | None for most drivers; some GPIO drivers accept `uint32_t nowMs`. |
| **Returns** | `RangeReading`, `ProximityReading`, `MotionReading`, or `bool` success — see class. |


### `singleShotMedian3()`

```cpp
RangeReading singleShotMedian3();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `RangeReading` |


### `millimeters()`

```cpp
uint16_t millimeters();
```

Convenience read returning distance in millimetres.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t` distance (may trigger a new measurement). |


### `medianMillimeters()`

```cpp
uint16_t medianMillimeters();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t` |


### `centimeters()`

```cpp
uint16_t centimeters();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t` |


### `medianCentimeters()`

```cpp
uint16_t medianCentimeters();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t` |


### `lastPulseUs()`

```cpp
uint32_t lastPulseUs() const;
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
const RangeReading& lastReading() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `const RangeReading&` |


### `attachEchoInterrupt()`

```cpp
bool attachEchoInterrupt(uint8_t mode = DF_IRQ_CHANGE);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint8_t mode = DF_IRQ_CHANGE |
| **Returns** | `bool` |


### `detachEchoInterrupt()`

```cpp
void detachEchoInterrupt();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `void` |


### `startPing()`

```cpp
void startPing();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `void` |


### `takeEchoEvent()`

```cpp
bool takeEchoEvent();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `completePing()`

```cpp
RangeReading completePing();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `RangeReading` |


### `setSimulatedPulseUs()`

```cpp
void setSimulatedPulseUs(uint32_t pulseUs);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint32_t pulseUs |
| **Returns** | `void` |


### `clearSimulatedPulse()`

```cpp
void clearSimulatedPulse();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `void` |


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

