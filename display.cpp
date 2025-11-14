#include "display.h"
#include "sensor_data.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

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
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  switch (mode) {
    case 0:  // Temperature mode
      display.println(F("TEMP"));
      display.println();
      display.print(sensorData.temperature, 1);
      display.println(F(" C"));
      break;

    case 1:  // Humidity mode
      display.println(F("HUMIDITY"));
      display.println();
      display.print(sensorData.humidity, 1);
      display.println(F(" %"));
      break;

    case 2:  // Proximity mode
      display.println(F("PROXIMITY"));
      display.println();
      display.print(sensorData.proximity);
      break;

    case 3:  // gesture mode
      display.println(F("GESTURE"));
      display.println();
      display.print(sensorData.gesture);
      break;

    case 4:
      {  // color mode
        char rgba[50];
        snprintf(rgba, sizeof(rgba),
                 "r:%d g:%d\nb:%d a:%d",
                 sensorData.r, sensorData.g, sensorData.b, sensorData.a);
        display.println(F("COLOR"));
        display.println();
        display.print(rgba);
        break;
      }

    case 5:
      display.println(F("PRESSURE"));
      display.println();
      display.print(sensorData.pressure, 1);
      display.println(F(" kPa"));
  }

  display.display();
}
