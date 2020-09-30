#ifndef PAGE_INFO_H
#define PAGE_INFO_H

const char PAGE_INFO[] PROGMEM = R"=====(<html>
	<head>
	</head>
	<body>
		<a href='/' class='btn btn--s'>&lt;</a>&nbsp;&nbsp;<strong>Umweltsensor</strong>
		<hr>
		<Strong>Info</strong><br>
		<table border='0' cellspacing='0' cellpadding='3' style='width:420px'>
		<colgroup span=2 width='50%'>
		<tr><td align='right'>Version :</td><td><div id='version'>N/A</div></td></tr>
		</table>
		<hr>
		<script>
			setValues('/scripts/info');
		</script>
	</body>
	</html>
)=====";




void send_info_html() {
	sendPage(PAGE_INFO, sizeof(PAGE_INFO), PTYPE_HTML);
}

void send_info_data() {
	String values = "";
	values += "version|div|" + (String)VERSION + "\n";
	web.send(200, PTYPE_PLAIN, values);
}


#endif