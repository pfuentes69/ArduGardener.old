#ifndef RGBFlasher_h
#define RGBFlasher_h

#include <Arduino.h>
#include "Debug.h"

#define RGB_FLASHER_MODE_OFF 0 // Off
#define RGB_FLASHER_MODE_BLINK1 1 // Blink 100 / 500
#define RGB_FLASHER_MODE_BLINK2 2 // Blink 100 / 200
#define RGB_FLASHER_MODE_BLINK3 3 // Blink 100 / 100
#define RGB_FLASHER_MODE_BLINK4 4 // Blink 500 / 500
#define RGB_FLASHER_MODE_BLINK5 5 // Blink 1000 / 1000
#define RGB_FLASHER_MODE_BLINK6 6 // Blink 100 / 1000
#define RGB_FLASHER_MODE_BLINK10 10 // Blink 100 / 10000
#define RGB_FLASHER_MODE_BLINK10s 101 // Blink 10 / 10000
#define RGB_FLASHER_MODE_BLINK30 30 // Blink 100 / 10000
#define RGB_FLASHER_MODE_BLINK60 60 // Blink 100 / 10000
#define RGB_FLASHER_MODE_ON 100 // On

#define RGB_FLASHER_COLOR_OFF 0 // Off
#define RGB_FLASHER_COLOR_RED 1 // Red
#define RGB_FLASHER_COLOR_GREEN 2 // Green
#define RGB_FLASHER_COLOR_BLUE 3 // Blue
#define RGB_FLASHER_COLOR_YELLOW 7 // Yellow
#define RGB_FLASHER_COLOR_MAGENTA 5 // Magenta
#define RGB_FLASHER_COLOR_CYAN 6 // Cyan
#define RGB_FLASHER_COLOR_WHITE 4 // Blanco
#define RGB_FLASHER_COLOR_CYCLE 10 // Ciclar

//**************************************
//*** BEGIN CLASS FLASHER 
//**************************************
class RGBFlasher
{
  // Class Member Variables
  // These are initialized at startup
  int ledRPin;      // the number of the LED pin
  int ledGPin;      // the number of the LED pin
  int ledBPin;      // the number of the LED pin
  long OnTime;     // milliseconds of on-time
  long OffTime;    // milliseconds of off-time

  // These maintain the current state
  int ledState;                 // ledState used to set the LED
  int ledMode;                 // ledState used to set the LED
  int ledColor;                 // ledState used to set the LED
  unsigned long previousMillis;   // will store last time LED was updated
  int tempColor;

  // Constructor - creates a Flasher 
  // and initializes the member variables and state
  public:
  Flasher();

  Flasher(int rPin, int gPin, int bPin, int lColor, int lMode);

  void configure(int rPin, int gPin, int bPin, int lColor, int lMode);
  void configureMode(int lColor, int lMode);
  void setColor(int colLed);
  void cycleColor();
  void update();
};

#endif
