#include "orientation.h"
#include "sensor_data.h"
#include <math.h>

// Threshold for orientation detection (in g's)
#define ORIENTATION_THRESHOLD 0.7

// Current orientation
static Orientation currentOrientation = ORIENTATION_UNKNOWN;

// Hysteresis to prevent rapid switching
static unsigned long lastOrientationChange = 0;
#define ORIENTATION_DEBOUNCE_MS 500

void updateOrientation() {
  // Get accelerometer data from sensor data structure
  float x = sensorData.accelX;
  float y = sensorData.accelY;
  float z = sensorData.accelZ;
  
  Orientation newOrientation = ORIENTATION_UNKNOWN;
  
  // Determine orientation based on which axis has the strongest gravity component
  // Z-axis dominant (face up/down)
  if (fabs(z) > ORIENTATION_THRESHOLD) {
    if (z > 0) {
      newOrientation = ORIENTATION_FACE_UP;
    } else {
      newOrientation = ORIENTATION_FACE_DOWN;
    }
  }
  // Y-axis dominant (portrait orientations)
  else if (fabs(y) > ORIENTATION_THRESHOLD) {
    if (y > 0) {
      newOrientation = ORIENTATION_PORTRAIT;
    } else {
      newOrientation = ORIENTATION_PORTRAIT_UPSIDE_DOWN;
    }
  }
  // X-axis dominant (landscape orientations)
  else if (fabs(x) > ORIENTATION_THRESHOLD) {
    if (x > 0) {
      newOrientation = ORIENTATION_LANDSCAPE_RIGHT;
    } else {
      newOrientation = ORIENTATION_LANDSCAPE_LEFT;
    }
  }
  
  // Apply debouncing - only change if enough time has passed
  if (newOrientation != currentOrientation) {
    unsigned long currentTime = millis();
    if (currentTime - lastOrientationChange > ORIENTATION_DEBOUNCE_MS) {
      currentOrientation = newOrientation;
      lastOrientationChange = currentTime;
      
      // Print orientation changes
      Serial.print("Orientation changed to: ");
      Serial.println(getOrientationString());
    }
  }
}

Orientation getOrientation() {
  return currentOrientation;
}

const char* getOrientationString() {
  switch (currentOrientation) {
    case ORIENTATION_FACE_UP:
      return "FACE UP";
    case ORIENTATION_FACE_DOWN:
      return "FACE DOWN";
    case ORIENTATION_PORTRAIT:
      return "PORTRAIT";
    case ORIENTATION_PORTRAIT_UPSIDE_DOWN:
      return "PORTRAIT UD";
    case ORIENTATION_LANDSCAPE_LEFT:
      return "LANDSCAPE L";
    case ORIENTATION_LANDSCAPE_RIGHT:
      return "LANDSCAPE R";
    default:
      return "UNKNOWN";
  }
}
