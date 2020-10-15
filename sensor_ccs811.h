#include "Adafruit_CCS811.h"

Adafruit_CCS811 ccs;
boolean ccs811_active;

void setup_ccs811() {
  ccs811_active = true;
  if(!ccs.begin()){
    ccs811_active = false;
    Serial.println("ccs811 failure");
    Serial.println("No data will be send");
    return;
  }
  
}

void ccs811_setEnvironment_values(String sensor, sensors *data) {
  if(sensor == "bosch") {
    ccs.setEnvironmentalData(int(data->bosch_hum), data->bosch_temp);
    return;
  }
  if(sensor == "std") {
    ccs.setEnvironmentalData(jConfig["sensorCCS811"]["std_hum"].as<unsigned char>(), jConfig["sensorCCS811"]["std_temp"].as<double>());    
  }
  
}

void readccs811(sensors *data) {
  if(ccs811_active == true) {
    while(!ccs.available()) {
      yield();
    }
    if(!ccs.readData()){
      data->ccs811_co2 = ccs.geteCO2();
      data->ccs811_tvoc = ccs.getTVOC();
    }
    else {
      data->ccs811_co2 = 0;
      data->ccs811_tvoc = 0;
    }
  }
}
