#include <cstring>
#include "sensors.h"
#include "sensor_data.h"
#include "Arduino_APDS9960.h"
#include "Arduino_BMI270_BMM150.h"
#include "Arduino_HS300x.h"

void initSensors() {
  if (!APDS.begin()) {
    Serial.println("Error initializing APDS-9960 sensor.");
    return;
  }

  if (!IMU.begin()) {
    Serial.println("Failed to initialize BMI270_BMM150!");
    return;
  }

  if (!HS300x.begin()) {
    Serial.println("Failed to initialize HS3003 sensor!");
    return;
  }
}

void updateSensors() {
}

float getTemperature() {
  float data = HS300x.readTemperature();
  sensorData.temperature = data;
  return data;
}

float getHumidity() {
  float data = HS300x.readHumidity();
  sensorData.humidity = data;
  return data;
}

// APDS9960 get proximity
int getProximity() {
  if (APDS.proximityAvailable()) {
    int proximity = APDS.readProximity();
    sensorData.proximity = proximity;
    return proximity;
  }

  return 0;
}

int getGesture() {
  if (APDS.gestureAvailable()) {
    int gesture = APDS.readGesture();

    switch (gesture) {
      case GESTURE_UP:
        Serial.println("Detected UP gesture");
        strncpy(sensorData.gesture, "UP", sizeof(sensorData.gesture) - 1);
        sensorData.gesture[sizeof(sensorData.gesture) - 1] = '\0';
        break;

      case GESTURE_DOWN:
        Serial.println("Detected DOWN gesture");
        strncpy(sensorData.gesture, "DOWN", sizeof(sensorData.gesture) - 1);
        sensorData.gesture[sizeof(sensorData.gesture) - 1] = '\0';
        break;

      case GESTURE_LEFT:
        Serial.println("Detected LEFT gesture");
        strncpy(sensorData.gesture, "LEFT", sizeof(sensorData.gesture) - 1);
        sensorData.gesture[sizeof(sensorData.gesture) - 1] = '\0';
        break;

      case GESTURE_RIGHT:
        Serial.println("Detected RIGHT gesture");
        strncpy(sensorData.gesture, "RIGHT", sizeof(sensorData.gesture) - 1);
        sensorData.gesture[sizeof(sensorData.gesture) - 1] = '\0';
        break;

      default:
        strncpy(sensorData.gesture, "NONE", sizeof(sensorData.gesture) - 1);
        sensorData.gesture[sizeof(sensorData.gesture) - 1] = '\0';
        break;
    }

    return gesture;
  }

  return -1;
}

void getColor() {
  if (APDS.colorAvailable()) {
    int r, g, b, a;

    APDS.readColor(r, g, b, a);
    sensorData.r = r;
    sensorData.g = g;
    sensorData.b = b;
    sensorData.a = a;

    // set onboard LED to dominant color
    if (r > g && r > b) {
      digitalWrite(LEDR, LOW);
      digitalWrite(LEDG, HIGH);
      digitalWrite(LEDB, HIGH);
    } else if (g > r && g > b) {
      digitalWrite(LEDG, LOW);
      digitalWrite(LEDR, HIGH);
      digitalWrite(LEDB, HIGH);
    } else if (b > g && b > r) {
      digitalWrite(LEDB, LOW);
      digitalWrite(LEDR, HIGH);
      digitalWrite(LEDG, HIGH);
    } else {
      digitalWrite(LEDR, HIGH);
      digitalWrite(LEDG, HIGH);
      digitalWrite(LEDB, HIGH);
    }
  }
}

/*
TODO:
- APDS9960 readColor
*/