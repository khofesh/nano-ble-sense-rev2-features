/*
https://docs.arduino.cc/tutorials/nano-33-ble-sense-rev2/microphone-sensor/
*/

#include "microphone.h"
#include "sensor_data.h"

// buffer to read samples into, each sample is 16-bits
short sampleBuffer[256];

// number of samples read
volatile int samplesRead = 0;

// callback for PDM data
void onPDMdata() {
  // query the number of bytes available
  int bytesAvailable = PDM.available();

  // read into the sample buffer
  PDM.read(sampleBuffer, bytesAvailable);

  // 16-bit, 2 bytes per sample
  samplesRead = bytesAvailable / 2;
}

void initMicrophone() {
  // configure the data receive callback
  PDM.onReceive(onPDMdata);

  // optionally set the gain, defaults to 20
  // PDM.setGain(30);

  // initialize PDM with:
  // - one channel (mono mode)
  // - a 16 kHz sample rate
  if (!PDM.begin(1, 16000)) {
    Serial.println("Failed to start PDM!");
    while (1);
  }
  
  Serial.println("Microphone initialized");
}

void updateMicrophone() {
  // wait for samples to be read
  if (samplesRead) {
    // calculate average and peak from samples
    long sum = 0;
    int peak = 0;
    
    for (int i = 0; i < samplesRead; i++) {
      int absValue = abs(sampleBuffer[i]);
      sum += absValue;
      if (absValue > peak) {
        peak = absValue;
      }
    }
    
    int average = samplesRead > 0 ? sum / samplesRead : 0;
    
    // update sensor data atomically
    noInterrupts();
    sensorData.micLevel = average;
    sensorData.micPeak = peak;
    interrupts();
    
    // clear the read count
    samplesRead = 0;
  }
}

int getMicrophoneLevel() {
  updateMicrophone();
  return sensorData.micLevel;
}
