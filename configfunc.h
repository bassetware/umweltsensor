//Configuration
#define CONF_VERSION "2.2.4"

#include <FS.h>
//#include "LittleFS.h"
StaticJsonDocument<2048> jConfig;
StaticJsonDocument<2048> doc_new;
// Config

void updateConfig() {
  Serial.print("Config Update to Version ");
  Serial.println(CONF_VERSION);
  SPIFFS.remove(CONFIG_PATH);
  File configFile = SPIFFS.open(CONFIG_PATH, "w");
  if (!configFile) {
    Serial.println(F("Failed to create file"));
    return;
  }
  
  
  doc_new["name"] = jConfig["name"] | AP_NAME;
  doc_new["altitude"] = jConfig["altitude"] | 0;
  doc_new["SleepTime"] = jConfig["SleepTime"] | SLEEPTIME;
  doc_new["readTime"] = jConfig["readTime"] | 10;
  doc_new["powermode"] = jConfig["powermode"] | "sleep";
  doc_new["battery"] = jConfig["battery"] | 0;
  
  DynamicJsonDocument doc_network(200);
  doc_network["active"] = jConfig["network"]["active"] | 0;
  doc_network["SSID"] = jConfig["network"]["SSID"] | "";
  doc_network["PSK"] = jConfig["network"]["PSK"] | "";
  doc_new["network"] = doc_network;

  DynamicJsonDocument doc_udp(200);
  doc_udp["active"] = jConfig["udp"]["active"] | 0;
  doc_udp["udp_port"] = jConfig["udp"]["udp_port"] | 9000;
  doc_udp["udp_broadcast_ip"] = jConfig["udp"]["udp_broadcast_ip"] | "0.0.0.0";
  doc_new["udp"] = doc_udp;

  DynamicJsonDocument doc_oled(200);
  doc_oled["active"] = jConfig["oled"]["active"] | 0;
  doc_oled["showtime"] = jConfig["oled"]["showtime"] | SHOWTIME;
  doc_oled["show_names"] = jConfig["oled"]["show_names"] | false;
  doc_oled["show_on_double_reset"] = jConfig["oled"]["show_on_double_reset"] | true;
  DynamicJsonDocument doc_oled_always_on(100);
  doc_oled_always_on["active"] = jConfig["oled"]["always_on"]["active"] | false;
  doc_oled_always_on["sensor"] = jConfig["oled"]["always_on"]["sensor"] | "";
  doc_oled_always_on["value"] = jConfig["oled"]["always_on"]["value"] | "";
  doc_oled["always_on"] = doc_oled_always_on;
  doc_new["oled"] = doc_oled;

  DynamicJsonDocument doc_mqtt(200);
  doc_mqtt["active"] = jConfig["mqtt"]["active"] | 0;
  doc_mqtt["Server"] = jConfig["mqtt"]["Server"] | "";
  doc_mqtt["Port"] = jConfig["mqtt"]["Port"] | 1883;
  doc_mqtt["Clientname"] = jConfig["mqtt"]["Clientname"] | SENSORNAME;
  doc_mqtt["Username"] = jConfig["mqtt"]["Username"] | "";
  doc_mqtt["Password"] = jConfig["mqtt"]["Password"] | "";
  doc_mqtt["Data_channel"] = jConfig["mqtt"]["Data_channel"] | "sensor/"SENSORNAME"/data";
  doc_new["mqtt"] = doc_mqtt;

  DynamicJsonDocument doc_sensorBosch(200);
  doc_sensorBosch["active"] = jConfig["SensorBosch"]["active"] | 0;
  doc_sensorBosch["name"] = jConfig["SensorBosch"]["name"] | SENSOR_BOSCH;
  doc_sensorBosch["type"] = jConfig["SensorBosch"]["type"] | SENSOR_BOSCH;
  doc_sensorBosch["temp_offset"] = jConfig["SensorBosch"]["temp_offset"] | 0;
  //doc_sensorBosch["bme680_gas_100"] = jConfig["SensorBosch"]["bme680_gas_100"] | 0; //Maximum resistance value measured at clean air.
  doc_sensorBosch["bme680_gas_zeroscore"] = jConfig["SensorBosch"]["bme680_gas_zeroscore"] | 0.2; //factor for zero score value
  doc_sensorBosch["bme680_hum_100"] = jConfig["SensorBosch"]["bme680_hum_100"] | 55; //optimal humidity
  doc_sensorBosch["bme680_temp_100"] = jConfig["SensorBosch"]["bme680_temp_100"] | 21; //optimal temperature

  doc_new["SensorBosch"] = doc_sensorBosch;
  
  DynamicJsonDocument doc_mhz19(150);
  doc_mhz19["active"] = jConfig["SensorMHZ19"]["active"] | 0;
  doc_mhz19["name"] = jConfig["SensorMHZ19"]["name"] | SENSOR_MHZ19;
  doc_mhz19["mhz19_offset"] = jConfig["SensorMHZ19"]["mhz19_offset"] | 0;
  doc_new["SensorMHZ19"] = doc_mhz19;

  DynamicJsonDocument doc_sensorccs811(150);
  doc_sensorccs811["active"] = jConfig["sensorCCS811"]["active"] | 0;
  doc_sensorccs811["name"] = jConfig["sensorCCS811"]["name"] | SENSOR_CCS811;
  doc_sensorccs811["environment_values_from"] = jConfig["sensorCCS811"]["environment_values_from"] | STD_SENSOR;
  doc_sensorccs811["std_temp"] = jConfig["sensorCCS811"]["std_temp"] | STD_TEMP;
  doc_sensorccs811["std_hum"] = jConfig["sensorCCS811"]["std_hum"] | STD_HUM;
  doc_new["sensorCCS811"] = doc_sensorccs811;

  DynamicJsonDocument doc_bh1750(100);
  doc_bh1750["active"] = jConfig["SensorBH1750"]["active"] | 0;
  doc_bh1750["name"] = jConfig["SensorBH1750"]["name"] | SENSOR_BH1750;
  doc_new["SensorBH1750"] = doc_bh1750; 

  DynamicJsonDocument doc_ds18b20(100);
  doc_ds18b20["active"] = jConfig["ds18b20"]["active"] | 0;
  doc_ds18b20["name"] = jConfig["ds18b20"]["name"] | SENSOR_DS18B20;
  doc_ds18b20["temp_offset"] = jConfig["ds18b20"]["temp_offset"] | 0;
  doc_new["ds18b20"] = doc_ds18b20; 

  DynamicJsonDocument doc_dontchange(100);
  doc_dontchange["version"] = CONF_VERSION;
  doc_dontchange["firstboot"] = jConfig["SYSTEM_DONT_CHANGE"]["firstboot"] | 1;
  doc_new["SYSTEM_DONT_CHANGE"] = doc_dontchange;
  serializeJsonPretty(doc_new, Serial);
  
  if (!serializeJson(doc_new, configFile)) {
    Serial.println(F("Failed to write to file"));
  }
  configFile.close();
  //jConfig = doc_new;
  Serial.println("Update Done");
  ESP.restart();
}
int loadConfig() {
  int errcode = 0;
  File configFile = SPIFFS.open(CONFIG_PATH, "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    Serial.println("Using Standard values");
    errcode = 1;
  }
  else {
    DeserializationError error = deserializeJson(jConfig, configFile);
    Serial.println("");
    Serial.println(error.c_str());
    Serial.println("config found: ");
    serializeJsonPretty(jConfig, Serial);
    Serial.println();
    Serial.println();
  
    if(error) {
      Serial.println(F("Failed to read file, using default configuration"));
      Serial.println("Using Standard values");
      errcode = 2;
    }
    //read out the config.
    if(jConfig["SYSTEM_DONT_CHANGE"]["version"] != CONF_VERSION) {
      Serial.println("Wrong config version found.");
      Serial.println("updating config");
      errcode = 3;
    }
  }
  //
  configFile.close();

  if(errcode > 0) {
    //saveConfig();
    if(errcode == 1) {
      Serial.println("Format FS");
      SPIFFS.format();
      SPIFFS.end();
      SPIFFS.begin();
    }
    updateConfig();
  }
  
  return errcode;
}
