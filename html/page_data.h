#ifndef PAGE_DATA_H
#define PAGE_DATA_H

const char PAGE_DATA_MHZ[] PROGMEM = R"====(
<tr><td align='right'>CO<sub>2</sub> :</td><td><div id='mhz19_ppm'>N/A</div></td></tr>
)====";

const char PAGE_DATA_BME280[] PROGMEM = R"====(
		<tr><td align='right'>Temperatur :</td><td><div id='bosch_temp'>N/A</div></td></tr>
		<tr><td align='right'>Luftdruck :</td><td><div id='bosch_pres'>N/A</div></td></tr>
		<tr><td align='right'>Luftfeuchtigkeit :</td><td><div id='bosch_hum'>N/A</div></td></tr>
)====";

const char PAGE_DATA_CCS811[] PROGMEM = R"====(
		<tr><td align='right'>CO<sub>2</sub> :</td><td><div id='ccs811_co2'>N/A</div></td></tr>
		<tr><td align='right'>TVOC :</td><td><div id='ccs811_tvoc'>N/A</div></td></tr>
)====";

const char PAGE_DATA_BH1750[] PROGMEM = R"====(
<tr><td align='right'>Lichtstärke :</td><td><div id='bh1750_light'>N/A</div></td></tr>
)====";
const char PAGE_DATA_TOP[] PROGMEM = R"=====(<html>
	<head>
	</head>
	<body>
		<a href='/' class='btn btn--s'>&lt;</a>&nbsp;&nbsp;<strong>Umweltsensor</strong>
		<hr>
		<Strong>Aktuelle Daten</strong>
		<table border='0' cellspacing='0' cellpadding='3' style='width:420px'>
		<colgroup span=2 width='50%'>
)=====";		

const char PAGE_DATA_BOTTOM[] PROGMEM = R"=====(	
		</table>
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




void send_data_html() {
  int p_size;
  
  int szHeader = sizeof(PAGE_HEADER);
  p_size = szHeader;
  
  int szDataTop = sizeof(PAGE_DATA_TOP);
  p_size += szDataTop;
  
  int szDataBotton = sizeof(PAGE_DATA_BOTTOM);
   p_size += szDataBotton;
  
  if(jConfig["SensorMHZ19"]["active"] == 1) {
	p_size += sizeof(PAGE_DATA_MHZ);
  }
  
  if(jConfig["SensorBosch"]["active"] == 1) {
	p_size += sizeof(PAGE_DATA_BME280);
  }
  
  if(jConfig["SensorBH1750"]["active"] == 1) {
	p_size += sizeof(PAGE_DATA_BH1750);
  }
  if(jConfig["sensorCCS811"]["active"] == 1) {
	p_size += sizeof(PAGE_DATA_CCS811);
  } 
  
  int buffer_point = 0;
  char *buffer = (char*)malloc(p_size);
  
  memcpy_P(buffer, PAGE_HEADER, szHeader);
  buffer_point = szHeader - 1;
  
  memcpy_P(buffer + buffer_point, PAGE_DATA_TOP, szDataTop);
  buffer_point = buffer_point + szDataTop - 1;
  
  if(jConfig["SensorBosch"]["active"] == 1) {
  memcpy_P(buffer + buffer_point, PAGE_DATA_BME280, sizeof(PAGE_DATA_BME280));
  buffer_point = buffer_point + sizeof(PAGE_DATA_BME280) - 1;
  }

  if(jConfig["SensorBH1750"]["active"] == 1) {
  memcpy_P(buffer + buffer_point, PAGE_DATA_BH1750, sizeof(PAGE_DATA_BH1750));
  buffer_point = buffer_point + sizeof(PAGE_DATA_BH1750) - 1;
  }
  
  if(jConfig["SensorMHZ19"]["active"] == 1) {
  memcpy_P(buffer + buffer_point, PAGE_DATA_MHZ, sizeof(PAGE_DATA_MHZ));
  buffer_point = buffer_point + sizeof(PAGE_DATA_MHZ) - 1;
  }
  
  if(jConfig["sensorCCS811"]["active"] == 1) {
  memcpy_P(buffer + buffer_point, PAGE_DATA_CCS811, sizeof(PAGE_DATA_CCS811));
  buffer_point = buffer_point + sizeof(PAGE_DATA_CCS811) - 1;
  }
  
  
  memcpy_P(buffer + buffer_point, PAGE_DATA_BOTTOM, szDataBotton);
  
  yield();
  web.send(200, PTYPE_HTML, buffer);
  free(buffer);
}

void send_data_vals() {
	//Serial.println(sending: data.script);
	String values = "";
	if(jConfig["SensorBosch"]["active"] == 1) {
	values += "bosch_temp|div|" + (String)sensordata.bosch_temp +  " °C\n";
	values += "bosch_pres|div|" + (String)(sensordata.bosch_pres_sea/100) +  " hPa\n";
	values += "bosch_hum|div|" + (String)sensordata.bosch_hum +  " %\n";
	}
	if(jConfig["SensorMHZ19"]["active"] == 1) {
	values += "mhz19_ppm|div|" + (String)sensordata.mhz19_ppm +  " ppm\n";
	}
	if(jConfig["SensorBH1750"]["active"] == 1) {
	values += "bh1750_light|div|" + (String)sensordata.bh1750_light +  " Lx\n";
	}
	if(jConfig["sensorCCS811"]["active"] == 1) {
	values += "ccs811_co2|div|" + (String)sensordata.ccs811_co2 +  " ppm\n";
	values += "ccs811_tvoc|div|" + (String)(sensordata.ccs811_tvoc) +  " ppb\n";
	}
	web.send(200, PTYPE_PLAIN, values);
}

#endif