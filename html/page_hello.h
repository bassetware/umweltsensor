#ifndef PAGE_HELLO_H
#define PAGE_HELLO_H

const char PAGE_HELLO_MHZ[] PROGMEM = R"====(
<a href="/mode.html" style="width:250px" class="btn btn--m btn--blue">MHZ-19 Kalibration</a><br>
)====";

const char PAGE_HELLO_TOP[] PROGMEM = R"=====(<html>
<html>
<head>
</head>
<body>
<strong>Umweltsensor</strong>
<hr>
<a href="/data.html" style="width:250px" class="btn btn--m btn--blue">Aktuelle Daten</a><br>
<a href="/config.html" style="width:250px" class="btn btn--m btn--blue">Konfiguration</a><br>
)=====";

const char PAGE_HELLO_BOTTOM[] PROGMEM = R"=====(
<a href="/update" style="width:250px" class="btn btn--m btn--blue">Firmewareupdate</a><br>
<a href="/info.html" style="width:250px" class="btn btn--m btn--blue">Info</a><br>
<a href="/reboot" style="width:250px" class="btn btn--m btn--blue">Reboot</a><br>
</body>
</html>
)=====";

void send_hello_html() {
  int p_size;
  
  int szHeader = sizeof(PAGE_HEADER);
  p_size = szHeader;
  
  int szHelloTop = sizeof(PAGE_HELLO_TOP);
  p_size += szHelloTop;
  
  int szHelloBotton = sizeof(PAGE_HELLO_BOTTOM);
  p_size += szHelloBotton;
  
  int szMHZ = sizeof(PAGE_HELLO_MHZ);
  p_size += szMHZ;


  int buffer_point = 0;
  char *buffer = (char*)malloc(p_size);
  
  memcpy_P(buffer, PAGE_HEADER, szHeader);
  buffer_point = szHeader - 1;
  
  memcpy_P(buffer + buffer_point, PAGE_HELLO_TOP, szHelloTop);
  buffer_point = buffer_point + szHelloTop - 1;
  
  memcpy_P(buffer + buffer_point, PAGE_HELLO_MHZ, szMHZ);
  buffer_point = buffer_point + szMHZ - 1;
  
  memcpy_P(buffer + buffer_point, PAGE_HELLO_BOTTOM, szHelloBotton);
  
  yield();
  web.send(200, PTYPE_HTML, buffer);
  free(buffer);
}


#endif