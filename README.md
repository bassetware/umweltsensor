# Umweltsensor
A Programm to observe environment data. 
It can be integrated to Homeassistant as a sensor. 

## Data Structures
This sketch uses JSON to store the configuration and to send data.
The configuration is stored inside the SPIFF of the ESP8266. It can be edited when entering the config mode.

## Usage
### Requirenments
1. You'll need the Arduino IDE with ESP8266 support installed
2. The following libs are used by the sketch and should be installed:
 - [ArduinoJson](https://arduinojson.org/)
 - [BH1750](https://github.com/claws/BH1750)
 - [PubSubClient](https://github.com/knolleary/pubsubclient)
 - [Adafruit Sensor](https://github.com/adafruit/Adafruit_Sensor)
 - [Adafruit_BME280](https://github.com/adafruit/adafruit_BME280_Library)
 - [Adafruit_BME680](https://github.com/adafruit/Adafruit_BME680)
 - [OneWire](http://www.pjrc.com/teensy/td_libs_OneWire.html)
 - [DallasTemperature](https://github.com/milesburton/Arduino-Temperature-Control-Library)
 - [AdafruitGFX](https://github.com/adafruit/Adafruit-GFX-Library)
 - [Adafruit_SSD1306](https://github.com/adafruit/Adafruit_SSD1306)
 - [Adafruit_CCS811](https://github.com/adafruit/Adafruit_CCS811)
 
 ### Installation
 1. Complile and upload the sketch or
 2. download the binary release and upload it with the [NODEMCU flasher](https://github.com/nodemcu/nodemcu-flasher)

 ### Configuration
 After uploading the sketch an open AP is created by the ESP8266 called "Umweltsensor". Connect to the AP. In your webbrowser open 192.168.4.1 to enter the configuration. If no webserver is open push the config button and reset the ESP again.
 Yout can now configure the JSON file directly. 

 ### Configuration modes
 yet to come.
