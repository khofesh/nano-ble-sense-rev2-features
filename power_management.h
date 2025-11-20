#pragma once

#include <Arduino.h>

// sleep modes
void enterLightSleep(uint32_t durationMs);
void sleepUnusedSensors(int currentMode);
void wakeAllSensors();
