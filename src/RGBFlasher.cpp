//**************************************
//*** BEGIN CLASS FLASHER 
//**************************************
#include "RGBFlasher.h"

RGBFlasher::Flasher()
{  
  ledState = LOW; 
  previousMillis = 0;
}

RGBFlasher::Flasher(int rPin, int gPin, int bPin, int lColor, int lMode)
{
   configure(rPin, gPin, bPin, lColor, lMode);
}

void RGBFlasher::configure(int rPin, int gPin, int bPin, int lColor, int lMode)
{
  ledRPin = rPin;
  ledGPin = gPin;
  ledBPin = bPin;
  pinMode(ledRPin, OUTPUT);     
  pinMode(ledGPin, OUTPUT);     
  pinMode(ledBPin, OUTPUT);     
    
  switch (lMode) {
    case RGB_FLASHER_MODE_OFF: // Off
      OnTime = 0;
      OffTime = 1000;
      break;
    case RGB_FLASHER_MODE_BLINK1: // Blink 100 / 500
      OnTime = 100;
      OffTime = 500;
      break;
    case RGB_FLASHER_MODE_BLINK2: // Blink 100 / 200
      OnTime = 100;
      OffTime = 200;
      break;
    case RGB_FLASHER_MODE_BLINK3: // Blink 100 / 100
      OnTime = 100;
      OffTime = 100;
      break;
    case RGB_FLASHER_MODE_BLINK4: // Blink 500 / 500
      OnTime = 500;
      OffTime = 500;
      break;
    case RGB_FLASHER_MODE_BLINK5: // Blink 1000 / 1000
      OnTime = 1000;
      OffTime = 1000;
      break;
    case RGB_FLASHER_MODE_BLINK6: // Blink 100 / 1000
      OnTime = 100;
      OffTime = 1000;
      break;
    case RGB_FLASHER_MODE_BLINK10: // Blink 100 / 1000
      OnTime = 100;
      OffTime = 10000;
      break;
    case RGB_FLASHER_MODE_BLINK10s: // Blink 100 / 1000
      OnTime = 10;
      OffTime = 10000;
      break;
    case RGB_FLASHER_MODE_BLINK30: // Blink 100 / 1000
      OnTime = 100;
      OffTime = 30000;
      break;
    case RGB_FLASHER_MODE_BLINK60: // Blink 100 / 1000
      OnTime = 100;
      OffTime = 60000;
      break;
    case RGB_FLASHER_MODE_ON: // On
      OnTime = 300;
      OffTime = 0;
      break;
  }

  ledMode = lMode; 
  ledColor = tempColor = lColor;
  
  ledState = LOW; 
  previousMillis = 0;
}

void RGBFlasher::configureMode(int lColor, int lMode)
{
  configure(ledRPin, ledGPin, ledBPin, lColor, lMode);
}

void RGBFlasher::setColor(int colLed)
{
  switch (colLed) {
    case RGB_FLASHER_COLOR_OFF: // Ciclar
      digitalWrite(ledRPin, LOW);
      digitalWrite(ledGPin, LOW);
      digitalWrite(ledBPin, LOW);
      break;
    case RGB_FLASHER_COLOR_RED: // Red
      digitalWrite(ledRPin, HIGH);
      digitalWrite(ledGPin, LOW);
      digitalWrite(ledBPin, LOW);
      break;
    case RGB_FLASHER_COLOR_GREEN: // Green
      digitalWrite(ledRPin, LOW);
      digitalWrite(ledGPin, HIGH);
      digitalWrite(ledBPin, LOW);
      break;
    case RGB_FLASHER_COLOR_BLUE: // Blue
      digitalWrite(ledRPin, LOW);
      digitalWrite(ledGPin, LOW);
      digitalWrite(ledBPin, HIGH);
      break;
    case RGB_FLASHER_COLOR_YELLOW: // Yellow
      digitalWrite(ledRPin, HIGH);
      digitalWrite(ledGPin, HIGH);
      digitalWrite(ledBPin, LOW);
      break;
    case RGB_FLASHER_COLOR_MAGENTA: // Magenta
      digitalWrite(ledRPin, HIGH);
      digitalWrite(ledGPin, LOW);
      digitalWrite(ledBPin, HIGH);
      break;
    case RGB_FLASHER_COLOR_CYAN: // Cyan
      digitalWrite(ledRPin, LOW);
      digitalWrite(ledGPin, HIGH);
      digitalWrite(ledBPin, HIGH);
      break;
    case RGB_FLASHER_COLOR_WHITE: // White
      digitalWrite(ledRPin, HIGH);
      digitalWrite(ledGPin, HIGH);
      digitalWrite(ledBPin, HIGH);
      break;
  }
}

void RGBFlasher::cycleColor()
{
  tempColor = (tempColor % 7) + 1;
  setColor(tempColor);
}

void RGBFlasher::update()
{
  // check to see if it's time to change the state of the LED
  unsigned long currentMillis = millis();
   
  if((ledState == HIGH) && (currentMillis - previousMillis >= OnTime))
  {
//    Debug::log("Start RGBFlasher Update OFF");
    ledState = LOW;  // Turn it off
    previousMillis = currentMillis;  // Remember the time
    setColor(RGB_FLASHER_COLOR_OFF);  // Update the actual LED
//    Debug::log("End RGBFlasher Update OFF");
  }
  else if ((ledState == LOW) && (currentMillis - previousMillis >= OffTime))
  {
//    Debug::log("Start RGBFlasher Update ON");
    ledState = HIGH;  // turn it on
    previousMillis = currentMillis;   // Remember the time
    if (ledColor != RGB_FLASHER_COLOR_CYCLE)
      setColor(ledColor);  // Update the actual LED
    else
      cycleColor();  // Update the actual LED
//    Debug::log("End RGBFlasher Update ON");
  }
}
//**************************************
//*** END CLASS FLASHER 
//**************************************
