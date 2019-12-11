//**************************************
//*** BEGIN CLASS MONITORMETEO 
//**************************************

#include "MonitorMeteo.h"


MonitorMeteo::MonitorMeteo() 
{
  previousMillis = millis() - 100000;
}

void MonitorMeteo::configure(long setUpdatePeriod = 30000)
{
  updatePeriod = setUpdatePeriod;
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  }
}

void MonitorMeteo::readData() {
      th.Read();
      temperature = th.t * (1 - ERROR_TEMP);
      humidity = th.h;
      pressure = bmp.readPressure() / 100;
      
      payload = "{\"temperature\":";
      payload += temperature;
      payload += ",\"humidity\":";
      payload += humidity;
      payload += ",\"pressure\":";
      payload += pressure;
      payload += "}";
      
      Serial.println(payload);
}

String* MonitorMeteo::readPayload()
{
  return &payload;
}

void MonitorMeteo::update()
{
  unsigned long currentMillis, pendingMillis;
  
  // check to see if it's time to change the state of the LED
  currentMillis = millis();
  pendingMillis = currentMillis - previousMillis;
  
  if (pendingMillis >= updatePeriod) {
    // Time to update
    Debug::log("Start MonitorMeteo Update");
    readData();
    previousMillis = currentMillis;
    Debug::log("End MonitorMeteo Update");
  }
}

//**************************************
//*** END CLASS MONITORMETEO
//**************************************
