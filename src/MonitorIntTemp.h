#ifndef MonitorIntTemp_h
#define MonitorIntTemp_h

#include <Arduino.h>
#include "BoardSetup.h"
#include "Debug.h"

class MonitorIntTemp
{
  // Class Member Variables
  long updatePeriod;
  int alarmTemp;

  // These maintain the current state
  unsigned long previousMillis;   // will store last time it was updated
  bool ventStatus;

  // Constructor - creates a Flasher 
  // and initializes the member variables and state
  public:
  MonitorIntTemp();
  void configure(int setAlarmTemp = 30, long setUpdatePeriod = 1000);
  void update();
};
#endif
