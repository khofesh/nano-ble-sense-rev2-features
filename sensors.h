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

/* hs3003 */
float getTemperature();
float getHumidity();

/* apds9960 */
int getProximity();
int getGesture();
void getColor();
