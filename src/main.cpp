
//**************************************
//*** BEGIN MEGAGARDENER
//*** 
//*** Version 2.3
//*** 
//*** Autor: Pedro Fuentes
//*** 
//**************************************

//#define USE_NETWORK

// CHECK IF NEEDED "#define DEBUG" in Debug.h

#include <Wire.h>
#include "RTClib.h"

#include "Debug.h"

#include "RedESP.h"
#include "BoardSetup.h"
#include "ConfigStore.h"
#include "MonitorIntTemp.h"
#include "MonitorMeteo.h"
#include "RelayTemp.h"
#include "Display.h"
#include "RGBFlasher.h"

#define VERSION "2.3"

#define NO_MENU -1
#define MENU_DEFAULT 0

#define SW0 0
#define SW1 1
#define SW2 2
#define SW3 3
#define SW4 4
#define SW5 5

#define SW_NONE 0
#define SW_LEFT 1
#define SW_UP 2
#define SW_DOWN 3
#define SW_RIGHT 4
#define SW_ENTER 5

#define IDDLE_MODE 0
#define MENU_MODE 1
#define MANUAL_MODE 2
#define AUTO_MODE 3
#define AUTO_WATERING 4
#define ERROR_MODE 5

#define NO_COMMAND 0
#define STOP 1
#define AUTO_6H 11
#define AUTO_12H 12
#define AUTO_24H 13
#define AUTO_3M 21
#define AUTO_5M 22
#define AUTO_7M 23
#define AUTO_10M 24
#define AUTO_ON 31
#define AUTO_OFF 32
#define MANUAL_3M 33
#define MANUAL_5M 34
#define MANUAL_7M 35
#define MANUAL_10M 36
#define CERRAR_MENU 91

// Global Constants
const long defaultOffPeriod = 10; // Seconds between Relay
const long defaultOnPeriod = 3; // Seconds during Relay
const int notes[] = { 262, 294, 330, 349, 392, 440, 494 };
const char ssid[] = "Papillon";            // your network SSID (name)
const char pass[] = "70445312";        // your network password

const char* keys[] = {
  "SW_NONE",
  "SW_LEFT",
  "SW_UP",
  "SW_DOWN",
  "SW_RIGHT",
  "SW_ENTER"
};

const char* modes[] = {
  "IDDLE",
  "MENU",
  "MANUAL",
  "AUTO",
  "ERROR"
};

const char* menu[] = 
  {//1234567812345678
    "FRECUENCIA AUTO ",
    "DURACION AUTO   ",
    "MODO            ",
    "SALIR           ",
    ""
  };

const char* submenu[][7] = {
  {//1234567812345678
    "AUTO cada 6H    ",
    "AUTO cada 12H   ",
    "AUTO cada 24H   ",
    ""
  },
  {//1234567812345678
    "AUTO por 3 mins ",
    "AUTO por 5 mins ",
    "AUTO por 7 mins ",
    "AUTO por 10 mins",
    ""
  },
  {//1234567812345678
    "AUTO ON         ",
    "AUTO OFF        ",
    "MANUAL 3 mins   ",
    "MANUAL 5 mins   ",
    "MANUAL 7 mins   ",
    "MANUAL 10 mins  ",
    ""
  },
  {
    "                "
  },
  {
    ""
  }
};

int command[][7] = {
  {
    AUTO_6H,
    AUTO_12H,
    AUTO_24H
  },
  {
    AUTO_3M,
    AUTO_5M,
    AUTO_7M,
    AUTO_10M
  },
  {
    AUTO_ON,
    AUTO_OFF,
    MANUAL_3M,
    MANUAL_5M,
    MANUAL_7M,
    MANUAL_10M
  },
  {
    CERRAR_MENU
  }
};


// Global objects
IPAddress myIP(192,168,2,90);
WiFiEspClient espClient;
RTC_DS1307 RTC;

bool useNetwork = false;

/*
 * Functions
 */
void showHour();
void showTemp();
int getKeyValue();
void waitKeyRelease();
int readKeyboard();
void setRelay(bool statusRelay);

RedESP redESP;
ConfigStore configStore;
ConfigStruct systemConfig;
MonitorIntTemp monitorIntTemp;
MonitorMeteo monitorMeteo;
RelayTemp relayTemp;
Display display;
RGBFlasher ledRGB;

//**************************************
//*** BEGIN CLASS GARDENER 
//**************************************
class Gardener {
  long updateInterval;
  long lastUpdate;

  int mode;
  int prevMode;
  int mAct;
  int smAct;

  int timerManual;

  public:
  Gardener () 
  {
    mode = prevMode = -1; // Invalid mode, to force initial mode properly
    mAct = NO_MENU;
    smAct = NO_MENU;
    lastUpdate = 0;
  }

  void configure(long interval = 20) 
  {
    updateInterval = interval;
  }

  void start() {
    this->defaultMode();
    lastUpdate = millis();
  }

  void setMode(int m) 
  {
    // we only do something if the mode changed
    if (mode != m) {
      mode = m;
      Serial.print("New mode: ");
      Serial.println(modes[m]);
      display.clear();
      switch (m) {
        // Prepare and lauch modes
        case AUTO_MODE:
          Debug::log("Start auto mode...");
          ledRGB.configure(STATUS_LED_R_PIN, STATUS_LED_G_PIN, STATUS_LED_B_PIN, RGB_FLASHER_COLOR_GREEN, RGB_FLASHER_MODE_BLINK10s);
          // Start Relay Cycle
          if (relayTemp.getRunningState())
            relayTemp.resume();
          else
            relayTemp.cycle(systemConfig.autoTime, systemConfig.autoFrequency - systemConfig.autoTime, false, true, true); // initState, runState, rep
          defaultDisplay();
          break;
        case IDDLE_MODE:
          ledRGB.configure(STATUS_LED_R_PIN, STATUS_LED_G_PIN, STATUS_LED_B_PIN, RGB_FLASHER_COLOR_YELLOW, RGB_FLASHER_MODE_BLINK10s);
          defaultDisplay();
          relayTemp.cancel();
          break;
        case MANUAL_MODE:
          ledRGB.configure(STATUS_LED_R_PIN, STATUS_LED_G_PIN, STATUS_LED_B_PIN, RGB_FLASHER_COLOR_BLUE, RGB_FLASHER_MODE_BLINK4);
          display.clear();
          display.print(2, "Regando...");
          Debug::log("Start manual watering...");
          // Start Relay Cycle
          relayTemp.cycle(timerManual, 0, true, true, false); // initState, runState, rep
          break;
        case AUTO_WATERING:
          ledRGB.configure(STATUS_LED_R_PIN, STATUS_LED_G_PIN, STATUS_LED_B_PIN, RGB_FLASHER_COLOR_BLUE, RGB_FLASHER_MODE_BLINK4);
          break;
        case ERROR_MODE:
          ledRGB.configure(STATUS_LED_R_PIN, STATUS_LED_G_PIN, STATUS_LED_B_PIN, RGB_FLASHER_COLOR_RED, RGB_FLASHER_MODE_BLINK6);
          display.print(2, "ERROR!!!");
          relayTemp.cancel();
          break;
      }
    }
  }

  void defaultMode()
  {
    // Set initial Mode
    if (systemConfig.autoMode)
      this->setMode(AUTO_MODE);
    else
      this->setMode(IDDLE_MODE);  
    this->defaultDisplay();
  }

  void defaultDisplay()
  {
    switch (mode) {
      case AUTO_MODE:
        display.print(3, "AUTO");
        showHour();
        showTemp();
        break;
      case IDDLE_MODE:
        display.print(3, "INACTIVO");
        showHour();
        showTemp();
        break;
    }
  }
  
  void setMenu(int m, int sm) {
    mAct = m;
    smAct = sm;
    Serial.print("Menu principal: ");
    Serial.println(menu[mAct]);
    Serial.print("Submenu: ");
    Serial.println(submenu[mAct][smAct]);
    display.clear();
    display.print(0, menu[mAct]);
    display.print(1, submenu[mAct][smAct]);
  }
  
  void executeCommand(int cmd) {
    Serial.print("Execute command: ");
    Serial.println(cmd);
    switch (cmd) {
      case AUTO_6H:
      case AUTO_12H:
      case AUTO_24H:
        display.print(2, "Guardar config! ");
        delay(1000);
        if (cmd == AUTO_6H)
          systemConfig.autoFrequency = 21600L;
        else if (cmd == AUTO_12H)
          systemConfig.autoFrequency = 43200L;
        else
          systemConfig.autoFrequency = 86400L;
        configStore.setConfig(systemConfig);
        configStore.saveConfig();
        this->defaultMode();
        break;
      case AUTO_3M:
      case AUTO_5M:
      case AUTO_7M:
      case AUTO_10M:
        display.print(2, "Guardar config! ");
        delay(1000);
        if (cmd == AUTO_3M)
          systemConfig.autoTime = 180L;
        else if (cmd == AUTO_5M)
          systemConfig.autoTime = 300L;
        else if (cmd == AUTO_7M)
          systemConfig.autoTime = 420L;
        else
          systemConfig.autoTime = 600L;
        configStore.setConfig(systemConfig);
        configStore.saveConfig();
        this->defaultMode();
        break;
      case MANUAL_3M:
      case MANUAL_5M:
      case MANUAL_7M:
      case MANUAL_10M:
        if (cmd == MANUAL_3M)
          timerManual = 180L;
        else if (cmd == MANUAL_5M)
          timerManual = 300L;
        else if (cmd == MANUAL_7M)
          timerManual = 420L;
        else
          timerManual = 600L;
        setMode(MANUAL_MODE);
        break;
      case AUTO_ON:
      case AUTO_OFF:
        display.print(2, "Guardar config! ");
        delay(1000);
        if (cmd == AUTO_ON) {
          systemConfig.autoMode = true;
          this->setMode(AUTO_MODE);
        } else {
          systemConfig.autoMode = false;
          this->setMode(IDDLE_MODE);
        }
        configStore.setConfig(systemConfig);
        configStore.saveConfig();
        break;
      case CERRAR_MENU:
        this->defaultMode();
        break;
    }
  }

  void update() 
  {
    int key;
    String sTime = "";
    String s;
    long hours, minutes, seconds, rem;
   
    if ((millis() - lastUpdate) > updateInterval) {
//      Debug::log("Start Gardener Update");
      // Check water level
      if (!digitalRead(WATER_LEVEL_PIN)) {
        // No Water we need to change the mode to error
        this->setMode(ERROR_MODE);
      }
      // time to update
      key = readKeyboard();
      switch (mode) {
        case ERROR_MODE: // Some error 
          // We can't do anything
          // To recover it's required to restart manually
          break;
        case AUTO_MODE: // Auto
        case AUTO_WATERING: // Auto on watering
          if (relayTemp.getState()) {
            // Water is on, we cut if key or we show timer
            if (key != SW_NONE) {
                // We enter in default mode
                Debug::log("Cut auto watering");
                relayTemp.cancel();
                this->defaultMode();
            } else {
              // We change mode to Watering
              this->setMode(AUTO_WATERING);
              // We show the remaining water mode
              rem = relayTemp.getRemaining();
              s = "0" + String(rem / 60); // Minutes
              s = s.substring(s.length() - 2);
              sTime += s;
              sTime += ":";
              s = "0" + String(rem % 60); // Seconds
              s = s.substring(s.length() - 2);
              sTime += s;
              sTime += "           ";
              display.print(2, "Regando...");
              display.print(3, sTime);     
              showHour();
              showTemp();
            } 
          } else {
            // Water is off, we show the remaining waiting time
            if (key != SW_NONE) {
              // We enter in menu mode
              this->setMode(MENU_MODE);
              this->setMenu(MENU_DEFAULT, MENU_DEFAULT);
            } else {
              // We change mode to Watering
              this->setMode(AUTO_MODE);
              // We show the remaining waiting
              rem = relayTemp.getRemaining();
              hours = rem / 3600;
              minutes = (rem - hours * 3600) / 60;
              seconds = (rem - hours * 3600) % 60;
              s = "0" + String(hours); // Hours
              s = s.substring(s.length() - 2);
              sTime += s;
              sTime += ":";
              s = "0" + String(minutes); // Seconds
              s = s.substring(s.length() - 2);
              sTime += s;
              sTime += ":";
              s = "0" + String(seconds);
              s = s.substring(s.length() - 2);
              sTime += s;
//              display.setCursor(5, 0);
//              display.print("           ");
              showHour();
              showTemp();
              display.print(3, "AUTO en ");
              display.setCursor(8, 3);
  //            Serial.println(sTime);
              display.print(sTime);
            }
          }
          break;
        case IDDLE_MODE: // Iddle
          showHour();
          showTemp();
          if (key != SW_NONE) {
              // We enter in menu mode
              this->setMode(MENU_MODE);
              this->setMenu(MENU_DEFAULT, MENU_DEFAULT);
          }
          break;
        case MANUAL_MODE:
          if (key != SW_NONE) {
              // We enter in default mode
              Debug::log("Cut manual watering");
              relayTemp.cancel();
              this->defaultMode();
          } else {
            if (relayTemp.getState()) {
              s = "0" + String(relayTemp.getRemaining() / 60); // Minutes
              s = s.substring(s.length() - 2);
              sTime += s;
              sTime += ":";
              s = "0" + String(relayTemp.getRemaining() % 60); // Seconds
              s = s.substring(s.length() - 2);
              sTime += s;
            //  sTime += ":";
            //  s = "0" + String(now.second());
            //  s = s.substring(s.length() - 2);
            //  sTime += s;
              showHour();
              showTemp();
              display.print(3, sTime);     
            } else {
              // Timer ended, change mode
              Debug::log("End manual watering");
              this->defaultMode();
            }
          }
          break;
        case MENU_MODE: // Menu
          if (key != SW_NONE) {
            // Update Menu
            switch (key) {
              case SW_UP:
                if (mAct > 0) {
                  // Menu Up
                  this->setMenu(mAct - 1, MENU_DEFAULT);
                }
                break;
              case SW_DOWN: 
                if (menu[mAct + 1] != "") {
                  // Menu Up
                  this->setMenu(mAct + 1, MENU_DEFAULT);
                }
                break;
              case SW_LEFT: 
                if (smAct > 0) {
                  // Menu Up
                  this->setMenu(mAct, smAct - 1);
                }
                break;
              case SW_RIGHT: 
                if (submenu[mAct][smAct + 1] != "") {
                  // Menu Up
                  this->setMenu(mAct, smAct + 1);
                }
                break;
              case SW_ENTER:
                this->executeCommand(command[mAct][smAct]);
                break;
            }
          }
          break;
      }
      // Record time for next update
      lastUpdate = millis();

//      Debug::log("End Gardener Update");
    }
  }
};

//**************************************
//*** END CLASS GARDENER 
//**************************************

Gardener gardener;

void setup () {
  Serial.begin(115200);
  Debug::log("***** SETUP START ****");
  Wire.begin();
  RTC.begin();

  if (configStore.loadConfig()) {
    systemConfig = configStore.getConfig();
    Debug::log("Configuration loaded");
    Serial.print("Auto mode:");
    Serial.println(systemConfig.autoMode);
    Serial.print("Auto Frequency: ");
    Serial.println((long)(systemConfig.autoFrequency));
    Serial.print("Auto Timer: ");
    Serial.println(systemConfig.autoTime);
  } else {
    Debug::log("Config doesn't exist. Must use defaults!");
    systemConfig = {
      false,   // bool autoMode;
      6 * 60 * 60, // int autoFrequency;
      3 * 60  // int autoTime;
    };
    configStore.setConfig(systemConfig);
    configStore.saveConfig();
  }

  display.configure(LCD_ROWS, LCD_LINES);
  relayTemp.configure(RELAY_PIN, RELAY_LED_PIN);
  gardener.configure(100);

  // Set the water level sensor
  pinMode(WATER_LEVEL_PIN, INPUT_PULLUP);

  // set the analogue pin for input and turn on the internal pullup resistor
  pinMode(KEYPAD_PIN, INPUT_PULLUP); 

/*
  Serial.println("Testing relay 2min");
  setRelay(true);
  delay(120000);
  setRelay(false);
*/

  if (! RTC.isrunning()) {
    Debug::log("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled/
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
//  RTC.adjust(DateTime(__DATE__, __TIME__));

  // Print a message to the LCD
//  lcd.backlight();
  display.print(0, "Papillon Garden ");
  display.print(1, "Version 2.4     ");
  delay(200);

  monitorIntTemp.configure(35, 30000);
  monitorMeteo.configure(30000);

#ifdef USE_NETWORK  
  redESP.configure(ssid, pass, myIP, 80);

  // check network init result
  if (!redESP.begin()) {
    Debug::log("Network problem!");
    display.print(2, "ESP Module Error");
    delay(5000);
    useNetwork = false;
  }
  else {
    // you're connected now, so print out the data
    Debug::log("You're connected to the network");
    display.print(2, "Connected!      ");
//      delay(500);
//      display.print(1, localIP);
    delay(2000);
    useNetwork = true;
  }
#else
  useNetwork = false;
  Debug::log("Network disabled");
#endif

  display.clear();
  gardener.start();

  Debug::log("***** SETUP END ****");
}

void loop () {
    gardener.update();
    relayTemp.update();
    monitorIntTemp.update();
    monitorMeteo.update();
    ledRGB.update();
    display.update();
#ifdef USE_NETWORK  
    redESP.update(monitorMeteo.readPayload());
#endif
}

/*
 * GENERAL PURPOSE FUNCTIONS
 */
 
/*
 * Show Hour Function
 */
void showHour()
{
  DateTime now = RTC.now();
  String sTime;
  String s;

  s = "0" + String(now.hour());
  s = s.substring(s.length() - 2);
  sTime += s;
  sTime += ":";
  s = "0" + String(now.minute());
  s = s.substring(s.length() - 2);
  sTime += s;
//  sTime += ":";
//  s = "0" + String(now.second());
//  s = s.substring(s.length() - 2);
//  sTime += s;
  display.setCursor(0, 0);
  display.print(sTime);      
}

void showTemp()
{
  
  char buf1[50];
  char bufT[10];
  dtostrf(monitorMeteo.temperature, 1, 1, bufT);
  sprintf(buf1,"%s*C", bufT);
  String sTemp = buf1;
  display.setCursor((int)(20 - sTemp.length()), 0);
  display.print(sTemp);      
  
}

int getKeyValue()
{
    int pinVal;
    int button = 0;
    pinVal = analogRead(KEYPAD_PIN);
//    Serial.println(pinVal);
    if (abs(pinVal - 13) <= 10) {
      button = 1;
    } else if (abs(pinVal - 54) <= 10) {
      button = 2;
    } else if (abs(pinVal - 122) <= 10) {
      button = 3;
    } else if (abs(pinVal - 214) <= 10) {
      button = 4;
    } else if (abs(pinVal - 417) <= 10) {
      button = 5;
    } else {
      button = 0;
    }
    return button;
}

void waitKeyRelease()
{
    int val;
    
    val = getKeyValue();
//    Serial.println(val);
    while (val != SW0) {
      delay(20);
      val = getKeyValue();
//      Serial.println(val);
    }
}

int readKeyboard() {
  int val;
  val = getKeyValue();
  if (val != SW_NONE) {
    Debug::log("Key pressed");
    display.resetLightCount();
    tone(BUZZER_PIN, notes[val], 20);
    delay(10);
//    Serial.print("Key pressed: ");
//    Serial.println(keys[val]);
    waitKeyRelease();
  }
  return val;
}
