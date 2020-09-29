#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "Adafruit_BME680.h"
Adafruit_BME280 bme; // I2C
Adafruit_BME680 bme680;
boolean bosch_active;


void setupbosch() {
  bool status;
  if (jConfig["SensorBosch"]["type"].as<String>() == "bme280") {
    status = bme.begin(0x76);
    if (!status) {
      bosch_active = false;
      Serial.println("BME280 failure");
      Serial.println("No data will be send");
   }
    else {
      bosch_active = true;
    }
  }
  if(jConfig["SensorBosch"]["type"].as<String>() == "bme680") {
    status = bme680.begin();
    if (!status) {
      bosch_active = false;
      Serial.println("BME680 failure");
    }
    else {
      bosch_active = true;
      bme680.setTemperatureOversampling(BME680_OS_8X);
      bme680.setHumidityOversampling(BME680_OS_2X);
      bme680.setPressureOversampling(BME680_OS_4X);
      bme680.setIIRFilterSize(BME680_FILTER_SIZE_3);
      bme680.setGasHeater(320, 150); // 320*C for 150 ms
    }
  }
}

float calcSeaLevel(float temp, float pres) {
  double tK = temp + 273.15;
  double exponent = 0.028964 * 9.80665 * (float)jConfig["altitude"];
  exponent = exponent / (8.31447 * tK);
  double hightfactor = pow(2.71828, exponent);
  return pres * hightfactor;
}

void readbosch(sensors *data) {
  if(jConfig["SensorBosch"]["type"].as<String>() == "bme280") {
    data->bosch_temp = bme.readTemperature();
    data->bosch_temp -= (float)jConfig["SensorBosch"]["temp_offset"];
    data->bosch_pres = bme.readPressure();
    data->bosch_pres_sea = calcSeaLevel(data->bosch_temp, data->bosch_pres);
    data->bosch_hum = bme.readHumidity();
  }
  if(jConfig["SensorBosch"]["type"].as<String>() == "bme680") {
    bme680.performReading();
    data->bosch_temp = bme680.temperature;
    data->bosch_temp -= (float)jConfig["SensorBosch"]["temp_offset"];
    data->bosch_pres = bme680.pressure;
    data->bosch_pres_sea = calcSeaLevel(data->bosch_temp, data->bosch_pres);
    data->bosch_hum = bme680.humidity;
    data->bosch_gas = bme680.gas_resistance;
  }

  StaticJsonDocument<200> SensorBosch;
  SensorBosch["name"] = jConfig["SensorBosch"]["name"];
  SensorBosch["temp"] = data->bosch_temp;
  SensorBosch["hum"] = data->bosch_hum;
  SensorBosch["pres"] = data->bosch_pres;
  SensorBosch["pres_sea"] = data->bosch_pres_sea;
  if(jConfig["SensorBosch"]["type"].as<String>() == "bme680") {
    SensorBosch["gas"] = data->bosch_gas;
  }
  SensorBosch["fail_state"] = !bosch_active;
  networkdata["bosch"] = SensorBosch;
}
