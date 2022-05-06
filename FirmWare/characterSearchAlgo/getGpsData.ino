
void readGPS(){
  a = 1;
  unsigned long age;
  float glat, glon;
  byte month, day, hour, minute, second, hundredths;
  int year;

  for(int i = 0; i < 2000; i++){gps.encode(sGps.read());\   
  gps.f_get_position(&glat, &glon, &age);delay(2);gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);}

//   Serial.print("lat: ");Serial.println(glat);Serial.print(" lon: ");Serial.println(glon);
//   sprintf(tsmp,"%02d/%02/%02d %02d:%02d:%02d ",month,day,year, hour, minute, second);
  // String b = String(month)+"/"+String(day)+"/"+String(year)+" "+String(hour)+":"+String(minute)+":"+String(second);
  // timeStamp = b;
  gps_lat = glat;
  gps_lon = glon;

  // Serial.print("timeStamp: ");
//  Serial.println(timeStamp);
//   Serial.println(b);

}
