//**************************************
//*** BEGIN CLASS DISPLAY 
//**************************************

#include "Display.h"

  Display::Display() 
  {
    previousMillis = 0;
  }
  
  void Display::configure(int lines, int rows, unsigned long setUpdatePeriod = 50, int setLightCountInit = 200)
  {
    lcd.begin(lines, rows);
    this->clear();
    updatePeriod = setUpdatePeriod;
    lightCountInit = lightCount = setLightCountInit;
  }

  void Display::clear()
  {
    lcd.clear();
    //           01234567890123456789
    bufLCD[0] = "                    ";
    bufLCD[1] = "                    ";
    bufLCD[2] = "                    ";
    bufLCD[3] = "                    ";
  }

  void Display::backlight()
  {
    lcd.backlight();
  }

  void Display::noBacklight()
  {
    lcd.noBacklight();
  }

  void Display::setCursor(int x, int y)
  {
    X = x;
    Y = y;
    lcd.setCursor(x, y);
  }

  void Display::print(String str)
  {
    String newBuf = bufLCD[Y];
    // Update buffer
    for (unsigned int i = 0; i < str.length(); i++) {
      newBuf.setCharAt(X+i, str.charAt(i));
    }
    // print if buffer changed
    if (newBuf != bufLCD[Y]) {
//      Serial.print("Anterior:");
//      Serial.println(bufLCD[Y]);
//      Serial.print("Nuevo   :");
//      Serial.println(newBuf);
      bufLCD[Y] = newBuf;
      lcd.print(str);
//      Serial.print("Update LCD: ");
//      Serial.println(str);
    }
  }

  void Display::print(int line, String str) 
  {
    this->setCursor(0, line);
    this->print(str);
  }

  void Display::resetLightCount() 
  {
    lightCount = lightCountInit;
    this->backlight();
  }

  void Display::update()
  {
    unsigned long currentMillis, pendingMillis;
  
    // check to see if it's time to change the state of the LED
    currentMillis = millis();
    pendingMillis = currentMillis - previousMillis;
    
    if (pendingMillis >= updatePeriod) {
//      Debug::log("Start Display Update");
      // Time to update

      if (lightCount > 0) {
        lightCount--;
        if (lightCount == 0)
          this->noBacklight();
      }
  
      previousMillis = currentMillis;
//      Debug::log("End Display Update");
    }
  }

//**************************************
//*** END CLASS DISPLAY 
//**************************************
