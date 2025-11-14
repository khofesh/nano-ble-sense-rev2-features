#include "sensors.h"
#include "display.h"
#include "ble_handler.h"
#include "sensor_data.h"

const int BUTTON_PIN = 2;
int currentMode = 0;
// temp == 1
// humidity == 2
// proximity == 3
const int NUM_MODES = 3; 

// sensor data
SensorData sensorData = {0};

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // init sensors, display, ble
  initSensors();

  Serial.println("system ready");
}

void loop() {
  static bool lastButtonState = HIGH;
  bool buttonState = digitalRead(BUTTON_PIN);

  if (buttonState == LOW && lastButtonState == HIGH) {
    currentMode = (currentMode + 1) % NUM_MODES;
    delay(200);
  }

  delay(100);
}
