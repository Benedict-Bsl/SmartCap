#include <SoftwareSerial.h>
#include <TinyGPS.h>

TinyGPS gps;

SoftwareSerial wifi(10, 9);
SoftwareSerial sGps(10,9);
#define AT_OK "AT"
#define RESTART "AT+RST"
#define RESPONSE "OK"
#define VERSION "AT+GMR"
#define DSLEEP "AT+GSLP"
#define SLEEP "AT+SLEEP="

//wifi Commands
#define MODE  "AT+CWMODE_CUR="
#define SMODE "AT+CWMODE_DEF"
#define CONNECT "AT+CWJAP"
#define SCONNECT "AT+CWJAP_DEF"
#define DISCONNECT "AT+CWQAP"
#define AUTO_CONNECT "AT+CWAUTOCONN"


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
//  initGPS();

//  while(!Serial){
//    
//  }

  wifi.begin(9600);
//  delay(50);
//  wifi.println(RESTART);
//  Serial.println("waiting restart....");
//  delay(5000);
//  int readings;
//  byte a;
//  while(wifi.available()){
//
//    readings = wifi.read();
//    //Serial.write(readings);    
//
//  }
//  Serial.println();
//
//
////    Serial.println(((String)RESPONSE).length());
//    Serial.println("something cameBack");
//    
////  }else{
////    Serial.println("nothing at all");
////  }
//
//  wifi.println(RESTART);
//  Serial.println("waiting restart....");
//  delay(5000);
//  String readings;

//  wifi.println((String)MODE+"?");
//  Serial.println("checking current mode");
//  delay(1000);
//        
//  if (wifi.available()){
//    readings = wifi.readString();
//    Serial.println(readings.length());
//    Serial.println(readings);
//    Serial.println("something cameBack");
//  }else{
//    Serial.println("nothing at all");
//  }  
//
//  
//  wifi.println((String)MODE+3);
//  Serial.println("Waiting for mode to be set");
//  delay(1000);
//        
//  if (wifi.available()){
//    readings = wifi.readString();
//    Serial.println(readings.length());
//    Serial.println(readings);
//
//    Serial.println("something cameBack");
//  }else{
//    Serial.println("nothing at all");
//  }  
//
//
//  Serial.println();
//  String cwName = "Green";
//  String cwPassword = "1a2z3e4r5t6";
//
//  String connectionParam = ((String)CONNECT)+"=\""+cwName+"\","+"\""+cwPassword+"\"";
//  wifi.println(connectionParam);
//  Serial.println("Trying to connect...");
//  delay(1500);
//
//  if (wifi.available()){
//    readings = wifi.readString();
//    Serial.println(readings.length());
//    Serial.println(readings);
//
//    Serial.println("something cameBack");
//  }else{
//    
//    Serial.println("nothing at all");
//  }  

//  Serial.println(wifi.readString());
//  Serial.println((String)RESPONSE);

}

void initGPS()
{ 
  Serial.print("Testing TinyGPS library v. "); Serial.println(TinyGPS::library_version());
  sGps.begin(9600);
}
int a = 0;

void readGPS(){
  a = 1;
  unsigned long age;
  float glat, glon;
  byte month, day, hour, minute, second, hundredths;
  int year;
  String timeStamp;
  for(int i = 0; i < 3000; i++){gps.encode(sGps.read());}
  gps.f_get_position(&glat, &glon, &age);gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);

   Serial.print("lat: ");Serial.println(glat);Serial.print(" lon: ");Serial.println(glon);
//   sprintf(tsmp,"%02d/%02/%02d %02d:%02d:%02d ",month,day,year, hour, minute, second);
   String b = String(month)+"/"+String(day)+"/"+String(year)+" "+String(hour)+":"+String(minute)+":"+String(second);
   timeStamp = b;
//  gps_lat = glat;
//  gps_lon = glon;

   Serial.print("timeStamp: ");
  Serial.println(timeStamp);
//   Serial.println(b);

}
void loop() {
//  byte readings;
//  if(Serial){
//    wifi.println(DISCONNECT);
//    readings = wifi.read();
////    Serial.println(readings.length());
//    Serial.println((String)readings);
//
//  }

//  // put your main code here, to run repeatedly:
//  if(Serial.available()) {
//
//    String command = Serial.readString();
//
//    wifi.println(command);
//    delay(200);
//  readGPS();

  if (wifi.available()) {
    Serial.write(wifi.read());
  }



  
}
