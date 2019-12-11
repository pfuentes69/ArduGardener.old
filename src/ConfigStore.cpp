//**************************************
//*** BEGIN CLASS CONFIGSTORE 
//**************************************

#include "ConfigStore.h"

ConfigStore::ConfigStore() 
{
}

bool ConfigStore::loadConfig()
{
  bool cfgExists;  // To know if there's data
  int eeAddress = 0;   //Location we want the data to be put.
  EEPROM.get(eeAddress, cfgExists);
  if (cfgExists) {
    eeAddress = sizeof(bool); //Move address to the next byte after boolean.
    EEPROM.get(eeAddress, configData);
    return true;
  } else {
    return false;
  }
}

void ConfigStore::saveConfig()
{
  bool cfgExists = true;  // To mark there's data
  int eeAddress = 0;   //Location we want the data to be put.
  EEPROM.put(eeAddress, cfgExists);
  eeAddress += sizeof(bool); //Move address to the next byte after float 'f'.
  EEPROM.put(eeAddress, configData);
  Serial.println("Configuration saved in EEPROM");
}

void ConfigStore::setConfig(ConfigStruct cfg)
{
  configData = cfg;
}

ConfigStruct ConfigStore::getConfig()
{
  return configData;
}
//**************************************
//*** END CLASS CONFIGSTORE 
//**************************************
