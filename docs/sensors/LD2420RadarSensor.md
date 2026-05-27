# `LD2420RadarSensor`

Header: [`src/families/motion/LD2420RadarSensor.h`](src/families/motion/LD2420RadarSensor.h)

## Overview

- **Bus:** UART
- **Sketch aliases:** `HLK_LD2420`, `HLK_LD2420_UART`
- **Representative examples:** `HLK_LD2420_UART_BasicRead`


## Quick start

```cpp
#include <DiFinders.h>

LD2420RadarSensor sensor;
// Serial1.begin(baud); sensor.begin(Serial1);
sensor.read();
Serial.println(sensor.presenceDetected());
```

## Methods

### `begin()`

```cpp
void begin(Stream& serial, FirmwareGeneration generation, bool waitForRadar = false);
```

Configure pins, bus, or serial stream.

| | |
| --- | --- |
| **Parameters** | Arguments depend on driver (GPIO pins, `Wire&`, `Stream&`, I2C address, SPI CS pin, …). |
| **Returns** | `void` or `bool` — `false` when the bus cannot be opened or the device does not respond. |


### `defaultBaudRate()`

```cpp
uint32_t defaultBaudRate() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `uint32_t` |


### `setDefaultBaudRate()`

```cpp
void setDefaultBaudRate(uint32_t baudRate);
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | uint32_t baudRate |
| **Returns** | `void` |


### `useLegacyDefaultBaudRate()`

```cpp
void useLegacyDefaultBaudRate();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `void` |


### `useModernDefaultBaudRate()`

```cpp
void useModernDefaultBaudRate();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `void` |


### `firmwareGenerationHint()`

```cpp
FirmwareGeneration firmwareGenerationHint() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `FirmwareGeneration` |


### `isUsingLegacyDefaultBaudRate()`

```cpp
bool isUsingLegacyDefaultBaudRate() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `isUsingModernDefaultBaudRate()`

```cpp
bool isUsingModernDefaultBaudRate() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `tunedProfile()`

```cpp
TunedProfile tunedProfile() const;
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `TunedProfile` |


### `applyFastPresenceProfile()`

```cpp
bool applyFastPresenceProfile();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `applyBalancedIndoorProfile()`

```cpp
bool applyBalancedIndoorProfile();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `applyLongRangeProfile()`

```cpp
bool applyLongRangeProfile();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `bool` |


### `clearTunedProfile()`

```cpp
void clearTunedProfile();
```

See header implementation in `src/families/`.

| | |
| --- | --- |
| **Parameters** | None. |
| **Returns** | `void` |

