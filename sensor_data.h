#pragma once

struct SensorData {
    float temperature;
    float humidity;
    int proximity;
    unsigned long lastUpdate;
    char gesture[10];
    /* apds9960 color data */
    int r;
    int g;
    int b;
    int a;
};

extern SensorData sensorData; // defined in sensor_data.cpp
