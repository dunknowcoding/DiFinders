/*
 * Library_CompileSmoke.ino
 * Link-only smoke test: declares one object per public driver (no hardware).
 * Pins use DF_* defaults from DiFindersConstants.h.
 */

#include <DiFinders.h>

#define MY_TRIG DF_PIN_TRIG
#define MY_ECHO DF_PIN_ECHO
#define MY_ANA DF_PIN_ANA
#define MY_DIG DF_PIN_DIG
#define MY_PIR DF_PIN_PIR
#define MY_MW DF_PIN_MW

HC_SR04 distanceSensor(MY_TRIG, MY_ECHO);
AJ_SR04M pulseSensorA(MY_TRIG, MY_ECHO);
RCWL1655 pulseSensorB(MY_TRIG, MY_ECHO);
RCWL1670 pulseSensorC(MY_TRIG, MY_ECHO);
GP2Y0A21 irSensor(MY_ANA, df_adc(DF_ADC_REF), df_adc());
GP2Y0D810 windowSensor(MY_DIG);
TCRT5000 lineSensor(DF_PIN_LINE_DIG, DF_PIN_LINE_ANA, DF_PIN_LINE_EMIT);
HC_SR505 pirSensor(MY_PIR);
MB10XXAnalog maxbotixSensor(MY_ANA, df_adc(DF_ADC_REF), df_adc());
TFMini laserUart;
WT53R_TTL noopTtl;
WT53R_485 modbusSensor;
TW10S_UART asciiLaser;
TOFSenseF2P_I2C tofI2c;
CMOSLaserDistanceSensor cmosLaser;
HLK_LD2410 mmWaveOut(MY_MW);
HLK_LD2410_UART mmWaveUart;
HLK_LD2411_UART mmWave2411;
HLK_LD2420_UART mmWave2420;
HLK_LD2461_UART mmWave2461;
HLK_LD2412_UART mmWave2412;
HLK_LD2412_OUT mmWave2412Out;
HLK_LD2410S_UART mmWave2410s;
HLK_LD2410S_OUT mmWave2410sOut;
GY_US42 i2cSonar;
GY_US42_UART uartSonar;
GY_US42_Pulse pulseSonar(MY_TRIG, MY_ECHO);
TOFSenseF2P_UART tofUart;
TOF050F tof050fBoard;
TOF200F tof200fBoard;
GP2Y0E03 sharpI2c;
E18_D80NK barrier(MY_DIG);
DYP_A01 dypUart;
HY_SRF05 srf05(MY_TRIG, MY_ECHO);
HLK_LD2450 trackRadar;
HLK_LD2451 trackRadarB;
DFRobot_C4001 openRadar;
VL53L0X tof0;
TOF200C tof200cAlias;
VL53L1X tof1;
VL53L1X_SPI tof1Spi;
TOF400C tof400cAlias;
TOF400C_SPI tof400cSpiAlias;
VL53L4CD tof4cd;
VL53L4CX tof4cx;
VL6180X tof6180;
TOF050C tof050cAlias;

void setup() {
  lineSensor.setCalibrationWindow(50, 900);
}

void loop() {
  (void)distanceSensor.probe();
  (void)pulseSensorA.probe();
  (void)pulseSensorB.probe();
  (void)pulseSensorC.probe();
  (void)irSensor.probe();
  (void)windowSensor.probe();
  (void)lineSensor.probe();
  (void)pirSensor.probe();
  (void)maxbotixSensor.probe();
  (void)laserUart.probe();
  (void)noopTtl.probe();
  (void)modbusSensor.probe();
  (void)asciiLaser.probe();
  (void)tofI2c.probe();
  (void)cmosLaser.probe();
  (void)mmWaveOut.probe();
  (void)mmWaveUart.probe();
  (void)mmWave2411.probe();
  (void)mmWave2420.probe();
  (void)mmWave2461.probe();
  (void)mmWave2412.probe();
  (void)mmWave2412Out.probe();
  (void)mmWave2410s.probe();
  (void)mmWave2410sOut.probe();
  (void)i2cSonar.probe();
  (void)uartSonar.probe();
  (void)pulseSonar.probe();
  (void)tofUart.probe();
  (void)tof050fBoard.probe();
  (void)tof200fBoard.probe();
  (void)sharpI2c.probe();
  (void)barrier.probe();
  (void)dypUart.probe();
  (void)srf05.probe();
  (void)trackRadar.probe();
  (void)trackRadarB.probe();
  (void)openRadar.probe();
  (void)tof0.probe();
  (void)tof200cAlias.probe();
  (void)tof1.probe();
  (void)tof1Spi.probe();
  (void)tof400cSpiAlias.probe();
  (void)tof400cAlias.probe();
  (void)tof4cd.probe();
  (void)tof4cx.probe();
  (void)tof6180.probe();
  (void)tof050cAlias.probe();
  delay(1000);
}
