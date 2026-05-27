# `MaxBotixAnalogSensor`

Header: [`src/families/ultrasonic/MaxBotixAnalogSensor.h`](src/families/ultrasonic/MaxBotixAnalogSensor.h)

## Overview

- **Bus:** Analog
- **Sketch aliases:** `MB10XXAnalog`, `MB7XXXAnalog`
- **Representative examples:** `MB10XXAnalog_BasicRead`


## Quick start

```cpp
#include <DiFinders.h>

MaxBotixAnalogSensor sensor;
// sensor.begin(...);  // see examples/
if (sensor.ready()) {
  RangeReading r = sensor.read();
  Serial.println(r.distanceMm);
}
```

## Methods

### `MaxBotixAnalogSensor()`

```cpp
 MaxBotixAnalogSensor(uint8_t analogPin, uint16_t adcReferenceMv = 3300,
                       uint16_t adcMax = DI_FINDERS_DEFAULT_ADC_MAX, uint16_t sensorSupplyMv = 0);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint8_t analogPin, uint16_t adcReferenceMv = 3300,
                       uint16_t adcMax = DI_FINDERS_DEFAULT_ADC_MAX, uint16_t sensorSupplyMv = 0 |
| **Returns** | `` |


### `begin()`

```cpp
void begin(uint8_t analogPin, uint16_t adcReferenceMv = 3300,
             uint16_t adcMax = DI_FINDERS_DEFAULT_ADC_MAX, uint16_t sensorSupplyMv = 0);
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


### `configureElectrical()`

```cpp
void configureElectrical(uint16_t adcReferenceMv, uint16_t adcMax, uint16_t sensorSupplyMv = 0);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint16_t adcReferenceMv, uint16_t adcMax, uint16_t sensorSupplyMv = 0 |
| **Returns** | `void` |


### `configureRange()`

```cpp
void configureRange(uint16_t minDistanceMm, uint16_t maxDistanceMm);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint16_t minDistanceMm, uint16_t maxDistanceMm |
| **Returns** | `void` |


### `attachEnablePin()`

```cpp
void attachEnablePin(uint8_t enablePin, bool activeHigh = true);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint8_t enablePin, bool activeHigh = true |
| **Returns** | `void` |


### `setCalibration()`

```cpp
void setCalibration(const CalibrationData& calibration);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | const CalibrationData& calibration |
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


### `readRaw()`

```cpp
uint16_t readRaw();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t` |


### `readVoltageMv()`

```cpp
uint16_t readVoltageMv();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t` |


### `read()`

```cpp
RangeReading read();
```

Perform one measurement or refresh GPIO/UART/CAN state.

| | |
| --- | --- |
| **Parameters** | None for most drivers; some GPIO drivers accept `uint32_t nowMs`. |
| **Returns** | `RangeReading`, `ProximityReading`, `MotionReading`, or `bool` success — see class. |


### `millimeters()`

```cpp
uint16_t millimeters();
```

Convenience read returning distance in millimetres.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t` distance (may trigger a new measurement). |


### `averageMillimeters()`

```cpp
uint16_t averageMillimeters(uint8_t samples = 3);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint8_t samples = 3 |
| **Returns** | `uint16_t` |


### `raw()`

```cpp
uint16_t raw() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t` |


### `voltage()`

```cpp
uint16_t voltage() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint16_t` |


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

