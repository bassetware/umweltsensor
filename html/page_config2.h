#ifndef PAGE_CONFIG_H
#define PAGE_CONFIG_H


const char PAGE_CONFIG_TOP[] PROGMEM = R"=====(<html>
	<head>
	</head>
	<body>
		<a href='/' class='btn btn--s'>&lt;</a>&nbsp;&nbsp;<strong>Umweltsensor</strong>
		<hr>
		<Strong>Konfiguration</strong>
		<br>
		<form action="" method="POST">
		<textarea id='config' name='config' cols='70' rows='30'>
)=====";		

const char PAGE_CONFIG_BOTTOM[] PROGMEM = R"=====(
		</textarea>
		<br>
		<input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save">
		</form>
		<hr>
		<script>
		setInterval(GetState, 3000);
		function GetState() {
			setValues('/scripts/data');
		}
		</script>
	</body>
	</html>
)=====";


/*void send_config_html() {
	StaticJsonDocument<2048> doc_html;
	String config;
 	if (web.args()) {  // Save Settings
        for (uint8_t i = 0; i < web.args(); i++) {
			Serial.println(web.argName(i));
            if (web.argName(i) == "plain") {
				Serial.println(urldecode(web.arg(i)));
				char buf[2048];
				urldecode(web.arg(i)).toCharArray(buf, sizeof(buf));
				char *token = strtok(buf, "=");
				token = strtok(NULL, "=");
				DeserializationError error = deserializeJson(doc_html, token);
				//Serial.println(error);
				config = String(token);
				File configFile = SPIFFS.open(CONFIG_PATH, "w");
				if (!serializeJson(doc_html, configFile)) {
					Serial.println(F("Failed to write to file"));
				}
				configFile.close();
				//jConfig = doc_html;
			}
		}
	} 
	File configFile = SPIFFS.open(CONFIG_PATH, "r");
	if (!configFile) {
		Serial.println("Failed to open config file");
		Serial.println("Using Standard values");
	}
	DeserializationError error = deserializeJson(doc_html, configFile);
	yield();
	configFile.close();
	yield();
	serializeJsonPretty(doc_html, config);
	web.setContentLength(sizeof(PAGE_HEADER) + sizeof(PAGE_CONFIG_TOP) + sizeof(PAGE_CONFIG_BOTTOM) + config.length());
	web.send_P(200,PTYPE_HTML, "");
	yield();
	web.sendContent_P(PAGE_HEADER);	
	yield();
	web.sendContent_P(PAGE_CONFIG_TOP);
	web.sendContent(config.c_str()); 
	yield();
	web.sendContent_P(PAGE_CONFIG_BOTTOM); 
}*/
StaticJsonDocument<2048> html_config;
void send_config_html() {
 	if (web.args()) {  // Save Settings
        for (uint8_t i = 0; i < web.args(); i++) {
			Serial.println(web.argName(i));
            if (web.argName(i) == "plain") {
				Serial.println(urldecode(web.arg(i)));
				char buf[2048];
				urldecode(web.arg(i)).toCharArray(buf, sizeof(buf));
				char *token = strtok(buf, "=");
				token = strtok(NULL, "=");
				
				DeserializationError error = deserializeJson(html_config, token);
				//Serial.println(error);
				File configFile = SPIFFS.open(CONFIG_PATH, "w");
				if (!serializeJson(html_config, configFile)) {
					Serial.println(F("Failed to write to file"));
				}
				configFile.close();
			}
		}
	} 
	String config;
	File configFile = SPIFFS.open(CONFIG_PATH, "r");
	if (!configFile) {
		Serial.println("Failed to open config file");
		Serial.println("Using Standard values");
	}
	DeserializationError error = deserializeJson(html_config, configFile);
	configFile.close();	
	serializeJsonPretty(html_config, config);
	Serial.println(config);
	
	web.setContentLength(sizeof(PAGE_HEADER) + sizeof(PAGE_CONFIG_TOP) + sizeof(PAGE_CONFIG_BOTTOM) + config.length());
	web.send_P(200,PTYPE_HTML, "");
	yield();
	web.sendContent_P(PAGE_HEADER);	
	yield();
	web.sendContent_P(PAGE_CONFIG_TOP);
	web.sendContent(config.c_str()); 
	yield();
	web.sendContent_P(PAGE_CONFIG_BOTTOM); 
}

#endif