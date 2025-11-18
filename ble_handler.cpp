#include "ble_handler.h"
#include "sensor_data.h"

// BLE Service
BLEService sensorService("750e28e3-1b71-4f3b-85f6-732d98faa60a");

// BLE Characteristics (Read + Notify) - separate for each sensor type
BLEFloatCharacteristic tempChar("6e439deb-76a8-4ba2-a724-29a45466e1a9",
                                BLERead | BLENotify);
BLEFloatCharacteristic humidChar("6e439deb-76a8-4ba2-a724-29a45466e1aa",
                                 BLERead | BLENotify);
BLEFloatCharacteristic pressChar("6e439deb-76a8-4ba2-a724-29a45466e1ab",
                                 BLERead | BLENotify);
BLEIntCharacteristic proxChar("6e439deb-76a8-4ba2-a724-29a45466e1ac",
                              BLERead | BLENotify);
BLECharacteristic gestureChar("6e439deb-76a8-4ba2-a724-29a45466e1ad",
                              BLERead | BLENotify, 10);

void initBLE() {
  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    return;
  }

  // device name
  BLE.setLocalName("N33SenseRev2");
  BLE.setAdvertisedService(sensorService);

  // add characteristics to service
  sensorService.addCharacteristic(tempChar);
  sensorService.addCharacteristic(humidChar);
  sensorService.addCharacteristic(pressChar);
  sensorService.addCharacteristic(proxChar);
  sensorService.addCharacteristic(gestureChar);

  BLE.addService(sensorService);

  // set initial values
  tempChar.writeValue(0.0f);
  humidChar.writeValue(0.0f);
  pressChar.writeValue(0.0f);
  proxChar.writeValue(0);
  gestureChar.writeValue((uint8_t*)"NONE", 4);

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
    case 0: // Temperature
      tempChar.writeValue(localData.temperature);
      break;
      
    case 1: // Humidity
      humidChar.writeValue(localData.humidity);
      break;
      
    case 2: // Proximity
      proxChar.writeValue(localData.proximity);
      break;
      
    case 3: // Gesture
      gestureChar.writeValue((uint8_t*)localData.gesture, strlen(localData.gesture));
      break;
      
    case 5: // Pressure
      pressChar.writeValue(localData.pressure);
      break;
      
    // TODO: other modes (color, accel, gyro, mag)
    default:
      break;
  }
}

void setModeFromBLE(int mode) {
  // todo: allow BLE client to change mode
}
