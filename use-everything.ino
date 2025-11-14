#include "sensors.h"
#include "display.h"
#include "ble_handler.h"
#include "sensor_data.h"

const int BUTTON_PIN = 2;
volatile int currentMode = 0;
// temp == 0
// humidity == 1
// proximity == 2
// gesture == 3
// color == 4
const int NUM_MODES = 5;

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

  Serial.println("system ready");
}

void loop() {
  // read sensor data
  getTemperature();
  getHumidity();
  getProximity();
  getGesture();

  // update display with current mode
  updateDisplay(currentMode);
  
  delay(100);
}
