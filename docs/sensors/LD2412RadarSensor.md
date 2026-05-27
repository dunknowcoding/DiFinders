# `LD2412RadarSensor`

Header: [`src/families/motion/LD2412RadarSensor.h`](src/families/motion/LD2412RadarSensor.h)

## Overview

- **Bus:** UART
- **Sketch aliases:** `HLK_LD2412`, `HLK_LD2412_UART`
- **Representative examples:** `HLK_LD2412_UART_BasicRead`


## Quick start

```cpp
#include <DiFinders.h>

LD2412RadarSensor sensor;
// Serial1.begin(baud); sensor.begin(Serial1);
sensor.read();
Serial.println(sensor.presenceDetected());
```

## Methods
