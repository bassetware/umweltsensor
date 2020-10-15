#include <OneWire.h>
#include <DallasTemperature.h>
#define TEMPERATURE_PRECISION 12 //give me all you can do!

OneWire oneWire(DATA_PIN);
DallasTemperature ds18b20(&oneWire);
DeviceAddress ds18b20_adr;

//Not daisychaining
void setup_ds18b20() {
    ds18b20.begin();
    ds18b20.getAddress(ds18b20_adr,0);
    if(ds18b20.getResolution(ds18b20_adr) != 12) {
        ds18b20.setResolution(ds18b20_adr, 12);
    }
}

void read_ds18b20(sensors *data) {
    ds18b20.requestTemperatures();
    data->ds18b20_temp = ds18b20.getTempC(ds18b20_adr);
    data->ds18b20_temp -= jConfig["ds18b20"]["temp_offset"].as<float>();
}
