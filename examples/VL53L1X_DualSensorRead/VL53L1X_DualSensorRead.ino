/*
 * VL53L1X_DualSensorRead.ino — edit MY_* wiring aliases only.
 * Constants: DiFindersConstants.h (DF_* macros).
 */



#include <DiFinders.h>
#include <Wire.h>
#define MY_LEFT_XSHUT 2
#define MY_RIGHT_XSHUT 3
#define MY_LEFT_ADDR 0x30
#define MY_RIGHT_ADDR 0x31

VL53L1X tofSensorLeft;
VL53L1X tofSensorRight;

void stopBothSensors() {
  tofSensorLeft.powerOff();
  tofSensorRight.powerOff();
  delay(20);
}

bool startOneSensor(VL53L1X& sensor, uint8_t newAddress, uint32_t periodMs) {
  sensor.powerOn();
  delay(20);
  if (!sensor.begin(Wire, DF_ADDR_VL53L1X)) {
    return false;
  }
  if (!sensor.setAddress(newAddress)) {
    return false;
  }
  delay(5);
  sensor.useShortDistanceMode();
  sensor.setMeasurementTimingBudget(20000);
  sensor.startContinuous(periodMs);
  delay(10);
  return true;
}

void printSensorReading(const char* name, VL53L1X& sensor) {
  RangeReading reading = sensor.read(false);
  Serial.print(name);
  Serial.print(F("(mm): "));
  Serial.println(reading.distanceMm);
}

void setup() {
  df_serial();
  df_wire();
  tofSensorLeft.attachShutdownPin(MY_LEFT_XSHUT);
  tofSensorRight.attachShutdownPin(MY_RIGHT_XSHUT);
  stopBothSensors();
  if (!startOneSensor(tofSensorLeft, MY_LEFT_ADDR, 30)) {
    Serial.println(F("init failed"));
    while (1) { delay(500); }
  }
  if (!startOneSensor(tofSensorRight, MY_RIGHT_ADDR, 40)) {
    Serial.println(F("init failed"));
    while (1) { delay(500); }
  }
  Serial.println(F("msg"));
}

void loop() {
  int printedAny = 0;
  if (tofSensorLeft.dataReady()) {
    printSensorReading("L", tofSensorLeft);
    printedAny = 1;
  }
  delay(2);
  if (tofSensorRight.dataReady()) {
    printSensorReading("R", tofSensorRight);
    printedAny = 1;
  }
  if (printedAny == 0) {
    Serial.println(F("msg"));
  }
  delay(20);
}
