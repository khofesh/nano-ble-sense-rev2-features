#pragma once

#include <Arduino.h>

// Orientation types
enum Orientation {
  ORIENTATION_UNKNOWN = 0,
  ORIENTATION_FACE_UP,
  ORIENTATION_FACE_DOWN,
  ORIENTATION_PORTRAIT,
  ORIENTATION_PORTRAIT_UPSIDE_DOWN,
  ORIENTATION_LANDSCAPE_LEFT,
  ORIENTATION_LANDSCAPE_RIGHT
};

void updateOrientation();
Orientation getOrientation();
const char* getOrientationString();
