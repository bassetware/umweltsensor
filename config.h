#define SENSORNAME "env_sensor"
#define AP_NAME "Umweltsensor"
#define AP_PW "sensorsensor"
#define HTTP_PORT 80
#define WLANTIMEOUT 20000
#define MQTT_ACTIVE
#define SERIAL_DATA
#define CONFIG_PATH "config/config.cfg"
#define SLEEPTIME 10 //in Seconds
#define CONFIG_BUTTON D7


#define SENSOR_BOSCH "bme280";
#define BOSCH_TEMPERATURE_OFFSET 0    //minus if measured temperature is HIGHER than reference temp, plus if its LOWER
#define ALTITUDE 55.0;

#define SSD1306
#define SHOWTIME 2000;
//#define BATTERY //uncommend if using no battery
#define BATTERY_FULL 4.2
#define BATTERY_SHUTDOWN 3

#define SENSOR_MHZ19 "MH-Z19"
#define CO2 D5
#define CO2LIMIT 5000

#define SENSOR_DS18B20 "DS18B20"
#define DATA_PIN D5

#define SENSOR_CCS811 "CCS811"
#define STD_SENSOR "std"
#define STD_TEMP 25
#define STD_HUM 50

#define SENSOR_BH1750 "BH-1750"
