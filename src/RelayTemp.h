#ifndef RelayTemp_h
#define RelayTemp_h

#include <Arduino.h>
#include "Debug.h"

class RelayTemp
{
  // Class Member Variables
  // These are initialized at startup
  int relayPin;      // the number of the RELAY pin
  int relayLedPin;      // the number of the LED pin
  long OnTime;     // seconds of on-time
  long OffTime;    // seconds of off-time

  // These maintain the current state
  bool relayState;                 // ledState used to set the Relay and Led
  unsigned long previousMillis;   // will store last time it was updated
  bool updating;
  bool repeat;
  bool runningState;

  public:
  RelayTemp();
  void configure(int pinR, int pinL);
  void cycle(long on, long off, bool initState = false, bool runState = true, bool rep = true);
  bool getRunningState();
  void setRelay();
  bool getState();
  bool getMode();
  void start();
  void cancel();
  void stop();
  void pause();
  void resume();
  long getRemaining();
  void update();
};

#endif
