#ifndef Display_h
#define Display_h

#include <Arduino.h>
#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h> // include i/o class header
#include "Debug.h"

class Display
{
  hd44780_I2Cexp lcd; // declare lcd object: auto locate & config display for hd44780 chip
  String bufLCD[4];
  int X, Y;
  unsigned long updatePeriod;
  int lightCountInit;
  // These maintain the current state
  unsigned long previousMillis;   // will store last time it was updated
  int lightCount;
  
  public:
  Display();
  void configure(int lines, int rows, unsigned long setUpdatePeriod = 50, int setLightCountInit = 200);
  void clear();
  void backlight();
  void noBacklight(); 
  void setCursor(int x, int y);
  void print(String str);
  void print(int line, String str);
  void resetLightCount();
  void update();
};

#endif
