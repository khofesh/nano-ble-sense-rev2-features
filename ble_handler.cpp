#include "ble_handler.h"
#include "sensor_data.h"

// BLE Service
BLEService sensorService("750e28e3-1b71-4f3b-85f6-732d98faa60a");

// BLE Characteristics (Read + Notify)
BLEFloatCharacteristic tempHumidPressChar("6e439deb-76a8-4ba2-a724-29a45466e1a9",
                                          BLERead | BLENotify);

void initBLE() {
  BLE.begin();

  // device name
  BLE.setLocalName("N33SenseRev2");
  BLE.setAdvertisedService(sensorService);

  // add characteristics to service
  sensorService.addCharacteristic(tempHumidPressChar);

  BLE.addService(sensorService);

  // advertising
  BLE.advertise();

  Serial.println("Peripheral device, waiting for connections...");
}

void updateBLE() {
}

void setModeFromBLE(int mode) {
}
