#pragma once

#include "core/DiFindersConfig.h"
#include "core/DiFindersConstants.h"
#include "core/DiFindersDebug.h"
#include "core/DiFindersPlatform.h"
#include "core/DiFindersBoard.h"
#include "core/DiFindersDma.h"
#include "core/DiFindersUart.h"
#include "core/DiFindersCan.h"
#include "core/DiFindersDiagnostics.h"
#include "core/DiFindersTypes.h"
#include "families/infrared/DigitalIrBarrierSensor.h"
#include "families/infrared/Gp2Y0E03Sensor.h"
#include "families/infrared/ReflectiveIrSensor.h"
#include "families/infrared/SharpAnalogDistanceSensor.h"
#include "families/infrared/SharpDigitalDistanceSensor.h"
#include "families/motion/LD2412RadarSensor.h"
#include "families/motion/LD2461RadarSensor.h"
#include "families/motion/LD2420RadarSensor.h"
#include "families/motion/LD2411RadarSensor.h"
#include "families/motion/LD2410RadarSensor.h"
#include "families/motion/LD2450RadarSensor.h"
#include "families/motion/MmwavePresenceSensor.h"
#include "families/motion/OpenProtocolMmwaveSensor.h"
#include "families/motion/PirMotionSensor.h"
#include "families/tof/AsciiUartRangeSensor.h"
#include "families/tof/VL53L0XSensor.h"
#include "families/tof/VL53L1XSensor.h"
#include "families/tof/VL53L1XSpiSensor.h"
#include "families/tof/VL53L4CDSensor.h"
#include "families/tof/VL53L4CXSensor.h"
#include "families/tof/VL6180XSensor.h"
#include "families/tof/ModbusDistanceSensor.h"
#include "families/tof/NoopLoopToFSensor.h"
#include "families/tof/ToFSenseFI2CSensor.h"
#include "families/ultrasonic/A02YYUWSensor.h"
#include "families/ultrasonic/GyUs42I2cSensor.h"
#include "families/ultrasonic/GyUs42PulseSensor.h"
#include "families/ultrasonic/MaxBotixAnalogSensor.h"
#include "families/ultrasonic/PulseUltrasonicSensor.h"
#include "families/ultrasonic/TFMiniSensor.h"
#include "families/ultrasonic/TF03CanSensor.h"
#include "families/ultrasonic/US100Sensor.h"

namespace DiFinders {

using Ultrasonic = PulseUltrasonicSensor;
using Reflective = ReflectiveIrSensor;
using Motion = PirMotionSensor;
using Presence = MmwavePresenceSensor;
using MmwaveOpenProtocol = OpenProtocolMmwaveSensor;
using ToF = VL53L0XSensor;

class HC_SR04 : public PulseUltrasonicSensor {
 public:
  HC_SR04() = default;
  HC_SR04(uint8_t triggerPin, uint8_t echoPin, uint8_t echoPinMode = INPUT)
	  : PulseUltrasonicSensor(triggerPin, echoPin, echoPinMode) {}
};

class RCWL1601 : public PulseUltrasonicSensor {
 public:
  RCWL1601() = default;
  RCWL1601(uint8_t triggerPin, uint8_t echoPin, uint8_t echoPinMode = INPUT)
	  : PulseUltrasonicSensor(triggerPin, echoPin, echoPinMode) {}
};

class JSN_SR04T : public PulseUltrasonicSensor {
 public:
  JSN_SR04T() = default;
  JSN_SR04T(uint8_t triggerPin, uint8_t echoPin, uint8_t echoPinMode = INPUT)
	  : PulseUltrasonicSensor(triggerPin, echoPin, echoPinMode) {}
};

class AJ_SR04M : public PulseUltrasonicSensor {
 public:
  AJ_SR04M() = default;
  AJ_SR04M(uint8_t triggerPin, uint8_t echoPin, uint8_t echoPinMode = INPUT)
	  : PulseUltrasonicSensor(triggerPin, echoPin, echoPinMode) {}
};

class RCWL1655 : public PulseUltrasonicSensor {
 public:
  RCWL1655() = default;
  RCWL1655(uint8_t triggerPin, uint8_t echoPin, uint8_t echoPinMode = INPUT)
	  : PulseUltrasonicSensor(triggerPin, echoPin, echoPinMode) {}
};

class RCWL1670 : public PulseUltrasonicSensor {
 public:
  RCWL1670() = default;
  RCWL1670(uint8_t triggerPin, uint8_t echoPin, uint8_t echoPinMode = INPUT)
	  : PulseUltrasonicSensor(triggerPin, echoPin, echoPinMode) {}
};

class GP2Y0A21 : public SharpAnalogDistanceSensor {
 public:
  GP2Y0A21() = default;
  GP2Y0A21(uint8_t analogPin, uint16_t adcReferenceMv = 5000,
           uint16_t adcMax = 1023)
	  : SharpAnalogDistanceSensor(analogPin, SharpAnalogModel::GP2Y0A21YK0F, adcReferenceMv, adcMax) {}
};

class GP2Y0A41 : public SharpAnalogDistanceSensor {
 public:
  GP2Y0A41() = default;
  GP2Y0A41(uint8_t analogPin, uint16_t adcReferenceMv = 3300,
           uint16_t adcMax = 1023)
	  : SharpAnalogDistanceSensor(analogPin, SharpAnalogModel::GP2Y0A41SK0F, adcReferenceMv, adcMax) {}
};

class GP2Y0A51 : public SharpAnalogDistanceSensor {
 public:
  GP2Y0A51() = default;
  GP2Y0A51(uint8_t analogPin, uint16_t adcReferenceMv = 3300,
           uint16_t adcMax = 1023)
	  : SharpAnalogDistanceSensor(analogPin, SharpAnalogModel::GP2Y0A51SK0F, adcReferenceMv, adcMax) {}
};

class GP2Y0A02 : public SharpAnalogDistanceSensor {
 public:
  GP2Y0A02() = default;
  GP2Y0A02(uint8_t analogPin, uint16_t adcReferenceMv = 3300,
           uint16_t adcMax = 1023)
	  : SharpAnalogDistanceSensor(analogPin, SharpAnalogModel::GP2Y0A02YK0F, adcReferenceMv, adcMax) {}
};

class GP2Y0A60 : public SharpAnalogDistanceSensor {
 public:
  GP2Y0A60() = default;
  GP2Y0A60(uint8_t analogPin, uint16_t adcReferenceMv = 3300,
           uint16_t adcMax = 1023)
	  : SharpAnalogDistanceSensor(analogPin, SharpAnalogModel::GP2Y0A60SZLF, adcReferenceMv, adcMax) {}
};

class GP2Y0D805 : public SharpDigitalDistanceSensor {
 public:
  GP2Y0D805() = default;
  GP2Y0D805(uint8_t outputPin, bool activeLow = true)
	  : SharpDigitalDistanceSensor(outputPin, SharpDigitalModel::GP2Y0D805Z0F, activeLow) {}
};

class GP2Y0D810 : public SharpDigitalDistanceSensor {
 public:
  GP2Y0D810() = default;
  GP2Y0D810(uint8_t outputPin, bool activeLow = true)
	  : SharpDigitalDistanceSensor(outputPin, SharpDigitalModel::GP2Y0D810Z0F, activeLow) {}
};

class GP2Y0D815 : public SharpDigitalDistanceSensor {
 public:
  GP2Y0D815() = default;
  GP2Y0D815(uint8_t outputPin, bool activeLow = true)
	  : SharpDigitalDistanceSensor(outputPin, SharpDigitalModel::GP2Y0D815Z0F, activeLow) {}
};

class TCRT5000 : public ReflectiveIrSensor {
 public:
  TCRT5000() = default;
  TCRT5000(uint8_t digitalPin, uint8_t analogPin = 0xFF, uint8_t emitterPin = 0xFF)
	  : ReflectiveIrSensor(digitalPin, analogPin, emitterPin) {
	setProfile(ReflectiveIrProfile::Tcrt5000);
  }
};

class LM393_IR : public ReflectiveIrSensor {
 public:
  LM393_IR() = default;
  LM393_IR(uint8_t digitalPin, uint8_t analogPin = 0xFF, uint8_t emitterPin = 0xFF)
	  : ReflectiveIrSensor(digitalPin, analogPin, emitterPin) {
	setProfile(ReflectiveIrProfile::GenericObstacleLm393);
  }
};

class WWZMDiB_IR : public ReflectiveIrSensor {
 public:
  WWZMDiB_IR() = default;
  WWZMDiB_IR(uint8_t digitalPin, uint8_t analogPin = 0xFF, uint8_t emitterPin = 0xFF)
	  : ReflectiveIrSensor(digitalPin, analogPin, emitterPin) {
	setProfile(ReflectiveIrProfile::WwzmdibIr);
  }
};

class IR_ObstacleAvoidance : public ReflectiveIrSensor {
 public:
  IR_ObstacleAvoidance() = default;
  IR_ObstacleAvoidance(uint8_t digitalPin, uint8_t analogPin = 0xFF, uint8_t emitterPin = 0xFF)
	  : ReflectiveIrSensor(digitalPin, analogPin, emitterPin) {
	setProfile(ReflectiveIrProfile::GenericObstacleLm393);
  }
};

class HC_SR501 : public PirMotionSensor {
 public:
  HC_SR501() = default;
  HC_SR501(uint8_t pin, bool activeHigh = true, uint8_t inputMode = INPUT)
	  : PirMotionSensor(pin, PirModuleProfile::HC_SR501, activeHigh, inputMode) {}
};

class HC_SR505 : public PirMotionSensor {
 public:
  HC_SR505() = default;
  HC_SR505(uint8_t pin, bool activeHigh = true, uint8_t inputMode = INPUT)
	  : PirMotionSensor(pin, PirModuleProfile::HC_SR505, activeHigh, inputMode) {}
};

class AM312 : public PirMotionSensor {
 public:
  AM312() = default;
  AM312(uint8_t pin, bool activeHigh = true, uint8_t inputMode = INPUT)
	  : PirMotionSensor(pin, PirModuleProfile::AM312, activeHigh, inputMode) {}
};

using MA312 = AM312;

class US100 : public US100Sensor {
 public:
  US100() = default;
  explicit US100(Stream& serial) : US100Sensor(serial) {}
};

class A02YYUW : public A02YYUWSensor {
 public:
  A02YYUW() = default;
  explicit A02YYUW(Stream& serial) : A02YYUWSensor(serial) {}
};

using A01NYUB = A02YYUW;
using DYP_A01 = A02YYUW;
using DYP_A01A = A02YYUW;
using DYP_A01B = A02YYUW;
using ME007YS = A02YYUW;

class GY_US42 : public GyUs42I2cSensor {
 public:
  GY_US42() = default;
  explicit GY_US42(uint8_t address) : GyUs42I2cSensor(address) {}
};

using GY_US42V2 = GY_US42;
using SRF02 = GY_US42;

class GY_US42_UART : public A02YYUWSensor {
 public:
  GY_US42_UART() = default;
  explicit GY_US42_UART(Stream& serial) : A02YYUWSensor(serial) {}
};

class GY_US42_Pulse : public GyUs42PulseSensor {
 public:
  GY_US42_Pulse() = default;
  GY_US42_Pulse(uint8_t triggerPin, uint8_t echoPin, uint8_t echoPinMode = INPUT)
      : GyUs42PulseSensor(triggerPin, echoPin, echoPinMode) {}
};

class GP2Y0E03 : public Gp2Y0E03Sensor {
 public:
  GP2Y0E03() = default;
  explicit GP2Y0E03(uint8_t address) : Gp2Y0E03Sensor(address) {}
};

class E18_D80NK : public DigitalIrBarrierSensor {
 public:
  E18_D80NK() = default;
  explicit E18_D80NK(uint8_t outputPin) : DigitalIrBarrierSensor(outputPin, DigitalIrBarrierProfile::E18_D80NK) {}
};

using E18_D60NK = E18_D80NK;
using IR_Photoelectric_Switch = E18_D80NK;

class HY_SRF05 : public PulseUltrasonicSensor {
 public:
  HY_SRF05() = default;
  HY_SRF05(uint8_t triggerPin, uint8_t echoPin, uint8_t echoPinMode = INPUT)
      : PulseUltrasonicSensor(triggerPin, echoPin, echoPinMode) {}
};

using SRF05 = HY_SRF05;

class TFMini : public TFMiniSensor {
 public:
  TFMini() = default;
  explicit TFMini(Stream& serial) : TFMiniSensor(serial) {}
};

using TFLuna = TFMini;

class WT53R_TTL : public NoopLoopToFSensor {
 public:
  WT53R_TTL() = default;
  explicit WT53R_TTL(Stream& serial) : NoopLoopToFSensor(serial) {}
};

class WT53BRC_TTL : public NoopLoopToFSensor {
 public:
  WT53BRC_TTL() = default;
  explicit WT53BRC_TTL(Stream& serial) : NoopLoopToFSensor(serial) {}
};

class CMOSLaserDistanceSensor : public NoopLoopToFSensor {
 public:
  CMOSLaserDistanceSensor() = default;
  explicit CMOSLaserDistanceSensor(Stream& serial) : NoopLoopToFSensor(serial) {}
};

class HighPrecisionLaserRangingModule : public NoopLoopToFSensor {
 public:
  HighPrecisionLaserRangingModule() = default;
  explicit HighPrecisionLaserRangingModule(Stream& serial) : NoopLoopToFSensor(serial) {}
};

class WT53R_485 : public ModbusDistanceSensor {
 public:
  WT53R_485() = default;
  explicit WT53R_485(Stream& serial) : ModbusDistanceSensor(serial) {}
};

using WT53R485 = WT53R_485;

class TW10S_UART : public AsciiUartRangeSensor {
 public:
  TW10S_UART() {
    setPrefix("D");
    setUnitMillimeters(1.0f);
    setDefaultBaudRate(19200);
  }

  explicit TW10S_UART(Stream& serial) : AsciiUartRangeSensor(serial) {
    setPrefix("D");
    setUnitMillimeters(1.0f);
    setDefaultBaudRate(19200);
  }
};

using TW10SUART = TW10S_UART;

class MB10XXAnalog : public MaxBotixAnalogSensor {
 public:
  MB10XXAnalog() = default;
  MB10XXAnalog(uint8_t analogPin, uint16_t adcReferenceMv = 3300,
               uint16_t adcMax = 1023, uint16_t sensorSupplyMv = 0)
      : MaxBotixAnalogSensor(analogPin, adcReferenceMv, adcMax, sensorSupplyMv) {}
};

using MB7XXXAnalog = MB10XXAnalog;

class VL53L0X : public VL53L0XSensor {
 public:
  VL53L0X() = default;
  explicit VL53L0X(uint8_t address) : VL53L0XSensor(address) {}
};

class TOF200C : public VL53L0XSensor {
 public:
  TOF200C() = default;
  explicit TOF200C(uint8_t address) : VL53L0XSensor(address) {}
};

class VL53L1X : public VL53L1XSensor {
 public:
  VL53L1X() = default;
  explicit VL53L1X(uint8_t address) : VL53L1XSensor(address) {}
};

using VL53L1CB = VL53L1X;
using TOF400C = VL53L1X;

class VL53L1X_SPI : public VL53L1XSpiSensor {};
using VL53L1CB_SPI = VL53L1X_SPI;
using TOF400C_SPI = VL53L1X_SPI;

class VL53L4CD : public VL53L4CDSensor {
 public:
  VL53L4CD() = default;
  explicit VL53L4CD(uint8_t address) : VL53L4CDSensor(address) {}
};

class VL53L4CX : public VL53L4CXSensor {
 public:
  VL53L4CX() = default;
  explicit VL53L4CX(uint8_t address) : VL53L4CXSensor(address) {}
};

class VL6180X : public VL6180XSensor {
 public:
  VL6180X() = default;
  explicit VL6180X(uint8_t address) : VL6180XSensor(address) {}
};

class TOF050C : public VL6180XSensor {
 public:
  TOF050C() = default;
  explicit TOF050C(uint8_t address) : VL6180XSensor(address) {}
};

// Seller SKU TOF050F is often a VL6180-class clone; treat as alias until a distinct protocol is confirmed.
class TOF050F : public VL6180XSensor {
 public:
  TOF050F() = default;
  explicit TOF050F(uint8_t address) : VL6180XSensor(address) {}
};

class TOF200F : public VL53L0XSensor {
 public:
  TOF200F() = default;
  explicit TOF200F(uint8_t address) : VL53L0XSensor(address) {}
};

class TOF200F_Modbus : public ModbusDistanceSensor {
 public:
  TOF200F_Modbus() = default;
  explicit TOF200F_Modbus(Stream& serial) : ModbusDistanceSensor(serial) {}
};

class TOFSenseF2P_I2C : public ToFSenseFI2CSensor {
 public:
  TOFSenseF2P_I2C() = default;
  explicit TOFSenseF2P_I2C(uint8_t address) : ToFSenseFI2CSensor(address) {}
};

class TOFSenseF2P_UART : public NoopLoopToFSensor {
 public:
  TOFSenseF2P_UART() = default;
  explicit TOFSenseF2P_UART(Stream& serial) : NoopLoopToFSensor(serial) {}
};

using TOFSenseF2P = TOFSenseF2P_I2C;
using UltrasonicRadarRangingSensor = PulseUltrasonicSensor;

class HLK_LD2410_UART : public LD2410RadarSensor {
 public:
  HLK_LD2410_UART() = default;
  explicit HLK_LD2410_UART(Stream& serial) : LD2410RadarSensor(serial) {}
};

class HLK_LD2411_UART : public LD2411RadarSensor {
 public:
  HLK_LD2411_UART() = default;
  explicit HLK_LD2411_UART(Stream& serial) : LD2411RadarSensor(serial) {}
};

class HLK_LD2420_UART : public LD2420RadarSensor {
 public:
  HLK_LD2420_UART() = default;
  explicit HLK_LD2420_UART(Stream& serial) : LD2420RadarSensor(serial) {}
};

class HLK_LD2461_UART : public LD2461RadarSensor {
 public:
  HLK_LD2461_UART() = default;
  explicit HLK_LD2461_UART(Stream& serial) : LD2461RadarSensor(serial) {}
};

class HLK_LD2412_UART : public LD2412RadarSensor {
 public:
  HLK_LD2412_UART() = default;
  explicit HLK_LD2412_UART(Stream& serial) : LD2412RadarSensor(serial) {}
};

using HLK_LD2412 = HLK_LD2412_UART;

class HLK_LD2412_OUT : public MmwavePresenceSensor {
 public:
  HLK_LD2412_OUT() = default;
  explicit HLK_LD2412_OUT(uint8_t outPin, bool activeHigh = true) : MmwavePresenceSensor(outPin, activeHigh) {}
};

class HLK_LD2410S_UART : public LD2410RadarSensor {
 public:
  HLK_LD2410S_UART() = default;
  explicit HLK_LD2410S_UART(Stream& serial) : LD2410RadarSensor(serial) {}

  uint32_t defaultBaudRate() const {
    return 115200UL;
  }
};

using HLK_LD2410S = HLK_LD2410S_UART;

class HLK_LD2410S_OUT : public MmwavePresenceSensor {
 public:
  HLK_LD2410S_OUT() = default;
  explicit HLK_LD2410S_OUT(uint8_t outPin, bool activeHigh = true) : MmwavePresenceSensor(outPin, activeHigh) {}
};

using HLK_LD2411 = HLK_LD2411_UART;
using HLK_LD2411S = HLK_LD2411_UART;
using HLK_LD2411S_UART = HLK_LD2411_UART;
using HLK_LD2420 = HLK_LD2420_UART;
using HLK_LD2461 = HLK_LD2461_UART;

using HLK_LD2410B = HLK_LD2410_UART;
using HLK_LD2410C_UART = HLK_LD2410_UART;

class HLK_LD2410 : public MmwavePresenceSensor {
 public:
  HLK_LD2410() = default;
  explicit HLK_LD2410(uint8_t outPin, bool activeHigh = true) : MmwavePresenceSensor(outPin, activeHigh) {}
};

using HLK_LD2410C = HLK_LD2410;
using HLK_LD1125H = HLK_LD2410;
using HLK_LD1155H = HLK_LD2410;

class HLK_LD2450 : public LD2450RadarSensor {
 public:
  HLK_LD2450() = default;
  explicit HLK_LD2450(Stream& serial) : LD2450RadarSensor(serial) {}
};

using HLK_LD2451 = HLK_LD2450;
using HLK_LD2402 = HLK_LD2450;

class DFRobot_C4001 : public OpenProtocolMmwaveSensor {
 public:
  DFRobot_C4001() = default;
  explicit DFRobot_C4001(Stream& serial) : OpenProtocolMmwaveSensor(serial) {}
};

using DFRobotMmWaveC4001 = DFRobot_C4001;
using MR24HPC1 = OpenProtocolMmwaveSensor;
using Benewake_TF03 = TFMini;

class Benewake_TF03_CAN : public TF03CanSensor {
 public:
  Benewake_TF03_CAN() = default;
};

using TF03_CAN = Benewake_TF03_CAN;
using US015 = US100;

}  // namespace DiFinders

#include "core/DiFindersHelpers.h"
#include "DiFindersUserAPI.h"

/* Optional namespace alias for advanced users (sketches use global df_* API). */
namespace df = DiFinders;
