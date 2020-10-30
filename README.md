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
 
 ### IAQ-calculation (Indoor air quality)
 IAQ Calculation is modified from the @G6EJD [(Sample Code)](https://github.com/G6EJD/BME680-Example).
 The IAQ is basically a sccore from 0 - 100, where 100 is the beste score. 
 Every value used for the IAQ calculation has an optimum value. These optimum values can be changed inside the configuration. (Bosch section) A weighted average value is calculated from these score. The weightings are: 10 Percent for the temperature, 20 percent for the humidity and 70 percent for the gas value. 
 ```
    "bme680_gas_zeroscore": 0.2,
    "bme680_hum_100": 45,
    "bme680_temp_100": 21
 ```
 Standard values are shown obove. A 
 The humidity has a range of 5 percent over and under the optimal value where the score is 100.
 Same applies to the temperature, where the range is 1 °C over and under the optimal value. 
 Thee gas-sensor is a little bit difficult to calculate, as the the sensor deliviers different values depending on almost everything. :grimacing: Hence the intervall between measurments and the settings inside the sensor have to be held constant. 
 Basically  a resistance is read out. This resistance will increase if the air is less polluted. To get a score, the sensor reads out the resistance and it to a maximum resistance level found prior to the measurment (clean air value). `"bme680_gas_zeroscore"` provides a facotor from the maximum resistance level, where the gas scoore becomes zero, when the actual resistance level gets below that value.
 If the actial resistancce level gets higher than the prior found maximum, it is stored as the new maximum.
 To get proppoer values I suggest the following procedure:
 1. Open configuration interface
 2. Set your measurement parameters inside the configuration
 3. Go to data section and reset the Clean air value 
 4. Reboot your device and put it in a clean air environment (Basically I put it outside)
 5. Wait for 20 Minutes to let the sensor find the maximum value.
 6. Now your device should be able to determin a propper IAQ value. 
