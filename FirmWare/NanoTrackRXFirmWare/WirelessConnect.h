//
//#define AT_OK "AT"
//#define RESTART "RST"
//#define RESPONSE 11
//#define VERSION "AT+GMR"
//#define DSLEEP "AT+GSLP"
//#define SLEEP "AT+SLEEP="
//
//#define SERIAL_OUTPUT true
////wifi Commands
//#define MODE  "AT+CWMODE_CUR"
//#define SMODE "AT+CWMODE_DEF"
//#define CONNECT "AT+CWJAP"
//#define SCONNECT "AT+CWJAP_DEF"
//#define DISCONNECT "AT+CWQAP"
//#define AUTO_CONNECT "AT+CWAUTOCONN"
//
//
//class WirelessConnect{
//
////    private:
//
//    public:
//      WirelessConnect(String cwName, String cwPassword);
//      WirelessConnect(void);
//      void serialOut(String output); 
//     
//};
bool db = true;

#define TINY_GSM_MODEM_SIM800
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <TinyGsmClient.h>
#include "DFRobot_ID809.h"
#include <TinyGPS.h>
#include <TimerOne.h>
// #include <Adafruit_GPS.h>
#include <APDS9930.h>
#define MOTOR_OUTPUT 8

#define COLLECT_NUMBER 3  //Fingerprint sampling times, can be set to 1-3
#define IRQ         6  //IRQ pin 

/*Use software serial when using UNO or NANO*/
#include <SoftwareSerial.h>

//gps 10,9
//fp 6,5
//gsm 7, 8
SoftwareSerial Serial1(6,5);
SoftwareSerial D1Mini(9,10);

#define FPSerial Serial1
APDS9930 apds = APDS9930();
uint16_t proximity_data = 0;

DFRobot_ID809 fingerprint;

IPAddress trackRXIP(170, 187,154, 110);
const char TRACKRX_HOST[] = "trakrx.brik8.com";
const char TRACKIP_HOST[] = "170.187.154.110";
const char URL[] = "/api/v1/smartcap/update?";
const char ATH_URL[] = "/api/v1/smartcap/auth?";
const char FIREBASE_HOST[]  = "trackrx-271d3-default-rtdb.firebaseio.com";
const String FIREBASE_AUTH  = "7CNETSMtdPrTzoQvJv21WkD8DmrFatSTLhAUPt0h";
const String FIREBASE_PATH  = "/";
const int SSL_PORT          = 443;

const int HTTP_PORT = 80;

bool FingerPrint;
float pillsQuantity=91;
bool mtCtrl = false;
String Data;
String Datax;
const char url[] = "URL";
int vn;
int m = 0;
char c;
String gsmD="";
//String v;


//finger print
int comparison_flag = 0;
int registration_flag = 0;
int deletion_flag = 0;
int fingerPrintAccepted = 0;


const char auth_var[] = "device_id=";
// const char device_id[] =  "098765";
const char device_id[] = "123456";
String tokenV="";
String quantity="";

int volatile flag = 0;

//SoftwareSerial gsm(7,8);

unsigned long previousMillis = 0;


void initFP(){

  FPSerial.begin(115200);
  FPSerial.listen();

  fingerprint.begin(FPSerial);

  while(!Serial);

  while(fingerprint.isConnected() == false){

    Serial.println("not communicating with device");

    delay(1000);
  }
  Serial.println("Device connected!");
}

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
        Serial.print(F(": 0x"));
        Serial.println(val, HEX);
      }
    }
  }
  apds.wireReadDataByte(0x1E, val);
  if(db){
    Serial.print(0x1E, HEX);
    Serial.print(F(": 0x"));
    Serial.println(val, HEX);
  }

#endif
}


uint16_t a = 0;
bool v= 0;

bool somaNumber(){
  if ( !apds.readProximity(proximity_data) ) {
    Serial.println(F("Error reading proximity value"));
  } else {
    if(proximity_data > 280 && proximity_data <315){
      a= 300;
      
    }else if(proximity_data > 315 && proximity_data < 410){
      a = 400;
    }else if(proximity_data > 410 && proximity_data < 510){
      a = 500;
    }

    Serial.println(proximity_data);
    if(a >= 400){
      
    Serial.print(" ");
    Serial.print("removed: ");       
    Serial.println(1);
      v=true;
      D1Mini.write("1");
    }
    else{
      v = false;
    }
    
  }
  return v;
}
char x;
char ck='0';
void sikiliza(){
  D1Mini.listen();
  Serial.print("Receiving: ");
  while(D1Mini.available()){
    Serial.print(" Got:");
    ck = D1Mini.read();
    
    Serial.print(ck);
    if(ck == '4'){
      mtCtrl = true;
    }
    break;
    

    Serial.print("loop: ");

  }
  delay(1000);
//  if(ck!=' '){
//    if(ck == '1'){
//      Serial.print("ring...");
//    }
//  }
//  if(ck !=' '){
//    if(ck == '2'){
//      Serial.print("motor2...");
//    }
//  }
  
  Serial.print(ck);Serial.print(" ");
  Serial.println("continuing");
    Serial.println();
    if(ck == '1'){
      Serial.println(F("Writing to buzzer"));
    digitalWrite(4,1);
    delay(100);
    digitalWrite(4,0);
    delay(50);
    digitalWrite(4, 1);
    delay(100);
    digitalWrite(4,0);
    delay(500);
    digitalWrite(4, 1);
    delay(1000);
    }
    else if(mtCtrl){
      Serial.println(F("Bottom locking mechanism.."));
      digitalWrite(7,1);
      digitalWrite(8,0);
      delay(150);
      digitalWrite(7,0);
      digitalWrite(8,0); 
      delay(15000);
      Serial.println(F("Back again..."));
      digitalWrite(7,0);
      digitalWrite(8,1);
      delay(130);
      digitalWrite(7,0);
      digitalWrite(8,0); 
      delay(10000);

      mtCtrl = false;
    }
    else if(ck=='0'){
      digitalWrite(4,0);  
      digitalWrite(7,0);
      digitalWrite(8,0);
    }


}
