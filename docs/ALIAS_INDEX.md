# DiFinders SKU alias index

Marketplace names (sketch types) map to family drivers. Include `#include <DiFinders.h>` only.

Example folders are authoritative when listed in [`.github/scripts/alias_examples.yaml`](../.github/scripts/alias_examples.yaml); otherwise the generator picks the best name match under `examples/`.

| Alias (sketch type) | Bus | Family driver | Primary example | Also see |
| --- | --- | --- | --- | --- |
| `A01NYUB` | UART | [`A02YYUWSensor`](sensors/A02YYUWSensor.md) | `A02YYUW_BasicRead` | ? |
| `A02YYUW` | UART | [`A02YYUWSensor`](sensors/A02YYUWSensor.md) | `A02YYUW_BasicRead` | ? |
| `AJ_SR04M` | GPIO | [`PulseUltrasonicSensor`](sensors/PulseUltrasonicSensor.md) | `HC_SR04_BasicRead` | ? |
| `AM312` | GPIO | [`PirMotionSensor`](sensors/PirMotionSensor.md) | `HC_SR501_MotionRead` | ? |
| `Benewake_TF03` | UART | [`TFMiniSensor`](sensors/TFMiniSensor.md) | `TFLuna_BasicRead` | ? |
| `Benewake_TF03_CAN` | CAN | [`TF03CanSensor`](sensors/TF03CanSensor.md) | `Benewake_TF03_CAN_MCP2515_ESP32` | `Benewake_TF03_CAN_MCP2515_UnoR4`, `Benewake_TF03_CAN_SN65HVD230_ESP32` |
| `CMOSLaserDistanceSensor` | UART | [`NoopLoopToFSensor`](sensors/NoopLoopToFSensor.md) | `TOFSenseF2P_UART_BasicRead` | ? |
| `DFRobotMmWaveC4001` | UART | [`OpenProtocolMmwaveSensor`](sensors/OpenProtocolMmwaveSensor.md) | `OpenProtocol_Mmwave_BasicRead` | ? |
| `DFRobot_C4001` | UART | [`OpenProtocolMmwaveSensor`](sensors/OpenProtocolMmwaveSensor.md) | `OpenProtocol_Mmwave_BasicRead` | ? |
| `DYP_A01` | UART | [`A02YYUWSensor`](sensors/A02YYUWSensor.md) | `DYP_A01_BasicRead` | ? |
| `DYP_A01A` | UART | [`A02YYUWSensor`](sensors/A02YYUWSensor.md) | `DYP_A01_BasicRead` | ? |
| `DYP_A01B` | UART | [`A02YYUWSensor`](sensors/A02YYUWSensor.md) | `DYP_A01_BasicRead` | ? |
| `E18_D60NK` | GPIO | [`DigitalIrBarrierSensor`](sensors/DigitalIrBarrierSensor.md) | `E18_D80NK_BasicRead` | ? |
| `E18_D80NK` | GPIO | [`DigitalIrBarrierSensor`](sensors/DigitalIrBarrierSensor.md) | `E18_D80NK_BasicRead` | ? |
| `GP2Y0A02` | Analog | [`SharpAnalogDistanceSensor`](sensors/SharpAnalogDistanceSensor.md) | `GP2Y0A21_BasicRead` | ? |
| `GP2Y0A21` | Analog | [`SharpAnalogDistanceSensor`](sensors/SharpAnalogDistanceSensor.md) | `GP2Y0A21_FilteredRead` | ? |
| `GP2Y0A41` | Analog | [`SharpAnalogDistanceSensor`](sensors/SharpAnalogDistanceSensor.md) | `GP2Y0A21_BasicRead` | ? |
| `GP2Y0A51` | Analog | [`SharpAnalogDistanceSensor`](sensors/SharpAnalogDistanceSensor.md) | `GP2Y0A21_BasicRead` | ? |
| `GP2Y0A60` | Analog | [`SharpAnalogDistanceSensor`](sensors/SharpAnalogDistanceSensor.md) | `GP2Y0A21_BasicRead` | ? |
| `GP2Y0D805` | GPIO | [`SharpDigitalDistanceSensor`](sensors/SharpDigitalDistanceSensor.md) | `GP2Y0D810_DetectRead` | ? |
| `GP2Y0D810` | GPIO | [`SharpDigitalDistanceSensor`](sensors/SharpDigitalDistanceSensor.md) | `GP2Y0D810_DetectRead` | ? |
| `GP2Y0D815` | GPIO | [`SharpDigitalDistanceSensor`](sensors/SharpDigitalDistanceSensor.md) | `GP2Y0D810_DetectRead` | ? |
| `GP2Y0E03` | I2C | [`Gp2Y0E03Sensor`](sensors/Gp2Y0E03Sensor.md) | `GP2Y0E03_AnalogRead` | ? |
| `GY_US42` | I2C | [`GyUs42I2cSensor`](sensors/GyUs42I2cSensor.md) | `GY_US42_Pulse_BasicRead` | ? |
| `GY_US42V2` | I2C | [`GyUs42I2cSensor`](sensors/GyUs42I2cSensor.md) | `GY_US42_I2C_BasicRead` | ? |
| `GY_US42_Pulse` | GPIO | [`GyUs42PulseSensor`](sensors/GyUs42PulseSensor.md) | `GY_US42_Pulse_BasicRead` | ? |
| `GY_US42_UART` | UART | [`A02YYUWSensor`](sensors/A02YYUWSensor.md) | `GY_US42_UART_BasicRead` | ? |
| `HC_SR04` | GPIO | [`PulseUltrasonicSensor`](sensors/PulseUltrasonicSensor.md) | `HC_SR04_BasicRead` | ? |
| `HC_SR501` | GPIO | [`PirMotionSensor`](sensors/PirMotionSensor.md) | `HC_SR501_MotionRead` | ? |
| `HC_SR505` | GPIO | [`PirMotionSensor`](sensors/PirMotionSensor.md) | `HC_SR501_MotionRead` | ? |
| `HLK_LD1125H` | GPIO | [`MmwavePresenceSensor`](sensors/MmwavePresenceSensor.md) | `HLK_LD2410_PresenceRead` | ? |
| `HLK_LD1155H` | GPIO | [`MmwavePresenceSensor`](sensors/MmwavePresenceSensor.md) | `HLK_LD2410_PresenceRead` | ? |
| `HLK_LD2402` | UART | [`LD2450RadarSensor`](sensors/LD2450RadarSensor.md) | `HLK_LD2461_UART_BasicRead` | ? |
| `HLK_LD2410` | GPIO | [`MmwavePresenceSensor`](sensors/MmwavePresenceSensor.md) | `HLK_LD2410_PresenceRead` | ? |
| `HLK_LD2410B` | UART | [`LD2410RadarSensor`](sensors/LD2410RadarSensor.md) | `HLK_LD2410_UartHighRateRead` | ? |
| `HLK_LD2410C` | GPIO | [`MmwavePresenceSensor`](sensors/MmwavePresenceSensor.md) | `HLK_LD2410_PresenceRead` | ? |
| `HLK_LD2410C_UART` | UART | [`LD2410RadarSensor`](sensors/LD2410RadarSensor.md) | `HLK_LD2410_UartHighRateRead` | ? |
| `HLK_LD2410S` | UART | [`LD2410RadarSensor`](sensors/LD2410RadarSensor.md) | `HLK_LD2410S_PresenceRead` | ? |
| `HLK_LD2410S_OUT` | GPIO | [`MmwavePresenceSensor`](sensors/MmwavePresenceSensor.md) | `HLK_LD2410S_PresenceRead` | ? |
| `HLK_LD2410S_UART` | UART | [`LD2410RadarSensor`](sensors/LD2410RadarSensor.md) | `HLK_LD2410S_UART_BasicRead` | ? |
| `HLK_LD2410_UART` | UART | [`LD2410RadarSensor`](sensors/LD2410RadarSensor.md) | `HLK_LD2410_UartHighRateRead` | `HLK_LD2410_PresenceRead` |
| `HLK_LD2411` | UART | [`LD2411RadarSensor`](sensors/LD2411RadarSensor.md) | `HLK_LD2411_UART_BasicRead` | ? |
| `HLK_LD2411S` | UART | [`LD2411RadarSensor`](sensors/LD2411RadarSensor.md) | `HLK_LD2411_UART_BasicRead` | ? |
| `HLK_LD2411S_UART` | UART | [`LD2411RadarSensor`](sensors/LD2411RadarSensor.md) | `HLK_LD2411_UART_BasicRead` | ? |
| `HLK_LD2411_UART` | UART | [`LD2411RadarSensor`](sensors/LD2411RadarSensor.md) | `HLK_LD2411_UART_BasicRead` | ? |
| `HLK_LD2412` | UART | [`LD2412RadarSensor`](sensors/LD2412RadarSensor.md) | `HLK_LD2412_UART_BasicRead` | ? |
| `HLK_LD2412_OUT` | GPIO | [`MmwavePresenceSensor`](sensors/MmwavePresenceSensor.md) | `HLK_LD2412_PresenceRead` | ? |
| `HLK_LD2412_UART` | UART | [`LD2412RadarSensor`](sensors/LD2412RadarSensor.md) | `HLK_LD2412_UART_BasicRead` | ? |
| `HLK_LD2420` | UART | [`LD2420RadarSensor`](sensors/LD2420RadarSensor.md) | `HLK_LD2420_UART_BasicRead` | ? |
| `HLK_LD2420_UART` | UART | [`LD2420RadarSensor`](sensors/LD2420RadarSensor.md) | `HLK_LD2420_UART_BasicRead` | ? |
| `HLK_LD2450` | UART | [`LD2450RadarSensor`](sensors/LD2450RadarSensor.md) | `HLK_LD2461_UART_BasicRead` | ? |
| `HLK_LD2451` | UART | [`LD2450RadarSensor`](sensors/LD2450RadarSensor.md) | `HLK_LD2461_UART_BasicRead` | ? |
| `HLK_LD2461` | UART | [`LD2461RadarSensor`](sensors/LD2461RadarSensor.md) | `HLK_LD2461_UART_BasicRead` | ? |
| `HLK_LD2461_UART` | UART | [`LD2461RadarSensor`](sensors/LD2461RadarSensor.md) | `HLK_LD2461_UART_BasicRead` | ? |
| `HY_SRF05` | GPIO | [`PulseUltrasonicSensor`](sensors/PulseUltrasonicSensor.md) | `HC_SR04_BasicRead` | ? |
| `HighPrecisionLaserRangingModule` | UART | [`NoopLoopToFSensor`](sensors/NoopLoopToFSensor.md) | `TOFSenseF2P_UART_BasicRead` | ? |
| `IR_ObstacleAvoidance` | GPIO | [`ReflectiveIrSensor`](sensors/ReflectiveIrSensor.md) | `TCRT5000_ReflectiveRead` | ? |
| `IR_Photoelectric_Switch` | GPIO | [`DigitalIrBarrierSensor`](sensors/DigitalIrBarrierSensor.md) | `E18_D80NK_BasicRead` | ? |
| `JSN_SR04T` | GPIO | [`PulseUltrasonicSensor`](sensors/PulseUltrasonicSensor.md) | `JSN_SR04T_BasicRead` | ? |
| `LM393_IR` | GPIO | [`ReflectiveIrSensor`](sensors/ReflectiveIrSensor.md) | `TCRT5000_ReflectiveRead` | ? |
| `MA312` | GPIO | [`PirMotionSensor`](sensors/PirMotionSensor.md) | `HC_SR501_MotionRead` | ? |
| `MB10XXAnalog` | Analog | [`MaxBotixAnalogSensor`](sensors/MaxBotixAnalogSensor.md) | `MB10XXAnalog_BasicRead` | ? |
| `MB7XXXAnalog` | Analog | [`MaxBotixAnalogSensor`](sensors/MaxBotixAnalogSensor.md) | `MB10XXAnalog_BasicRead` | ? |
| `ME007YS` | UART | [`A02YYUWSensor`](sensors/A02YYUWSensor.md) | `DYP_A01_BasicRead` | ? |
| `MR24HPC1` | UART | [`OpenProtocolMmwaveSensor`](sensors/OpenProtocolMmwaveSensor.md) | `OpenProtocol_Mmwave_BasicRead` | ? |
| `MultiSensor_QuickRead` | GPIO | [`PulseUltrasonicSensor`](sensors/PulseUltrasonicSensor.md) | `MultiSensor_QuickRead` | ? |
| `Platform_MultiRead` | GPIO | [`PulseUltrasonicSensor`](sensors/PulseUltrasonicSensor.md) | `Platform_MultiRead` | ? |
| `RCWL1601` | GPIO | [`PulseUltrasonicSensor`](sensors/PulseUltrasonicSensor.md) | `HC_SR04_BasicRead` | ? |
| `RCWL1655` | GPIO | [`PulseUltrasonicSensor`](sensors/PulseUltrasonicSensor.md) | `HC_SR04_BasicRead` | ? |
| `RCWL1670` | GPIO | [`PulseUltrasonicSensor`](sensors/PulseUltrasonicSensor.md) | `HC_SR04_BasicRead` | ? |
| `SRF02` | I2C | [`GyUs42I2cSensor`](sensors/GyUs42I2cSensor.md) | `GY_US42_I2C_BasicRead` | ? |
| `SRF05` | GPIO | [`PulseUltrasonicSensor`](sensors/PulseUltrasonicSensor.md) | `HC_SR04_BasicRead` | ? |
| `TCRT5000` | GPIO | [`ReflectiveIrSensor`](sensors/ReflectiveIrSensor.md) | `TCRT5000_ReflectiveRead` | ? |
| `TF03_CAN` | CAN | [`TF03CanSensor`](sensors/TF03CanSensor.md) | `Benewake_TF03_CAN_MCP2515_ESP32` | `Benewake_TF03_CAN_MCP2515_UnoR4`, `Benewake_TF03_CAN_SN65HVD230_ESP32` |
| `TFLuna` | UART | [`TFMiniSensor`](sensors/TFMiniSensor.md) | `TFLuna_BasicRead` | ? |
| `TFMini` | UART | [`TFMiniSensor`](sensors/TFMiniSensor.md) | `TFLuna_BasicRead` | ? |
| `TOF050C` | I2C | [`VL6180XSensor`](sensors/VL6180XSensor.md) | `VL6180X_BasicRead` | ? |
| `TOF050F` | I2C | [`VL6180XSensor`](sensors/VL6180XSensor.md) | `VL6180X_BasicRead` | ? |
| `TOF200C` | I2C | [`VL53L0XSensor`](sensors/VL53L0XSensor.md) | `VL53L0X_BasicRead` | ? |
| `TOF200F` | I2C | [`VL53L0XSensor`](sensors/VL53L0XSensor.md) | `VL53L0X_BasicRead` | ? |
| `TOF200F_Modbus` | RS485 | [`ModbusDistanceSensor`](sensors/ModbusDistanceSensor.md) | `WT53R485_BasicRead` | ? |
| `TOF400C` | I2C | [`VL53L1XSensor`](sensors/VL53L1XSensor.md) | `VL53L1X_BasicRead` | ? |
| `TOF400C_SPI` | SPI | [`VL53L1XSpiSensor`](sensors/VL53L1XSpiSensor.md) | `VL53L1X_SPI` | ? |
| `TOFSenseF2P` | I2C | [`ToFSenseFI2CSensor`](sensors/ToFSenseFI2CSensor.md) | `TOFSenseF2P_I2C_BasicRead` | ? |
| `TOFSenseF2P_I2C` | I2C | [`ToFSenseFI2CSensor`](sensors/ToFSenseFI2CSensor.md) | `TOFSenseF2P_I2C_BasicRead` | ? |
| `TOFSenseF2P_UART` | UART | [`NoopLoopToFSensor`](sensors/NoopLoopToFSensor.md) | `TOFSenseF2P_UART_BasicRead` | ? |
| `TW10SUART` | UART | [`AsciiUartRangeSensor`](sensors/AsciiUartRangeSensor.md) | `TW10S_UART_BasicRead` | ? |
| `TW10S_UART` | UART | [`AsciiUartRangeSensor`](sensors/AsciiUartRangeSensor.md) | `TW10S_UART_BasicRead` | ? |
| `US015` | UART | [`US100Sensor`](sensors/US100Sensor.md) | `US100_BasicRead` | ? |
| `US100` | UART | [`US100Sensor`](sensors/US100Sensor.md) | `US100_TemperatureRead` | ? |
| `VL53L0X` | I2C | [`VL53L0XSensor`](sensors/VL53L0XSensor.md) | `VL53L0X_ContinuousRead` | ? |
| `VL53L1CB` | I2C | [`VL53L1XSensor`](sensors/VL53L1XSensor.md) | `VL53L1X_BasicRead` | ? |
| `VL53L1CB_SPI` | SPI | [`VL53L1XSpiSensor`](sensors/VL53L1XSpiSensor.md) | `VL53L1X_SPI` | ? |
| `VL53L1X` | I2C | [`VL53L1XSensor`](sensors/VL53L1XSensor.md) | `VL53L1X_DualSensorRead` | ? |
| `VL53L1X_SPI` | SPI | [`VL53L1XSpiSensor`](sensors/VL53L1XSpiSensor.md) | `VL53L1X_SpiBurstRead` | ? |
| `VL53L4CD` | I2C | [`VL53L4CDSensor`](sensors/VL53L4CDSensor.md) | `VL53L4CD_DualSensorRead` | ? |
| `VL53L4CX` | I2C | [`VL53L4CXSensor`](sensors/VL53L4CXSensor.md) | `VL53L4CX_BasicRead` | ? |
| `VL6180X` | I2C | [`VL6180XSensor`](sensors/VL6180XSensor.md) | `VL6180X_AmbientRead` | ? |
| `WT53BRC_TTL` | UART | [`NoopLoopToFSensor`](sensors/NoopLoopToFSensor.md) | `TOFSenseF2P_UART_BasicRead` | ? |
| `WT53R485` | RS485 | [`ModbusDistanceSensor`](sensors/ModbusDistanceSensor.md) | `WT53R485_BasicRead` | ? |
| `WT53R_485` | RS485 | [`ModbusDistanceSensor`](sensors/ModbusDistanceSensor.md) | `WT53R485_BasicRead` | ? |
| `WT53R_TTL` | UART | [`NoopLoopToFSensor`](sensors/NoopLoopToFSensor.md) | `TOFSenseF2P_UART_BasicRead` | ? |
| `WWZMDiB_IR` | GPIO | [`ReflectiveIrSensor`](sensors/ReflectiveIrSensor.md) | `TCRT5000_ReflectiveRead` | ? |

See [API_INDEX.md](API_INDEX.md) for global helpers.
