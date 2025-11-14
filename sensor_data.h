#pragma once

struct SensorData {
    /* HS300x */
    float temperature;
    float humidity;
    /* APDS9960 */
    int proximity;
    unsigned long lastUpdate;
    char gesture[10];
    /* apds9960 color data */
    int r;
    int g;
    int b;
    int a;
    /* LPS22HB */
    float pressure;
    float altitude;
    /* BMI270 */
    float accelX;
    float accelY;
    float accelZ;
};

extern SensorData sensorData; // defined in sensor_data.cpp
