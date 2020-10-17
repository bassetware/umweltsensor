#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "Adafruit_BME680.h"
Adafruit_BME280 bme; // I2C
Adafruit_BME680 bme680;
boolean bosch_active;
float bme680_maximum_gas;




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
      File maxgas = SPIFFS.open(MAX_GAS_PATH, "r");
      if(!maxgas) { //if file does not exist create a new one
        sensordata.bosch_clean_air = 0;
        SPIFFS.remove(MAX_GAS_PATH);
        maxgas = SPIFFS.open(MAX_GAS_PATH, "w");
        maxgas.print(sensordata.bosch_clean_air);
        maxgas.close();
      }
      else {
          String line = maxgas.readStringUntil('\n');
          sensordata.bosch_clean_air = line.toFloat();
          maxgas.close();


      }
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

void reset_maximum_gas() {
  SPIFFS.remove(MAX_GAS_PATH);
  File maxgas = SPIFFS.open(MAX_GAS_PATH, "w");
  maxgas.print(bme680_maximum_gas);
  maxgas.close();
}

float calcIAQ(float gas, float temp, float hum, float cleanair) {
  //calc the score for the temp
  Serial.println(gas);
  Serial.println(hum);
  Serial.println(temp);
  float humscore;
  float tempscore;
  float gasscore;
  float range;
  if(hum <= (jConfig["SensorBosch"]["bme680_hum_100"].as<float>() + 5) && hum >= (jConfig["SensorBosch"]["bme680_hum_100"].as<float>() - 5)) {
    humscore = 100;
    Serial.println("best")
  }
  else if(hum > (jConfig["SensorBosch"]["bme680_hum_100"].as<float>() + 5)) {
    float hum_offset = hum - (jConfig["SensorBosch"]["bme680_hum_100"].as<float>() + 5);
    range = 100 - (jConfig["SensorBosch"]["bme680_hum_100"].as<float>() + 5);
    float humscore = 100 - ((hum_offset / range)*100);
  }
  else if(hum < (jConfig["SensorBosch"]["bme680_hum_100"].as<float>() - 5)) {
    float hum_offset = hum - (jConfig["SensorBosch"]["bme680_hum_100"].as<float>() - 5);
    range = jConfig["SensorBosch"]["bme680_hum_100"].as<float>() - 5;
    float humscore = (hum_offset / range) * 100;
  }
  Serial.println(humscore);
  //calculating the score of the temperature
  //giving a 1 degree delta
  if(temp <= (jConfig["SensorBosch"]["bme680_temp_100"].as<float>() + 1) && temp >= (jConfig["SensorBosch"]["bme680_temp_100"].as<float>() - 1)) {
    tempscore = 100;
  }
  else if(temp > (jConfig["SensorBosch"]["bme680_temp_100"].as<float>() + 1)) {
    float temp_offset = temp - (jConfig["SensorBosch"]["bme680_temp_100"].as<float>() + 1);
    range = (jConfig["SensorBosch"]["bme680_temp_100"].as<float>() + 15) - (jConfig["SensorBosch"]["bme680_temp_100"].as<float>() + 1);
    float score = 100 - ((temp_offset / range)*100);
  }
  else if(temp < (jConfig["SensorBosch"]["bme680_temp_100"].as<float>() - 1)) {
    float temp_offset = temp - (jConfig["SensorBosch"]["bme680_temp_100"].as<float>() - 1);
    range = (jConfig["SensorBosch"]["bme680_temp_100"].as<float>() - 1) - (jConfig["SensorBosch"]["bme680_temp_100"].as<float>() -15);
    float score = (temp_offset / range) * 100;
  }

  /*Calculating the gas score
  First step check the gas value:
  If the new value is higher than the maximum gas value from the configuration then the gas value for calculation will be the maximum value
  If the new value is over 2 % higher than the maximum value from the configuration than the maximum value will be updated
  */

  if(gas < (cleanair * jConfig["SensorBosch"]["bme680_gas_zeroscore"].as<float>())) {
    gasscore = 0;
  }
  else {
    float gas_offset = gas - (cleanair * jConfig["SensorBosch"]["bme680_gas_zeroscore"].as<float>());
    range = cleanair - (cleanair * jConfig["SensorBosch"]["bme680_gas_zeroscore"].as<float>());
    gasscore = (gas_offset / range) * 100;
  }
    //calculating the overall score
float sumscore = (tempscore) + (humscore*2) + (gasscore * 7);
 sumscore /= 10;
 return sumscore;
}

void readbosch(sensors *data) {
  if(jConfig["SensorBosch"]["type"].as<String>() == "bme280") {
    data->bosch_temp = bme.readTemperature();
    data->bosch_temp += (float)jConfig["SensorBosch"]["temp_offset"];
    data->bosch_pres = bme.readPressure();
    data->bosch_pres_sea = calcSeaLevel(data->bosch_temp, data->bosch_pres);
    data->bosch_hum = bme.readHumidity();
  }
  if(jConfig["SensorBosch"]["type"].as<String>() == "bme680") {
    bme680.performReading();
    data->bosch_temp = bme680.temperature;
    data->bosch_temp += (float)jConfig["SensorBosch"]["temp_offset"];
    data->bosch_pres = bme680.pressure;
    data->bosch_pres_sea = calcSeaLevel(data->bosch_temp, data->bosch_pres);
    data->bosch_hum = bme680.humidity;
    data->bosch_gas = bme680.gas_resistance;
    bme680.performReading();
    data->bosch_gas = bme680.gas_resistance;
    if(data->bosch_gas > data->bosch_clean_air) {   
      if((data->bosch_gas/data->bosch_clean_air) > 1.02) {
        data->bosch_clean_air = data->bosch_gas;
        SPIFFS.remove(MAX_GAS_PATH);
        File maxgas = SPIFFS.open(MAX_GAS_PATH, "w");
        maxgas.print(data->bosch_clean_air);
        maxgas.close();
      }
      else {
        data->bosch_gas = data->bosch_clean_air;
      }
    }
    data->bosch_iaq = calcIAQ(data->bosch_gas, data->bosch_temp, data->bosch_hum, data->bosch_clean_air);
  }
}
