#include "BLEProperty.h"
#include "BLECharacteristic.h"
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

// color (RGBA) - 16 bytes (4 ints)
BLECharacteristic colorChar("1d3aa889-eb6f-4ff0-8321-1fd97e10f448",
                            BLERead | BLENotify, 16);
// accell
BLECharacteristic accelChar("75b6fecb-31d7-425d-8ee4-4d85701f6843",
                            BLERead | BLENotify, 12);
// gyro
BLECharacteristic gyroChar("b6c3897b-e7bb-473e-afcf-c322357dd7ee",
                           BLERead | BLENotify, 12);
// magnetometer
BLECharacteristic magChar("49f371bf-bc07-4047-a5df-56a306476493",
                          BLERead | BLENotify, 12);
// microphone (level + peak) - 8 bytes (2 ints)
BLECharacteristic micChar("8c4e2f63-0e5d-4c89-a19e-6b9c3d8f2a47",
                          BLERead | BLENotify, 8);

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
  sensorService.addCharacteristic(colorChar);
  sensorService.addCharacteristic(accelChar);
  sensorService.addCharacteristic(gyroChar);
  sensorService.addCharacteristic(magChar);
  sensorService.addCharacteristic(micChar);

  BLE.addService(sensorService);

  // set initial values
  float envData[3] = { 0.0f, 0.0f, 0.0f };
  envChar.writeValue((uint8_t*)envData, 12);
  proxChar.writeValue(0);
  gestureChar.writeValue((uint8_t*)"NONE", 4);
  modeChar.writeValue(0);
  int colorData[4] = { 0, 0, 0, 0 };
  colorChar.writeValue((uint8_t*)colorData, 16);
  float accelData[3] = {0.0f, 0.0f, 0.0f};
  accelChar.writeValue((uint8_t*)accelData, 12);
  float gyroData[3] = {0.0f, 0.0f, 0.0f};
  gyroChar.writeValue((uint8_t*)gyroData, 12);
  float magData[3] = {0.0f, 0.0f, 0.0f};
  magChar.writeValue((uint8_t*)magData, 12);
  int micData[2] = {0, 0};
  micChar.writeValue((uint8_t*)micData, 8);

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

    case 3:  // Color (RGBA)
      {
        int colorData[4] = {
          localData.r,
          localData.g,
          localData.b,
          localData.a
        };
        colorChar.writeValue((uint8_t*)colorData, 16);
      }
      break;

    case 4:  // accelerometer
      {
        float accelData[3] = {
          localData.accelX,
          localData.accelY,
          localData.accelZ
        };
        accelChar.writeValue((uint8_t*)accelData, 12);
      }
      break;

    case 5:  //  gyro
      {
        float gyroData[3] = {
          localData.gyroX,
          localData.gyroY,
          localData.gyroZ
        };
        gyroChar.writeValue((uint8_t*)gyroData, 12);
      }
      break;

    case 6: // magnetometer
      {
        float magData[3] = {
          localData.magX,
          localData.magY,
          localData.magZ
        };
        magChar.writeValue((uint8_t*)magData, 12);
      }
      break;

    case 7:  // Microphone (level + peak)
      {
        int micData[2] = {
          localData.micLevel,
          localData.micPeak
        };
        micChar.writeValue((uint8_t*)micData, 8);
      }
      break;

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

    // validate mode is within valid range (0-7)
    if (newMode >= 0 && newMode < 8) {
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
