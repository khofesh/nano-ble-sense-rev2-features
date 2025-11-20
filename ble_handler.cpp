#include "ble_handler.h"
#include "sensor_data.h"

// BLE Service
BLEService sensorService("750e28e3-1b71-4f3b-85f6-732d98faa60a");

// BLE Characteristics (Read + Notify)
// Combined environmental characteristic (temp, humidity, pressure) - 12 bytes (3 floats)
BLECharacteristic envChar("6e439deb-76a8-4ba2-a724-29a45466e1a9",
                          BLERead | BLENotify, 12);
BLEIntCharacteristic proxChar("6e439deb-76a8-4ba2-a724-29a45466e1ac",
                              BLERead | BLENotify);
BLECharacteristic gestureChar("6e439deb-76a8-4ba2-a724-29a45466e1ad",
                              BLERead | BLENotify, 10);

// BLE Characteristic for mode control (Write)
BLEIntCharacteristic modeChar("6e439deb-76a8-4ba2-a724-29a45466e1ae",
                              BLERead | BLEWrite);

void initBLE() {
  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    return;
  }

  // device name
  BLE.setLocalName("N33SenseRev2");
  BLE.setAdvertisedService(sensorService);

  // add characteristics to service
  sensorService.addCharacteristic(envChar);
  sensorService.addCharacteristic(proxChar);
  sensorService.addCharacteristic(gestureChar);
  sensorService.addCharacteristic(modeChar);

  BLE.addService(sensorService);

  // set initial values
  float envData[3] = {0.0f, 0.0f, 0.0f};
  envChar.writeValue((uint8_t*)envData, 12);
  proxChar.writeValue(0);
  gestureChar.writeValue((uint8_t*)"NONE", 4);
  modeChar.writeValue(0);

  // advertising
  BLE.advertise();

  Serial.println("BLE Peripheral ready, waiting for connections...");
}

void updateBLE(int mode) {
  // check if a central device is connected
  BLEDevice central = BLE.central();

  if (!central || !central.connected()) {
    return;
  }

  // create local snapshot of sensor data to avoid race conditions
  SensorData localData;
  copySensorDataAtomic(&localData);

  // only notify for the current mode to reduce BLE traffic
  switch (mode) {
    case 0:  // Environment (temp + humidity + pressure)
      {
        float envData[3] = {
          localData.temperature,
          localData.humidity,
          localData.pressure
        };
        envChar.writeValue((uint8_t*)envData, 12);
      }
      break;

    case 1:  // Proximity
      proxChar.writeValue(localData.proximity);
      break;

    case 2:  // Gesture
      gestureChar.writeValue((uint8_t*)localData.gesture, strlen(localData.gesture));
      break;

    // TODO: other modes (color, accel, gyro, mag)
    default:
      break;
  }
}

void setModeFromBLE(int mode) {
  BLEDevice central = BLE.central();

  if (!central || !central.connected()) {
    return;
  }

  // check if mode characteristic was written to
  if (modeChar.written()) {
    int newMode = modeChar.value();

    // validate mode is within valid range (0-6)
    if (newMode >= 0 && newMode < 7) {
      // update the mode (extern variable from main sketch)
      extern volatile int currentMode;
      noInterrupts();
      currentMode = newMode;
      interrupts();

      Serial.print("Mode changed via BLE to: ");
      Serial.println(newMode);
    } else {
      Serial.print("Invalid mode received via BLE: ");
      Serial.println(newMode);
    }
  }
}
