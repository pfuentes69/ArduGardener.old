#ifndef RedESP_h
#define RedESP_h

#include <Arduino.h>
#include <WiFiEspMOD.h>

class RedESP {

  // use a ring buffer to increase speed and reduce memory allocation
  RingBuffer buf;
  WiFiEspServer server;
  IPAddress IPserver;
  int status;   // the Wifi radio's status

  char* sSSID;
  char* sPWD;
  bool staticIP;

  public:
  RedESP();

  void configure(char *strSSID, char* strPWD, IPAddress IPA, int port = 80);
  void configure(char *strSSID, char* strPWD, int port = 80);
  bool begin();  
  void update(String *sPayload); 
};

#endif
