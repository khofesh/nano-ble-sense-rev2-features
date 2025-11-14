#pragma once

struct SensorData {
    float temperature;
    float humidity;
    int proximity;
    unsigned long lastUpdate;
    char* gesture;
};

extern SensorData sensorData; // defined in sensor_data.cpp
