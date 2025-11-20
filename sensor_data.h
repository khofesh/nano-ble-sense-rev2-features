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
    float gyroX;
    float gyroY;
    float gyroZ;
    /* BMM150 */
    float magX;
    float magY;
    float magZ;
    /* PDM Microphone */
    int micLevel;  // average sound level
    int micPeak;   // peak sound level
    /* Orientation (derived from accelerometer) */
    int orientation;  // 0-6: unknown, face-up, face-down, portrait, portrait-ud, landscape-l, landscape-r
};

extern SensorData sensorData; // defined in sensor_data.cpp

// thread-safe data access helpers
void copySensorDataAtomic(SensorData* dest);
int getCurrentModeAtomic();
