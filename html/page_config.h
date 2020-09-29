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
)=====";		

const char PAGE_CONFIG_BOTTOM[] PROGMEM = R"=====(
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




void send_config_html() {
  long p_size;
  
  int szHeader = sizeof(PAGE_HEADER);
  p_size = szHeader;
  
  int szConfigTop = sizeof(PAGE_CONFIG_TOP);
  p_size += szConfigTop;
  
  char config[512];
  File configFile = SPIFFS.open(CONFIG_PATH, "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    Serial.println("Using Standard values");
  }
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, configFile);
  configFile.close();

  serializeJsonPretty(doc, config);
  char config_html[512];
  snprintf(config_html, sizeof(config_html), "<textarea id='config' name='config' cols='100' rows='30'>%s</textarea>", config);
  p_size += sizeof(config_html);
  
  
  
  int szConfigBotton = sizeof(PAGE_CONFIG_BOTTOM);
  p_size += szConfigBotton;
  
  int buffer_point = 0;
  char *buffer = (char*)malloc(p_size);
  
  memcpy_P(buffer, PAGE_HEADER, szHeader);
  buffer_point = szHeader - 1;
  
  memcpy_P(buffer + buffer_point, PAGE_CONFIG_TOP, szConfigTop);
  buffer_point = buffer_point + szConfigTop - 1;
  
  memcpy_P(buffer + buffer_point, config_html, sizeof(config_html));
  buffer_point = buffer_point + sizeof(config_html) - 1;
  
  
  memcpy_P(buffer + buffer_point, PAGE_CONFIG_BOTTOM, szConfigBotton);
  
  yield();
  web.send(200, PTYPE_HTML, buffer);
  free(buffer);
}

#endif