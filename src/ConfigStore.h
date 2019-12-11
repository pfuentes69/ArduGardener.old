#ifndef ConfigStore_h
#define ConfigStore_h

#include <EEPROM.h>
#include <Arduino.h>

// Custom types
struct ConfigStruct {
  bool autoMode;
  long autoFrequency;
  long autoTime;
};

class ConfigStore
{
  ConfigStruct configData;

  public:
  ConfigStore();
  bool loadConfig();
  void saveConfig();
  void setConfig(ConfigStruct cfg);
  ConfigStruct getConfig();
};

#endif
