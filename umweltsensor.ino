#define VERSION "0.4.0"

//includes
#include "config.h"

#include <Wire.h>
#include <ArduinoJson.h>
#include "configfunc.h"

StaticJsonDocument<2048> networkdata;

//Sensors
typedef struct sensors {
  float bosch_temp;
  float bosch_hum;
  float bosch_pres;
  float bosch_pres_sea;
  float bosch_gas;
  float bosch_clean_air;
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
boolean configMode;


#include "sensor_bme280.h"
#include "sensor_ccs811.h"
#include "oled_ssd1306.h"
#include "mhz19.h"
sensorMHZ19 mhz(CO2,CO2LIMIT);
#include "battery.h"
#include "BH1750.h"
#include "sensor_ds18b20.h"
#include "networkfuncs.h"


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

String readSensors() {
  networkdata.clear();
  networkdata["name"] = jConfig["mqtt"]["Clientname"];
  networkdata["software"] = VERSION;
  if(jConfig["battery"] == 1) {
    read_battery(&sensordata);
    networkdata["battery"] = sensordata.battery;
  }
  
  if(jConfig["SensorBosch"]["active"] == 1) {
    readbosch(&sensordata);
    StaticJsonDocument<200> SensorBosch;
    SensorBosch["name"] = jConfig["SensorBosch"]["name"];
    SensorBosch["temp"] = sensordata.bosch_temp;
    SensorBosch["hum"] = sensordata.bosch_hum;
    SensorBosch["pres"] = sensordata.bosch_pres;
    SensorBosch["pres_sea"] = sensordata.bosch_pres_sea;
    if(jConfig["SensorBosch"]["type"].as<String>() == "bme680") {
      SensorBosch["gas"] = sensordata.bosch_gas;
      SensorBosch["clean_air"] = sensordata.bosch_clean_air;
      SensorBosch["iaq"] = sensordata.bosch_iaq;
    }
    SensorBosch["fail_state"] = !bosch_active;
    networkdata["bosch"] = SensorBosch;
  }

  if(jConfig["SensorBH1750"]["active"] == 1) {
    readBH1750(&sensordata);
    StaticJsonDocument<100> sBH1750;
    sBH1750["name"] = jConfig["SensorBH1750"]["name"];
    sBH1750["light"] = sensordata.bh1750_light;
    networkdata["bh1750"] = sBH1750;
  }

  if(jConfig["sensorCCS811"]["active"] == 1) {
    ccs811_setEnvironment_values(jConfig["sensorCCS811"]["environment_values_from"], &sensordata);
    readccs811(&sensordata);
    StaticJsonDocument<200> sensorccs811;
    sensorccs811["name"] = jConfig["ccs811"]["name"];
    sensorccs811["CO2"] = sensordata.ccs811_co2;
    sensorccs811["TVOC"] = sensordata.ccs811_tvoc;
    sensorccs811["fail_state"] = !ccs811_active;
    networkdata["ccs811"] = sensorccs811;
  }

  if(jConfig["ds18b20"]["active"] == 1) {
    read_ds18b20(&sensordata);
    StaticJsonDocument<100> sensords18b20;
    sensords18b20["name"] = jConfig["ds18b20"]["name"].as<String>();
    sensords18b20["temp"] = sensordata.ds18b20_temp;
    networkdata["ds18b20"] = sensords18b20;
  }

  if(jConfig["SensorMHZ19"]["active"] == 1) {
    mhz.readCO2();
    while(mhz.status() == MHZ19_SAMPLING) {
      yield();
    }
    if(mhz.status() == MHZ19_READY) {
      sensordata.mhz19_ppm = mhz.getVal();
      StaticJsonDocument<100> mhz19;
      mhz19["name"] = jConfig["mhz19"]["name"].as<String>();
      mhz19["CO2"] = sensordata.mhz19_ppm;
      networkdata["mhz19"] = mhz19;
    }
  }
  
  String returnString;
  serializeJson(networkdata, returnString);
  JSONdata(&sensordata);
  return returnString;
}

void setup() {
  // Standard ini to get debug messages
  Serial.begin(115200);
  Serial.println();
  /////
  //check if config mode should be loaded
  pinMode(CONFIG_BUTTON, INPUT_PULLUP);
  delay(10);
  configMode = !digitalRead(CONFIG_BUTTON);
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

  boolean showDataonOled = !(jConfig["oled"]["show_on_double_reset"].as<bool>() ^ doublereset);
  Serial.println(showDataonOled);
  if(jConfig["oled"]["active"] == 1) {
    if(!configMode) {
      if(showDataonOled) {simplemessage("Sensoren\nauslesen",true,2);}
    }
    else {
      Serial.println("Show config!");
      printmessage("Konfiguration\n", 2000, true, 1);
    }
  }
  String networkstring;
  networkstring = readSensors();

  //The mhz needs some time (4 ms)
  if(jConfig["SensorMHZ19"]["active"] == 1) {

  }

  //////////////////////////////////////
  
  if(!configMode && jConfig["powermode"] == "sleep") {
    Serial.println("sleep mode enabled");
    
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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Constant Mode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(!configMode && jConfig["powermode"] == "const") {
    Serial.println("const mode enabeld");
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
              ESP.deepSleep(jConfig["SleepTime"].as<unsigned long>() * 1000000); 
              //Go to sleep and try again
            }
          }  
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
        //initWeb();     
    }     
  }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Configuration
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  if(configMode)  {
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
      snprintf(message, sizeof(message), "%s\n\verbinden\n192.168.4.1\naufrufen", AP_NAME);
    }
    else {
      snprintf(message, sizeof(message), "Im Browser\n%s\naufrufen",  WiFi.localIP().toString().c_str());
    }
    initWeb();
    Serial.println(WiFi.localIP());    
    if(jConfig["oled"]["active"] == 1) {
      Serial.println(message);
      simplemessage(message, false, 1);
    }
    //additional configs is powermode is const
   }
}

long nextreading;
long next_transmit;

void loop() {
  web.handleClient();
  if(mhz.status() == MHZ19_READY) {
      sensordata.mhz19_ppm = mhz.getVal();
  }
  if(millis() > nextreading) {
    String networkstring = readSensors();
    yield();
    //Serial.println("lesen");
    if(jConfig["powermode"].as<String>() == "const" && !configMode) {
      Serial.println("hier mache ich was");
      if(jConfig["network"]["active"] == 1) {
        if(WiFi.status() != WL_CONNECTED) {
          ESP.deepSleep(jConfig["SleepTime"].as<unsigned long>() * 1000000);
          //Go to sleep for the time between two readings. Then automatically try to connect again.
        } 
        yield();
        if(jConfig["mqtt"]["active"] == 1 ) {
          if(!client.connected()) {
            if(connect_mqtt()) {
              send_mqtt(jConfig["mqtt"]["Data_channel"].as<char*>(), networkstring);
              yield();
            }
          }
          else {
            client.loop();
            send_mqtt(jConfig["mqtt"]["Data_channel"].as<char*>(), networkstring);
            yield();
          }
        }
        yield();
        /*if(jConfig["udp"]["active"] == 1) {
          Serial.println("Sending UDP Packet");
          IPAddress broadcastIP;
          broadcastIP.fromString(jConfig["udp"]["udp_broadcast_ip"].as<char*>());
          udp.beginPacketMulticast(broadcastIP, jConfig["udp"]["udp_port"].as<unsigned short>(), WiFi.localIP());
          char udp_msg[1024];
          udp.write(udp_msg, sizeof(udp_msg));
          udp.endPacket();
          yield();
        }*/
      }
    }

    nextreading = millis() + (jConfig["readTime"].as<unsigned long>() * 1000);
  }

}
