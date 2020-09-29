#include <BH1750.h>
BH1750 lightMeter(0x23);;
boolean bh1750_active;

void setupBH1750() {
  boolean status = lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);
  if(!status) {
    bh1750_active = false;
    Serial.println("BH1750 failure");
    Serial.println("No data will be send");
  }
  else {
    bh1750_active = true;
    //float dump = lightMeter.readLightLevel();
  }
}

void readBH1750(sensors *data) {
  data->bh1750_light = lightMeter.readLightLevel();
  StaticJsonDocument<200> sBH1750;
  sBH1750["light"] = jConfig["bh1750"]["name"];
  sBH1750["light"] = data->bh1750_light;
  networkdata["bh1750"] = sBH1750;
}
