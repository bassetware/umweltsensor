#define VERSION "0.3.1"

//includes
#include "config.h"

#include <Wire.h>
#include <ArduinoJson.h>
#include "configfunc.h"


//Sensors
typedef struct sensors {
  float bosch_temp;
  float bosch_hum;
  float bosch_pres;
  float bosch_pres_sea;
  float bosch_gas;
  float bosch_iaq;
  float battery;
  float mhz19_ppm;
  float bh1750_light;
  float ccs811_co2;
  float ccs811_tvoc;
  float dht_temp;
  float dht_hum;
  float ds18b20_temp;
};

sensors sensordata;
int config_err;
StaticJsonDocument<2048> networkdata;
#include "networkfuncs.h"

#include "sensor_bme280.h"
#include "sensor_ccs811.h"
#include "oled_ssd1306.h"
#include "mhz19.h"
sensorMHZ19 mhz(CO2,CO2LIMIT);
#include "battery.h"
#include "BH1750.h"
#include "sensor_ds18b20.h"



void JSONdata(sensors *data) {
  Serial.println("Sensor-data:");
  serializeJsonPretty(networkdata, Serial);
  Serial.println();
  Serial.println();
}


void initSystem() {
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
  }
  config_err = loadConfig();

  networkdata["name"] = jConfig["mqtt"]["Clientname"];
  networkdata["software"] = VERSION;
  
  if(jConfig["network"]["active"] == 1) {
    setupWifi();
  }  
  Wire.begin();
  
  if(jConfig["oled"]["active"] == 1) {
    setupSSD1306();
  }

  if(jConfig["SensorBosch"]["active"] == 1) {
    setupbosch();
  }

  if(jConfig["SensorBH1750"]["active"] == 1) {
    setupBH1750();
  }
   
  if(jConfig["sensorCCS811"]["active"] == 1) {
    setup_ccs811();
  }
  if(jConfig["ds18b20"]["active"] == 1) {
    setup_ds18b20();
  }
}

void readSensors() {
  if(jConfig["battery"] == 1) {
    read_battery(&sensordata);
  }
  
  if(jConfig["SensorBosch"]["active"] == 1) {
    readbosch(&sensordata);
  }

  if(jConfig["SensorBH1750"]["active"] == 1) {
    readBH1750(&sensordata);
  }

  if(jConfig["sensorCCS811"]["active"] == 1) {
    ccs811_setEnvironment_values(jConfig["sensorCCS811"]["environment_values_from"], &sensordata);
    readccs811(&sensordata);
  }
  if(jConfig["ds18b20"]["active"] == 1) {
    read_ds18b20(&sensordata);
  }

  if(jConfig["SensorMHZ19"]["active"] == 1) {
    mhz.readCO2();
  }
}

void setup() {
  // Standard ini to get debug messages
  Serial.begin(115200);
  Serial.println();
  /////
  //check if config mode should be loaded
  pinMode(CONFIG_BUTTON, INPUT_PULLUP);
  delay(10);
  boolean configMode = !digitalRead(CONFIG_BUTTON);
  /////////////////////////////////////////////////
  //Detect Reset mode: Wakeup from Deepsleep or double Reset
  rst_info *resetInfo;
  resetInfo = ESP.getResetInfoPtr();
  boolean doublereset = resetInfo->reason==6?true:false;

  //Init the System and read the sensors
  initSystem();
  if(jConfig["SYSTEM_DONT_CHANGE"]["firstboot"] == 1) {
    Serial.println("Freshly flashed Firmeware detected");
    Serial.println("Open AP for Configuration");
    jConfig["SYSTEM_DONT_CHANGE"]["firstboot"] = 0;
    updateConfig();
    configMode = true;
  }
  //Serial.println(jConfig["oled"]["show_on_double_reset"].as<bool>());
  //if(jConfig["oled"]["show_on_double_reset"].as<bool>() == true && doublereset == true) 

  boolean showDataonOled = !(jConfig["oled"]["show_on_double_reset"].as<bool>() ^ doublereset);
  Serial.println(showDataonOled);
  if(jConfig["oled"]["active"] == 1) {
    if(!configMode) {
      if(showDataonOled) {simplemessage("Sensoren\nauslesen",true,2);}
    }
    else {
      Serial.println("Show config!");
      printmessage("Config Mode", 2000, true, 2);
    }
  }

  readSensors();

  //The mhz needs some time (4 ms)
  if(jConfig["SensorMHZ19"]["active"] == 1) {
    while(mhz.status() == MHZ19_SAMPLING) {
      //Serial.print(".");
      yield();
    }
    if(mhz.status() == MHZ19_READY) {
      sensordata.mhz19_ppm = mhz.getVal();
      StaticJsonDocument<200> mhz19;
      mhz19["CO2"] = sensordata.mhz19_ppm;
      networkdata["mhz19"] = mhz19;
    }
  }

  //////////////////////////////////////
  
  if(!configMode && jConfig["powermode"] == "sleep") {
    String networkstring;
    JSONdata(&sensordata);
    serializeJson(networkdata, networkstring);
    if(jConfig["oled"]["active"] == 1 && showDataonOled) {
      oleddata(&sensordata);
    }
    if(jConfig["oled"]["active"] == 1 && jConfig["oled"]["always_on"]["active"].as<bool>() == true) {
    alwaysOnDisplay();
    }
    if(jConfig["network"]["active"] == 1) {
      while(WiFi.status() != WL_CONNECTED) {
        yield();
        delay(10);
        if(millis() - startWIFI > WLANTIMEOUT) {
          if(jConfig["oled"]["active"] == 1) {
            if(oled_active) {
              printmessage("Wifi\nfailed", 2000, true,2);
            }
          } 
          ESP.deepSleep(jConfig["SleepTime"].as<unsigned long>() * 1000000); 
        }
        
      }
      if(jConfig["mqtt"]["active"] == 1 ) {
        setup_mqtt();
        if(connect_mqtt()) {
          client.loop();
          send_mqtt(jConfig["mqtt"]["Data_channel"].as<char*>(), networkstring);
          delay(10); //Wait for data to be send out.
        }
        else {
          if(jConfig["oled"]["active"] == 1) {
            if(oled_active) {
              printmessage("MQTT\nfailed", 2000, true, 2);
            }
            Serial.println("MIST");  
          }
          else {
            Serial.println("Could not connect to MQTT-Server");
          }
        }
      }
        if(jConfig["udp"]["active"] == 1) {
          Serial.println("Sending UDP Packet");
          IPAddress broadcastIP;
          broadcastIP.fromString(jConfig["udp"]["udp_broadcast_ip"].as<char*>());
          udp.beginPacketMulticast(broadcastIP, jConfig["udp"]["udp_port"].as<unsigned short>(), WiFi.localIP());
          char udp_msg[1024];
          udp.write(udp_msg, sizeof(udp_msg));
          udp.endPacket();
          yield();
          delay(1000);
        }     
      ESP.deepSleep(jConfig["SleepTime"].as<unsigned long>() * 1000000);
    } 
  }
  else  {
    Serial.println("Config Mode enabled");
    char message[64];
    if(jConfig["network"]["active"] == 1) {
       while(WiFi.status() != WL_CONNECTED && millis() - startWIFI < WLANTIMEOUT) {
        yield();
        delay(10);
       }
    }
    if(WiFi.status() != WL_CONNECTED || jConfig["network"]["active"] != 1) {
      Serial.println("open AP");
      openAP(); //Open AP and setup webserver do config
      Serial.println("AP open");
      snprintf(message, sizeof(message), "Im Browser\n%s\nöffnen",  WiFi.hostname().c_str());
    }
    initWeb();
    Serial.println(WiFi.localIP());    
    if(jConfig["oled"]["active"] == 1) {
      Serial.println(message);
      //simplemessage(message, false, 1);
    }
   }
}

long nextreading;
long next_transmit;
void loop() {
  web.handleClient();
  if(mhz.status() == MHZ19_READY) {
      sensordata.mhz19_ppm = mhz.getVal();
      nextreading = millis() + 10000;
  }
  if(millis() > nextreading && mhz.status() == MHZ19_STANDBY) {
    readSensors();
  }
}
