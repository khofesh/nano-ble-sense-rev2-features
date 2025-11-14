/*
- apds9960
- bmi270 + bmm150
- hs3003
- lps22hb
- microphone
*/
#pragma once

#include <Arduino.h>

void initSensors();
void updateSensors();

float getTemperature();
float getHumidity();
int getProximity();
int getGesture();
