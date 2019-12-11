//**************************************
//*** BEGIN CLASS RELAYTEMP 
//**************************************

#include "RelayTemp.h"

RelayTemp::RelayTemp() 
{
  previousMillis = 0;    
  relayState = false; 
  updating = false;
  repeat = true;
  runningState = false;
}

void RelayTemp::configure(int pinR, int pinL)
{
  relayPin = pinR;
  relayLedPin = pinL;
  pinMode(relayPin, OUTPUT);
  pinMode(relayLedPin, OUTPUT);
  setRelay();
}

void RelayTemp::cycle(long on, long off, bool initState = false, bool runState = true, bool rep = true)
{
  relayState = initState;
  repeat = rep;
  this->setRelay();
    
  OnTime = on * 1000;
  OffTime = off * 1000;  

  runningState = true;

  if (runState)
    this->start();
  else
    this->pause();
}

bool RelayTemp::getRunningState()
{
  return runningState;
}

void RelayTemp::setRelay() 
{
  if (relayState) {
    Serial.println("Relay ON");
    digitalWrite(relayLedPin, LOW);
    digitalWrite(relayPin, LOW);    
  } else {
    Serial.println("Relay OFF");
    digitalWrite(relayLedPin, HIGH);
    digitalWrite(relayPin, HIGH);    
  }
}

bool RelayTemp::getState()
{
  return relayState;
}

bool RelayTemp::getMode()
{
  return updating;
}

void RelayTemp::start()
{
  updating = true;
  runningState = true;
  previousMillis = millis();
  Serial.println("Cycle start");
}

void RelayTemp::cancel()
{
  updating = false;
  runningState = false;
  Serial.println("Cycle cancel");
  relayState = false;
  this->setRelay();
}

void RelayTemp::stop()
{
  updating = false;
  runningState = false;
  Serial.println("Relay stop");
  relayState = false;
  this->setRelay();
  previousMillis = 0;
}

void RelayTemp::pause()
{
  updating = false;
  Serial.println("Relay pause");
}

void RelayTemp::resume()
{
  updating = true;
  Serial.println("Relay resume");
}

long RelayTemp::getRemaining()
{
  unsigned long pendingMillis = millis() - previousMillis;
  
  if (relayState) {
    return (OnTime - pendingMillis) / 1000; 
  } else {
    return (OffTime - pendingMillis) / 1000; 
  }
}

void RelayTemp::update()
{
  unsigned long currentMillis, pendingMillis;
  
  if (!updating)
    return;
    
  // check to see if it's time to change the state of the LED
  currentMillis = millis();
  pendingMillis = currentMillis - previousMillis;
  
  if (relayState && (pendingMillis >= OnTime)) {
    Debug::log("Begin RelayTemp ON Update");
    // Time to update
    relayState = !relayState;  // inverse status
    this->setRelay();
    previousMillis = currentMillis;  // Remember the time
    if (!repeat) {
      // Not to repeat again
      this->stop();
    }
    Debug::log("End RelayTemp ON Update");
  } else if (!relayState && (pendingMillis >= OffTime)) {
    Debug::log("Begin RelayTemp OFF Update");
    // Time to update
    relayState = !relayState;  // inverse status
    this->setRelay();
    previousMillis = currentMillis;  // Remember the time
    Debug::log("End RelayTemp OFF Update");
  }
}
//**************************************
//*** END CLASS RELAYTEMP 
//**************************************
