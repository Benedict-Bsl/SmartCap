#include <SoftwareSerial.h>
#include "jsonlib.h"
#include "SIM800L.h"
#include "DFRobot_ID809.h"



//micro definitions for fingerPrint
const unsigned int IRQ = 3;


//Devices access

SoftwareSerial Serial1(6,5);
SoftwareSerial sGps(10, 9);
SIM800L* sim800c;

// StaticJsonDocument<32> doc;



#define FPSerial Serial1
DFRobot_ID809 fingerprint;
bool db = true;
//some global var


const char URL[] = "170.187.154.110/api/v1/smartcap/update";
const char ATH_URL[] = "170.187.154.110/api/v1/smartcap/auth";
const char status[] = "170.187.154.110/api/v1/smartcap/check/status?";
const char content_appCoded[] = "application/x-www-form-urlencoded";
const char content_json[]  = "application/json";
const char buzz[] = "170.187.154.110/api/v1/smartcap/check/status";
unsigned int fingerPrintAccepted=0;
signed int gps_lat;
signed int gps_lon;
int sv = 0;
unsigned volatile int flag = 0;
unsigned volatile int flag2 = 0;

//functions prototyping

// void http();
// void clearGsm();
// void gsmRead(uint32_t);
void customDelay(uint32_t);

void customDelay(uint32_t timeout_ms){
    uint32_t startMil = millis();
    do{

        
    }while(millis() - startMil < timeout_ms);

}


void initFP(){

  FPSerial.begin(115200);
  FPSerial.listen();

  fingerprint.begin(FPSerial);

  while(!Serial);

  while(fingerprint.isConnected() == false){

    if(db){Serial.println(F("not communicating with device"));}

    delay(1000);
  }
  if(db){Serial.println(F("Device connected!"));}
}


void fingerMatch(){
  uint8_t ret = 0;
  // fingerprint.ctrlLED(fingerprint.eBreathing, fingerprint.eLEDBlue, 0);
  // if(db){Serial.println(F("Please put your finger on the fingerPrint sensor"));}

  if((fingerprint.collectionFingerprint(0) != ERR_ID809)){

    // while collecting fingerprint, sent fingerprint mode to quick blink
    // fingerprint.ctrlLED(fingerprint.eFastBlink, fingerprint.eLEDYellow, 3);
    // if(db){Serial.println(F("Capturing succeds"));}
    // // if(db){Serial.println(F("release finger"));}

    while(fingerprint.detectFinger());
    // if(db){Serial.println(F("FingerReleased"));}
    //search for the desired fingerprint
    ret = fingerprint.search();

    if(ret != 0){
      fingerprint.ctrlLED(fingerprint.eKeepsOn, fingerprint.eLEDGreen, 0);
      if(db){Serial.print(F("Got one: "));}
      if(db){Serial.println(ret);}
     fingerPrintAccepted = 1;
     //motor turn
        digitalWrite(12,100);
        digitalWrite(11, 0);
        delay(200);
        digitalWrite(12, 0);
        digitalWrite(11, 0);
        delay(10000);
        digitalWrite(12, 0);
      //  delay(3000);
        digitalWrite(11,100);
        delay(200);
        digitalWrite(12, 0);
        digitalWrite(11, 0);
        delay(5000);
    }
    else{
      fingerprint.ctrlLED(fingerprint.eKeepsOn, fingerprint.eLEDRed, 0);
      if(db){Serial.println(F("Matching failed"));}
     fingerPrintAccepted = 0;
    }
  }
  else{
    // fingerprint.ctrlLED(0, 0, 0);
    if(db){Serial.println(F("Try again.."));}
  }
  
}

void buzzer(){
  PORTD ^= (1 << 4);
  customDelay(100);
  PORTD ^= (1 << 4);
  customDelay(500);
  PORTD ^= (1 << 4);
  customDelay(100);
  PORTD ^= (1 << 4);
  customDelay(500);
  PORTD ^= (1 << 4);
  customDelay(1000);

}



// void readGPS(uint32_t timeout_ms){
//   unsigned long age;
//   float glat, glon;
//   byte month, day, hour, minute, second, hundredths;
//   int year;
//   uint32_t startMil = millis();      
//     do{
//         gps.encode(sGps.read());\   
//         gps.f_get_position(&glat, &glon, &age);
//         gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
//     }while( millis() - startMil < timeout_ms);

//   // Serial.print("lat: ");Serial.println(glat);Serial.print(" lon: ");Serial.println(glon);
// //   sprintf(tsmp,"%02d/%02/%02d %02d:%02d:%02d ",month,day,year, hour, minute, second);
//   // String b = String(month)+"/"+String(day)+"/"+String(year)+" "+String(hour)+":"+String(minute)+":"+String(second);
//   // timeStamp = b;
//   gps_lat = glat;
//   gps_lon = glon;

//   // Serial.print("timeStamp: ");
// //  Serial.println(timeStamp);
// //   Serial.println(b);

// }