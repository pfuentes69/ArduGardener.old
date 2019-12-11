#ifndef MonitorMeteo_h
#define MonitorMeteo_h

#include <Arduino.h>
#include <Adafruit_BMP085.h>
#include <AM2320.h>
#include "Debug.h"

#include "BoardSetup.h"

#define ERROR_TEMP 0.1

class MonitorMeteo
{
  // Class Member Variables
  Adafruit_BMP085 bmp;
  AM2320 th;
  long updatePeriod;
  public:
  float temperature;
  long humidity;
  long pressure;
  String payload;

  // These maintain the current state
  unsigned long previousMillis;   // will store last time it was updated

  // Constructor - creates a Flasher 
  // and initializes the member variables and state
  public:
  MonitorMeteo();
  void configure(long setUpdatePeriod = 30000);
  void readData();
  void update();
  String* readPayload();
};

#endif
