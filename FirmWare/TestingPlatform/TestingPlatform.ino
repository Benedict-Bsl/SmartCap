#include <SoftwareSerial.h>

SoftwareSerial wifi(5, 4);

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
  Serial.begin(9600);

//  while(!Serial){
//    
//  }

  wifi.begin(115200);
  delay(50);
  wifi.println(RESTART);
  Serial.println("waiting restart....");
  delay(5000);
  int readings;
  byte a;
  while(wifi.available()){

    readings = wifi.read();
    //Serial.write(readings);    

  }
  Serial.println();


//    Serial.println(((String)RESPONSE).length());
    Serial.println("something cameBack");
    
//  }else{
//    Serial.println("nothing at all");
//  }

  wifi.println(RESTART);
  Serial.println("waiting restart....");
  delay(5000);
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
//  while(Serial.available()) {
//
//    String command = Serial.readString();
//
//    wifi.println(command);
//    delay(200);
//
//
//    if (wifi.available()) {
//      Serial.println(wifi.readString());
//    }
//  }



  
}
