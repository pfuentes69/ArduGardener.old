//**************************************
//*** BEGIN CLASS MONITORINTTEMP 
//**************************************

#include "MonitorIntTemp.h"

MonitorIntTemp::MonitorIntTemp() 
{
  previousMillis = 0;
  ventStatus = false;
}

void MonitorIntTemp::configure(int setAlarmTemp = 30, long setUpdatePeriod = 1000)
{
  alarmTemp = setAlarmTemp;
  updatePeriod = setUpdatePeriod;
  pinMode(LM35_PIN, INPUT); //declara pin del LM de entrada
  pinMode(VENT_PIN, OUTPUT);
  digitalWrite(VENT_PIN, LOW);    // turn the VENT off by making the voltage LOW
}

void MonitorIntTemp::update()
{
  unsigned long currentMillis, pendingMillis;
  
  // check to see if it's time to change the state of the LED
  currentMillis = millis();
  pendingMillis = currentMillis - previousMillis;
  
  if (pendingMillis >= updatePeriod) {
    Debug::log("Start MonitorIntTemp Update");
    // Time to update
    //Calcula la temperatura usando como referencia 5v
    delay(10); // Estabilizar lectura, preventivo
    int temp = analogRead(LM35_PIN);
    delay(10); // Estabilizar lectura
    temp = analogRead(LM35_PIN);
    delay(10); // Estabilizar lectura
    float temperatura = (5.0 * temp*100.0)/1023.0;
      Serial.print("Int temp = ");
      Serial.println (temperatura); //escribe la temperatura en el serial
    if (!ventStatus && (temperatura >= alarmTemp)) {
      Serial.println("Encender ventilador");
      digitalWrite(VENT_PIN, HIGH);   // turn the vent on (HIGH is the voltage level)
      ventStatus = true;
    } else if (ventStatus && (temperatura < (alarmTemp - 4))) {
      Serial.println("Temperatura normal");
      digitalWrite(VENT_PIN, LOW);    // turn the LED off by making the voltage LOW
      ventStatus = false;
    }
    previousMillis = currentMillis;
    Debug::log("End MonitorIntTemp Update");
  }
}
//**************************************
//*** END CLASS MONITORINTTEMP 
//**************************************
