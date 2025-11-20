#include "power_management.h"
#include "Arduino_LPS22HB.h"
#include "Arduino_APDS9960.h"
#include <PDM.h>
#include <mbed.h>

using namespace mbed;
using namespace rtos;

// Light sleep - uses mbed RTOS sleep
void enterLightSleep(uint32_t durationMs) {
  // This allows the CPU to enter low-power mode while maintaining peripherals
  ThisThread::sleep_for(std::chrono::milliseconds(durationMs));
}

void sleepUnusedSensors(int currentMode) {
  // Pressure sensor can be put to sleep when not in use
  if (currentMode != 0) {
    BARO.end();
  }
  
  // APDS9960 has lower power modes
  if (currentMode != 1 && currentMode != 2 && currentMode != 3) {
    // not using proximity, gesture, or color
    // APDS.end();  
  }
  
  // PDM microphone consumes power when running
  if (currentMode != 7) {
    // PDM.end();
  }
}

void wakeAllSensors() {
  BARO.begin();
  // APDS.begin();
  // PDM.begin(1, 16000);
}
