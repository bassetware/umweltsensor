# Umweltsensor
A Programm to observe environment data. 
It can be integrated to Homeassistant as a sensor. 

## Data Structures
This sketch uses JSON to store the configuration and to send data.
The configuration is stored inside the SPIFF of the ESP8266. It can be edited when entering the config mode.

## Usage
### Requirenments
1. You'll need the Arduino IDE with ESP8266 support installed
2. The following libs have to be installed:
 - ArduinoJson
 - BH1750 [on Github](https://github.com/claws/BH1750)
