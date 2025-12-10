#include <sys/_intsup.h>
#include <cstring>
#include "sensors.h"
#include "sensor_data.h"
#include "orientation.h"
#include "Arduino_APDS9960.h"
#include "Arduino_BMI270_BMM150.h"
#include "Arduino_HS300x.h"
#include "Arduino_LPS22HB.h"

void initSensors() {
  if (!APDS.begin()) {
    Serial.println("Error initializing APDS-9960 sensor.");
    while (1)
      ;
  }

  if (!IMU.begin()) {
    Serial.println("Failed to initialize BMI270_BMM150!");
    while (1)
      ;
  }

  if (!HS300x.begin()) {
    Serial.println("Failed to initialize HS3003 sensor!");
    while (1)
      ;
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
  static unsigned long lastColorRead = 0;
  unsigned long currentTime = millis();

  // Clear proximity/gesture data first to unblock color sensor
  if (APDS.proximityAvailable()) {
    APDS.readProximity();
  }
  if (APDS.gestureAvailable()) {
    APDS.readGesture();
  }

  // only read color every 500ms
  if (currentTime - lastColorRead >= 500) {
    if (APDS.colorAvailable()) {
      int r, g, b, a;

      APDS.readColor(r, g, b, a);
      sensorData.r = r;
      sensorData.g = g;
      sensorData.b = b;
      sensorData.a = a;

      // Imitate the actual color using RGB ratios and Clear intensity
      if (a > 10) {  // Only display if there's measurable light
        // Find the maximum RGB value to normalize ratios
        int maxRGB = max(max(r, g), b);
        
        if (maxRGB > 0) {
          // Map Clear intensity to LED brightness (0-255)
          // APDS9960 Clear channel typically ranges 0-10000+
          int brightness = constrain(map(a, 0, 5000, 0, 255), 0, 255);
          
          // Calculate each LED channel: maintain RGB ratios, scale by brightness
          // Invert for common-anode (255 = off, 0 = full on)
          int redLED = 255 - ((r * brightness) / maxRGB);
          int greenLED = 255 - ((g * brightness) / maxRGB);
          int blueLED = 255 - ((b * brightness) / maxRGB);
          
          analogWrite(LEDR, redLED);
          analogWrite(LEDG, greenLED);
          analogWrite(LEDB, blueLED);
        } else {
          // No color detected, turn off
          digitalWrite(LEDR, HIGH);
          digitalWrite(LEDG, HIGH);
          digitalWrite(LEDB, HIGH);
        }
      } else {
        // Too dark, turn off LED
        digitalWrite(LEDR, HIGH);
        digitalWrite(LEDG, HIGH);
        digitalWrite(LEDB, HIGH);
      }
    }
    lastColorRead = currentTime;
  }
}

void getPressure() {
  static unsigned long lastPressureRead = 0;
  unsigned long currentTime = millis();

  // read every 1 seconds
  if (currentTime - lastPressureRead >= 1000) {
    BARO.begin();  // wake up sensor
    delay(10);
    float pressure = BARO.readPressure();
    float altitude = 44330 * (1 - pow(pressure / 101.325, 1 / 5.255));
    sensorData.pressure = pressure;
    sensorData.altitude = altitude;
    BARO.end();  // put sensor to sleep
    lastPressureRead = currentTime;
  }
}

void getAccelerometer() {
  if (IMU.accelerationAvailable()) {
    float x, y, z;
    IMU.readAcceleration(x, y, z);
    sensorData.accelX = x;
    sensorData.accelY = y;
    sensorData.accelZ = z;

    // Update orientation based on accelerometer data
    updateOrientation();
    sensorData.orientation = (int)getOrientation();
  }
}

void getGyroscope() {
  if (IMU.gyroscopeAvailable()) {
    float x, y, z;
    IMU.readGyroscope(x, y, z);
    sensorData.gyroX = x;
    sensorData.gyroY = y;
    sensorData.gyroZ = z;
  }
}

void getMagnetometer() {
  if (IMU.magneticFieldAvailable()) {
    float x, y, z;
    IMU.readMagneticField(x, y, z);
    sensorData.magX = x;
    sensorData.magY = y;
    sensorData.magZ = z;
  }
}

/*
TODO:
# microphone
I don't know what to do with microphone

# BLE
advertise all the sensor data

*/