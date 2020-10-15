void read_battery(sensors *data) {
  int raw = analogRead(A0);
  float val =raw/1023;
  val =BATTERY_FULL*val;
  data->battery = val;
  
}
