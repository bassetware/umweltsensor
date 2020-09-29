#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>
WiFiUDP udp;

WiFiClient espClient;
PubSubClient client(espClient);

#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
long startWIFI;


const char PTYPE_HTML[] = "text/html";
const char PTYPE_PLAIN[] = "text/plain";
const char PTYPE_CSS[] = "text/css";
ESP8266WebServer web(HTTP_PORT);
ESP8266HTTPUpdateServer httpUpdater;
#include "html/page_header.h"

unsigned char h2int(char c) {
    if (c >= '0' && c <='9'){
        return((unsigned char)c - '0');
    }
    if (c >= 'a' && c <='f'){
        return((unsigned char)c - 'a' + 10);
    }
    if (c >= 'A' && c <='F'){
        return((unsigned char)c - 'A' + 10);
    }
    return(0);
}


String urldecode(String str) {
    String encodedString="";
    char c;
    char code0;
    char code1;
    for (int i =0; i < str.length(); i++){
        c=str.charAt(i);
      if (c == '+'){
        encodedString+=' ';  
      }else if (c == '%') {
        i++;
        code0=str.charAt(i);
        i++;
        code1=str.charAt(i);
        c = (h2int(code0) << 4) | h2int(code1);
        encodedString+=c;
      } else{
        
        encodedString+=c;  
      }
      
      yield();
    } 
   return encodedString;
}

void sendPage(const char *data, int count, const char *type) {
  yield();
  int szHeader = sizeof(PAGE_HEADER);
  char *buffer = (char*)malloc(count + szHeader);
  if (buffer) {
    memcpy_P(buffer, PAGE_HEADER, szHeader);
    memcpy_P(buffer + szHeader - 1, data, count);   /* back up over the null byte from the header string */
    web.send(200, type, buffer);
    free(buffer);
  } else {
    Serial.print(F("*** Malloc failed for "));
    Serial.print(count);
    Serial.println(F(" bytes in sendPage() ***"));
  }
}

#include "html/page_hello.h"
#include "html/page_data.h"
#include "html/page_config2.h"

void initWeb() {
  web.on("/", send_hello_html);
//
  web.on("/data.html", send_data_html);
  web.on("/scripts/data", send_data_vals);
//  web.on("/scripts/config", send_config_vals);
//
  web.on("/config.html", send_config_html);
//  web.on("/scripts/loadconfig", send_config_data);
//
//  web.on("/mode.html", send_mode_html);
//  web.on("/scripts/mode", send_mode_vals);
//
//  web.on("/mqtt.html", send_mqtt_html);
//  web.on("/scripts/mqtt", send_mqtt_vals);
  
  web.on("/reboot", []() {
    send_hello_html();
    ESP.restart();
  });

  web.onNotFound([]() {
    web.send(404, PTYPE_HTML, "Page not Found");
  });
  httpUpdater.setup(&web);
  web.begin();
}

void openAP() {
  Serial.print("Setting soft-AP ... ");
  WiFi.hostname(jConfig["name"].as<char*>());
  boolean result = WiFi.softAP(AP_NAME);
  
}

void setupWifi() {
  startWIFI = millis();
  WiFi.hostname(jConfig["name"].as<char*>()); 
  WiFi.begin(jConfig["network"]["SSID"].as<char*>(), jConfig["network"]["PSK"].as<char*>());
  if(jConfig["upd"]["active"] == 1) {
    udp.begin(jConfig["udp"]["udp_port"]);
  }
}

boolean WiFi_isTimeout() {
  return (millis() - startWIFI > WLANTIMEOUT);
}

boolean connectWifi() {
  Serial.println();
  WiFi.hostname("umweltsensor");
  Serial.print("Connecting to ");
  Serial.println(jConfig["SSID"].as<char*>());
  long wait = 0;

  WiFi.begin(jConfig["SSID"].as<char*>(), jConfig["PSK"].as<char*>());
  while (WiFi.status() != WL_CONNECTED) {
    delay(1);
    //Serial.print(".");
    wait++;
    if(wait > WLANTIMEOUT) {
      return false;
    }
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  return true;
}

boolean connect_mqtt() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(jConfig["mqtt"]["Clientname"].as<char*>(), jConfig["mqtt"]["Username"].as<char*>(), jConfig["mqtt"]["Password"].as<char*>())) {
      //if (client.connect(mqttClientname_t, mqttUsername_t, mqttPassword_t)) {
      Serial.println("connected");
      return true;
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      // Wait 5 seconds before retrying
      return false;
    }
  }
}

boolean check_mqtt() {
  if(!client.connected()) {
    return connect_mqtt();
  }
  else {
    return true;
  }
}

void setup_mqtt() {
  client.setServer(jConfig["mqtt"]["Server"].as<char*>(), (int)jConfig["mqtt"]["Port"]);
  //client.setServer("192.168.178.2", (int)jConfig["mqtt"]["Port"]);
}

void send_mqtt(const char *channel, String data) {
  client.publish((char*) channel,(char*) data.c_str());
}
  
