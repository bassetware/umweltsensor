#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "icons.h"
#include "Fonts/FreeSans18pt7b.h"
#include "Fonts/FreeSans12pt7b.h"
#include "Fonts/FreeSans9pt7b.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
boolean oled_active;

void setupSSD1306() {
  boolean status = display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  if(!status) {
    oled_active = false;
    Serial.println("OLED failure");
  }
  else {
    oled_active = true;
    display.clearDisplay();
  }
}

void showValue(float data, const uint8_t *picture) {
  display.setFont(&FreeSans18pt7b);
  display.setTextColor(SSD1306_WHITE);
  int16_t  x1, y1;
  uint16_t w, h;
  char value[12];
  snprintf(value, sizeof(value), "%.1f", data);
  display.getTextBounds(value, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH / 2) - (w / 2), 0 + h);
  display.print(value);
  display.drawBitmap(0, 32, picture, 128, 32, 1);
  display.display(); 
}

void alwaysOnDisplay() {
    if(jConfig["oled"]["always_on"]["value"].as<String>() == "temp") {
      showValue(networkdata[jConfig["oled"]["always_on"]["sensor"].as<char*>()][jConfig["oled"]["always_on"]["value"].as<char*>()], thermo_unit);
    }
    if(jConfig["oled"]["always_on"]["value"].as<String>() == "hum") {
      showValue(networkdata[jConfig["oled"]["always_on"]["sensor"].as<char*>()][jConfig["oled"]["always_on"]["value"].as<char*>()], drops_unit);
    }
    if(jConfig["oled"]["always_on"]["value"].as<String>() == "pres") {
      showValue(networkdata[jConfig["oled"]["always_on"]["sensor"].as<char*>()][jConfig["oled"]["always_on"]["value"].as<char*>()], baro_unit);
    }
    if(jConfig["oled"]["always_on"]["value"].as<String>() == "pres_sea") {
      showValue(networkdata[jConfig["oled"]["always_on"]["sensor"].as<char*>()][jConfig["oled"]["always_on"]["value"].as<char*>()], baro_unit);
    }
    if(jConfig["oled"]["always_on"]["value"].as<String>() == "CO2") {
      showValue(networkdata[jConfig["oled"]["always_on"]["sensor"].as<char*>()][jConfig["oled"]["always_on"]["value"].as<char*>()], co2_unit);
    }
    if(jConfig["oled"]["always_on"]["value"].as<String>() == "light") {
      showValue(networkdata[jConfig["oled"]["always_on"]["sensor"].as<char*>()][jConfig["oled"]["always_on"]["value"].as<char*>()], candle_unit);
    }
}

void oleddata(sensors *data) {
  int16_t  x1, y1;
  uint16_t w, h;
  char value[12];
  display.setFont(&FreeSans18pt7b);
  display.setTextColor(SSD1306_WHITE);
  display.clearDisplay();
  display.display();

if(jConfig["SensorBosch"]["active"] == 1) {
  if(jConfig["oled"]["show_names"].as<bool>() == true) {
    display.getTextBounds(jConfig["SensorBosch"]["name"].as<char*>(), 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH / 2) - (w / 2), (SCREEN_HEIGHT / 2) + (h / 2));
    display.print(jConfig["SensorBosch"]["name"].as<char*>());
    display.display();
    delay(jConfig["oled"]["showtime"].as<long>());
  }
  showValue(data->bosch_temp, thermo_unit);
  delay(jConfig["oled"]["showtime"].as<long>());

  display.clearDisplay();
  display.display();
  showValue(data->bosch_pres_sea/100, baro_unit);
  delay(jConfig["oled"]["showtime"].as<long>());

  display.clearDisplay();
  display.display();
  showValue(data->bosch_hum, drops_unit);
  delay(jConfig["oled"]["showtime"].as<long>());
}


if(jConfig["SensorMHZ19"]["active"] == 1) {
  display.clearDisplay();
  display.display();
  if(jConfig["oled"]["show_names"].as<bool>() == true) {
    display.getTextBounds(jConfig["SensorMHZ19"]["name"].as<char*>(), 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH / 2) - (w / 2), (SCREEN_HEIGHT / 2) + (h / 2));
    display.print(jConfig["SensorMHZ19"]["name"].as<char*>());
    display.display();
    delay(jConfig["oled"]["showtime"].as<long>());
  }
  showValue(data->mhz19_ppm, co2_unit);
  delay(jConfig["oled"]["showtime"].as<long>()); 
}
if(jConfig["SensorBH1750"]["active"] == 1) {
  display.clearDisplay();
  display.display();
  if(jConfig["oled"]["show_names"].as<bool>() == true) {
    display.getTextBounds(jConfig["SensorBH1750"]["name"].as<char*>(), 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH / 2) - (w / 2), (SCREEN_HEIGHT / 2) + (h / 2));
    display.print(jConfig["SensorBH1750"]["name"].as<char*>());
    display.display();
    delay(jConfig["oled"]["showtime"].as<long>());
  }
  showValue(data->bh1750_light, candle_unit);
  delay(jConfig["oled"]["showtime"].as<long>()); 
}
if(jConfig["battery"] == 1) {
  display.setFont(&FreeSans18pt7b);
  display.clearDisplay();
  display.display();
  if(jConfig["oled"]["show_names"].as<bool>() == true) {
    display.getTextBounds("Spannung", 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH / 2) - (w / 2), (SCREEN_HEIGHT / 2) + (h / 2));
    display.print("Spannung");
    display.display();
    delay(jConfig["oled"]["showtime"].as<long>());
  }
  snprintf(value, sizeof(value), "%.2f", data->battery);
  display.getTextBounds(value, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH / 2) - (w / 2), 0 + h);
  display.print(value);
  display.drawBitmap(0, 32, battery_unit, 128, 32, 1);
  display.fillRect(26, 38, 12, 22, SSD1306_WHITE); //assuming that the battery is full
  float fill = round(22*((data->battery - BATTERY_SHUTDOWN)/(BATTERY_FULL-BATTERY_SHUTDOWN))); //calculate the real fill level
  display.fillRect(26,38,12, 22-fill, SSD1306_BLACK); //blacking out the already used battery  
  display.display();
  delay(jConfig["oled"]["showtime"].as<long>()); 
}
  display.clearDisplay();
  display.display();  
}

void printmessage(char *message, int delay_ms, boolean centered, int fontsize) {
  if(fontsize == 3) {display.setFont(&FreeSans18pt7b);}
  if(fontsize == 2) {display.setFont(&FreeSans12pt7b);}
  if(fontsize == 1) {display.setFont(&FreeSans9pt7b);}
  display.setTextColor(SSD1306_WHITE);
  display.clearDisplay();
  int16_t  x1, y1;
  uint16_t w, h;
  if(!centered) {
    display.getTextBounds(message, 0, 0, &x1, &y1, &w, &h);
    display.setCursor(0,0);
    display.print(message); 
  }
  else {
    uint16_t h_old = h;
    char *token = strtok(message, "\n");
    display.getTextBounds(token, 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH / 2) - (w / 2),h);
    display.print(token);
    h_old = h;
    token = strtok(NULL, "\n");
    while(token != NULL) {
      display.getTextBounds(token, 0, 0, &x1, &y1, &w, &h);
      display.setCursor((SCREEN_WIDTH / 2) - (w / 2),h_old+h+2);
      display.print(token);
      h_old = h_old + h + 2;
      token = strtok(NULL, "\n");
    }
  }
  long stop_message = millis() + delay_ms;
  while(stop_message > millis()) {
    yield();
  }
  display.clearDisplay();
  display.display();
}

void simplemessage(char *message, boolean centered,int fontsize) {
  if(fontsize == 3) {display.setFont(&FreeSans18pt7b);}
  if(fontsize == 2) {display.setFont(&FreeSans12pt7b);}
  if(fontsize == 1) {display.setFont(&FreeSans9pt7b);}
  display.setTextColor(SSD1306_WHITE);
  display.clearDisplay();
  int16_t  x1, y1;
  uint16_t w, h;
  if(!centered) {
    display.getTextBounds(message, 0, 0, 0, 0, &w, &h);
    display.setCursor(0,0);
    display.print(message); 
  }
  else {
    uint16_t h_old;
    char *token = strtok(message, "\n");
    display.getTextBounds(token, 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH / 2) - (w / 2),h);
    display.print(token);
    h_old = h;
    token = strtok(NULL, "\n");
    while(token != NULL) {
      display.getTextBounds(token, 0, 0, &x1, &y1, &w, &h);
      display.setCursor((SCREEN_WIDTH / 2) - (w / 2),h_old + h+ 2);
      display.print(token);
      h_old = h_old + h + 2;
      token = strtok(NULL, "\n");
    }
  }
  display.display();
}
