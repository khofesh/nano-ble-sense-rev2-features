#include "sensors.h"
#include "display.h"
#include "ble_handler.h"
#include "sensor_data.h"

const int BUTTON_PIN = 2;
volatile int currentMode = 0;
// temp + humidity + pressure == 0
// proximity == 1
// gesture == 2
// color == 3
// accelerometer = 4
// gyroscope = 5
// magnetometer = 6
const int NUM_MODES = 7;

// debouncing variables
volatile unsigned long lastInterruptTime = 0;
const unsigned long debounceDelay = 200;

// interrupt service routine for button press
void buttonISR() {
  unsigned long interruptTime = millis();

  // debounce: ignore if interrupt triggered within debounceDelay
  if (interruptTime - lastInterruptTime > debounceDelay) {
    currentMode = (currentMode + 1) % NUM_MODES;
    lastInterruptTime = interruptTime;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // attach interrupt on falling edge (button press)
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);

  // init sensors, display, ble
  initSensors();
  initDisplay();
  initBLE();

  Serial.println("system ready");
}

void loop() {
  // atomically read current mode to prevent race condition with ISR
  int localMode;
  noInterrupts();
  localMode = currentMode;
  interrupts();
  
  // read sensor data based on local mode snapshot
  switch (localMode) {
    case 0:
      getTemperature();
      getHumidity();
      getPressure();
      break;
    case 1:
      getProximity();
      break;
    case 2:
      getGesture();
      break;
    case 3:
      getColor();
      break;
    case 4:
      getAccelerometer();
      break;
    case 5:
      getGyroscope();
      break;
    case 6:
      getMagnetometer();
      break;
  }

  // check for mode changes from BLE
  setModeFromBLE(localMode);

  // update BLE with sensor data
  updateBLE(localMode);

  // update display with current mode
  updateDisplay(localMode);

  delay(100);
}
