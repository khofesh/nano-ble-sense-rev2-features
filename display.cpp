#include "display.h"
#include "sensor_data.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// I2C mutex flag to prevent concurrent access
static volatile bool i2cBusy = false;

void initDisplay() {
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }

  // clear the buffer
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Display Ready"));
  display.display();
  delay(1000);
}

void updateDisplay(int mode) {
  // acquire I2C mutex
  noInterrupts();
  if (i2cBusy) {
    interrupts();
    // skip this update if I2C is busy
    return; 
  }
  i2cBusy = true;
  interrupts();
  
  // create local snapshot of sensor data to avoid race conditions
  SensorData localData;
  copySensorDataAtomic(&localData);
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  switch (mode) {
    case 0:  // Temperature mode
      display.println(F("TEMP"));
      display.println();
      display.print(localData.temperature, 1);
      display.println(F(" C"));
      break;

    case 1:  // Humidity mode
      display.println(F("HUMIDITY"));
      display.println();
      display.print(localData.humidity, 1);
      display.println(F(" %"));
      break;

    case 2:  // Proximity mode
      display.println(F("PROXIMITY"));
      display.println();
      display.print(localData.proximity);
      break;

    case 3:  // gesture mode
      display.println(F("GESTURE"));
      display.println();
      display.print(localData.gesture);
      break;

    case 4:
      {  // color mode
        char rgba[50];
        snprintf(rgba, sizeof(rgba),
                 "r:%d g:%d\nb:%d a:%d",
                 localData.r, localData.g, localData.b, localData.a);
        display.println(F("COLOR"));
        display.println();
        display.print(rgba);
        break;
      }

    case 5:
      display.println(F("PRESSURE"));
      display.println();
      display.print(localData.pressure, 1);
      display.println(F(" kPa"));
      display.println();
      display.print(localData.altitude, 1);
      display.println(F(" m"));
      break;

    case 6: 
      display.println(F("ACCEL (g)"));
      display.println();
      display.print(F("X: "));
      display.println(localData.accelX, 2);
      display.print(F("Y: "));
      display.println(localData.accelY, 2);
      display.print(F("Z: "));
      display.println(localData.accelZ, 2);
      break;

    case 7:
      display.println(F("GYRO (dps)"));
      display.println();
      display.print(F("X: "));
      display.println(localData.gyroX, 2);
      display.print(F("Y: "));
      display.println(localData.gyroY, 2);
      display.print(F("Z: "));
      display.println(localData.gyroZ, 2);
      break;

    case 8:
      display.println(F("MAG (uT)"));
      display.println();
      display.print(F("X: "));
      display.println(localData.magX, 2);
      display.print(F("Y: "));
      display.println(localData.magY, 2);
      display.print(F("Z: "));
      display.println(localData.magZ, 2);
      break;
  }

  display.display();
  
  // release I2C mutex
  noInterrupts();
  i2cBusy = false;
  interrupts();
}
