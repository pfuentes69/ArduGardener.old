#ifndef Debug_h
#define Debug_h

//#define DEBUG

#include <Arduino.h>

class Debug {
  public:
  Debug();

  static void log(String msg);
};

#endif
