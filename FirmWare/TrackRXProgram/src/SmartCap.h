#include <SoftwareSerial.h>
#include "jsonlib.h"
#include "SIM800L.h"
#include "DFRobot_ID809.h"
#include <TinyGPS.h>
#include <Wire.h>
#include "APDS9930.h"


//micro definitions for fingerPrint
const unsigned int IRQ = 3;


//Devices access

SoftwareSerial Serial1(6,5);
SoftwareSerial sGps(10, 9);
SIM800L* sim800c;

// StaticJsonDocument<32> doc;


TinyGPS gps;
#define FPSerial Serial1
DFRobot_ID809 fingerprint;
bool db = true;
//some global var
const char cm[] = ",";
const char endl[] = "\n";
const char qt[] = "\"";
const char u[] = "http://";

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
APDS9930 apds = APDS9930();
uint16_t proximity_data = 0;

//functions prototyping

// void http();
// void clearGsm();
// void gsmRead(uint32_t);
void customDelay(uint32_t);
void readGPS();
bool initSensor();
void sens();

bool intSensor() {
  
  // Initialize APDS-9930 (configure I2C and initial values)
  if ( apds.init() ) {
    if(db){Serial.println(F("APDS-9930 initialization complete"));}
  } else {
    if(db){Serial.println(F("Something went wrong during APDS-9930 init!"));}
    return false;
  }

  if ( apds.enableProximitySensor(false) ) {
    if(db){Serial.println(F("Proximity sensor is now running"));}
  } else {
    if(db){Serial.println(F("Something went wrong during sensor init!"));}
    return false;
  }

#ifdef DUMP_REGS
  /* Register dump */
  uint8_t reg;
  uint8_t val;

  for(reg = 0x00; reg <= 0x19; reg++) {
    if( (reg != 0x10) && \
        (reg != 0x11) )
    {
      apds.wireReadDataByte(reg, val);
      if(db){
        Serial.print(reg, HEX);
        Serial.print(": 0x");
        Serial.println(val, HEX);
      }
    }
  }
  apds.wireReadDataByte(0x1E, val);
  if(db){
    Serial.print(0x1E, HEX);
    Serial.print(": 0x");
    Serial.println(val, HEX);
  }

#endif
}

void sens() {
  
  if ( !apds.readProximity(proximity_data) ) {
    Serial.println("Error reading proximity value");
  } else {

    if(proximity_data > 280 && proximity_data <315){
      sv= 300;
      
    }else if(proximity_data > 315 && proximity_data < 410){
      sv = 400;
    }else if(proximity_data > 410 && proximity_data < 510){
      sv = 500;
    }
    Serial.print(" ");
    Serial.print("removed: ");
    Serial.print(sv);
//      if(proximity_data > 320){
//        
//        a+=1;
//        return;
//      }
//      Serial.print(a);
    Serial.println(" pill");
  }
  
  
}
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

void trigger(){
  flag = 1;
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

    // while(fingerprint.detectFinger());
    // if(db){Serial.println(F("FingerReleased"));}
    //search for the desired fingerprint
    ret = fingerprint.search();

    if(ret != 0){
      fingerprint.ctrlLED(fingerprint.eKeepsOn, fingerprint.eLEDGreen, 0);
      if(db){Serial.print(F("Got one: "));}
      if(db){Serial.println(ret);}
     fingerPrintAccepted = 1;
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

// void buzzer(){
//   // PORTD ^= (1 << 4);
//   // customDelay(100);
//   // PORTD ^= (1 << 4);
//   // customDelay(500);
//   // PORTD ^= (1 << 4);
//   // customDelay(100);
//   // PORTD ^= (1 << 4);
//   // customDelay(500);
//   // PORTD ^= (1 << 4);
//   // customDelay(1000);
//   digitalWrite(4, 1);
//   delay(100);
//   digitalWrite(4,0);
//   delay(500);
//   digitalWrite(4, 1);
//   delay(100);
//   digitalWrite(4,0);
//   delay(500);
//   digitalWrite(4, 1);
//   delay(1000);

// }

void readGPS(){
  unsigned long age;
  float glat, glon;
  byte month, day, hour, minute, second, hundredths;
  int year;

  for(int i = 0; i < 2000; i++){gps.encode(sGps.read());\   
  gps.f_get_position(&glat, &glon, &age);delay(2);gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);}

  if(db){ Serial.print("lat: ");Serial.println(glat);Serial.print(" lon: ");Serial.println(glon);}
//   sprintf(tsmp,"%02d/%02/%02d %02d:%02d:%02d ",month,day,year, hour, minute, second);
  // String b = String(month)+"/"+String(day)+"/"+String(year)+" "+String(hour)+":"+String(minute)+":"+String(second);
  // timeStamp = b;
  gps_lat = glat;
  gps_lon = glon;

  // Serial.print("timeStamp: ");
//  Serial.println(timeStamp);
//   Serial.println(b);

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