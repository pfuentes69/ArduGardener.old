//**************************************
//*** BEGIN CLASS DEBUG 
//**************************************

#include "Debug.h"

Debug::Debug() 
  {
  }
  
void Debug::log(String msg)
  {
#ifdef DEBUG
    Serial.println("LOG: " + msg);
#endif
  }

//**************************************
//*** END CLASS DISPLAY 
//**************************************
