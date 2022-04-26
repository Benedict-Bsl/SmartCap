#include <SoftwareSerial.h>
#include <TinyGPS.h>

TinyGPS gps;
SoftwareSerial sGps(2,3);
void setup() {
  Serial.begin(115200);
  sGps.begin(9600);
}

void loop() {
  unsigned long age;
  float glat, glon;
  byte month, day, hour, minute, second, hundredths;
  int year;

  for(int i = 0; i < 1000; i++){gps.encode(sGps.read());\   
  gps.f_get_position(&glat, &glon, &age);delay(1);gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);}
  
  char tsmp[32];
  String a="";
  Serial.print("lat: ");Serial.println(glat);Serial.print(" lon: ");Serial.println(glon);
  sprintf(tsmp,"%02d/%02/%02d %02d:%02d:%02d ",month,day,year, hour, minute, second);
  String b = String(month)+"/"+String(day)+"/"+String(year)+" "+String(hour)+":"+String(minute)+":"+String(second);
  Serial.print("timeStamp: ");
//  Serial.println(a);
  Serial.println(b);

}
