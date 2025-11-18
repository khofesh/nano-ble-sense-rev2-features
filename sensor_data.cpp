#include "sensor_data.h"
#include <Arduino.h>
#include <cstring>

SensorData sensorData = {
  0.0, 0.0, 0, 0, "NONE", 
  0, 0, 0, 0,
  0.0, 0.0,
  0.0, 0.0, 0.0,
  0.0, 0.0, 0.0,
  0.0, 0.0, 0.0
};

// atomic copy of sensor data to prevent race conditions
void copySensorDataAtomic(SensorData* dest) {
  noInterrupts();
  memcpy(dest, &sensorData, sizeof(SensorData));
  interrupts();
}

// atomic read of current mode from main file
extern volatile int currentMode;
int getCurrentModeAtomic() {
  noInterrupts();
  int mode = currentMode;
  interrupts();
  return mode;
}
