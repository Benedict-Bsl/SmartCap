#define TINY_GSM_MODEM_SIM800
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include "DFRobot_ID809.h"
#include <TinyGPS.h>
#include <TimerOne.h>

// #include <Adafruit_GPS.h>

#define MOTOR_OUTPUT 8

#define COLLECT_NUMBER 3  //Fingerprint sampling times, can be set to 1-3
#define IRQ         3
  //IRQ pin 

/*Use software serial when using UNO or NANO*/
#include <SoftwareSerial.h>

//gps 10,9
//fp 6,5
//gsm 7, 8
SoftwareSerial Serial1(6,5);
#define FPSerial Serial1

DFRobot_ID809 fingerprint;

SoftwareSerial gsm(7, 8);

int volatile flag = 0;
int fingerPrintAccepted = 0;

SoftwareSerial sGps(10,9);
TinyGPS gps;
float gps_lat;
float gps_lon;
float deg;
unsigned long age, date = 0;
byte month, day, hour, minute, second, hundredths;
int year;
String timeStamp;
int a = 0;
float pillsQuantity=91;

StaticJsonDocument<200> resp;

const char buz[] = "value=&token=";
const char TRACKIP_HOST[] = "170.187.154.110";
const char URL[] = "170.187.154.110/api/v1/smartcap/update?";
const char ATH_URL[] = "170.187.154.110/api/v1/smartcap/auth?";
const char buzz[] = "170.187.154.110/api/v1/smartcap/check/status?";

const char gsmStatus[] = "AT+CIPSTATUS\n";
const char term[] ="AT+HTTPTERM\n";
const char shut[] = "AT+CIPSHUT\n";
const char sapbr0[] = "AT+SAPBR=0,1\n";
const char sapbr[] = "AT+SAPBR=1,1\n";

const char internet[] = "AT+CSTT=\"iternet\",\"\",\"\"\n";
const char ciicr[] = "AT+CIICR\n";
const char cifsr[] = "AT+CIFSR\n";
const char sapbr2[] = "AT+SAPBR=2,1\n";
const char hinit[] = "AT+HTTPINIT\n";
const char cid[] = "AT+HTTPPARA=CID, 1\n";
const char cidn[] = "AT+HTTPPARA=CID, 0\n";

const char tchy[] = "AT+CDNSGIP=www.techy8.com\n";
const char htpara[] ="AT+HTTPPARA=";
const char cm[] = ",";
const char endl[] = "\n";
const char qt[] = "\"";
const char httppara[] = "AT+HTTPPARA?\n";
const char u[] = "http://";
const char POst[]= "AT+HTTPACTION=1\n";
const char REad[] = "AT+HTTPREAD\n";
const char discn[] = "AT+CGATT=";
const char on[] = "1";
const char off[] = "0";  
const char auth_var[] = "device_id=";
// #define device_id[] =  "098765";
// #define device_id "123456"
const char device_id[] = "123456";
const char url[] = "URL";
String v;

const char token[]="720b703c-a433-4c1f-92f4-fd6c49e63ec1";

char c;

void setup() {

    Timer1.initialize(5000000);
  Timer1.attachInterrupt(toggle);
  Timer1.stop();
  pinMode(4, OUTPUT);
  // put your setup code here, to run once:
  gsm.begin(9600);
  Serial.begin(9600);
  httpCon();
  sGps.begin(9600);
  delay(1000);

//  readGPS();


  Serial.println("starting loop");
  
//  Timer1.restart();
}
void loop() {
  Serial.println("getting buzzer value:");
  buzzer();
  
////  Timer1.restart();
//  if(flag == 1){
//    Timer1.stop();
//    updateData();
//    Timer1.restart();
//  }
//  
//  readGPS();
//  // put your main code here, to run repeatedly:
//  Serial.print("lat: ");Serial.println(gps _lat);Serial.print(" lon: ");Serial.println(gps_lon);

}

void httpCon() {


//  const char dataR[]= "finger_print=0&quantity=95&lat=-6.7702270&lon=39.2608260&token=5230ce32-2d0b-40e8-88d5-8f4ecde654aa";

//  const char token[]="5230ce32-2d0b-40e8-88d5-8f4ecde654aa";


  gsm.write(gsmStatus);
  gsmRead(1000L,v);
  v = "";
  gsm.write(discn);gsm.write(off);gsm.write(endl);
  gsmRead(5000L,v);
  v = "";

  gsm.write(discn);gsm.write(on);gsm.write(endl);
  gsmRead(6000L,v);
  v = "";
  
  // Serial.println(F("term http"));
  gsm.write(term);
  gsmRead(1500L,v);
  v = "";
  
  // Serial.println(F("Restarting service..."));
  gsm.write(shut);
  gsmRead(1500L,v);
  v = "";
  
  gsm.write(gsmStatus);
  // Serial.print(F("read status..."));
  gsmRead(2500L,v);
  v = "";

  // Serial.println(F("first sapbr"));
  gsm.write(sapbr0);
  gsmRead(2500L,v);
  v = "";
  
  // Serial.println(F("trying internet.."));
  
  gsm.write(internet);
  gsmRead(2500L,v);
  v = "";
  
  // Serial.println(F("initializing internet"));
  gsm.write(ciicr);
  gsmRead(2500L,v);
  v = "";

  
  // Serial.println(F("Getting IP..."));
  gsm.write(cifsr);
  gsmRead(2500L,v);
  v = "";

  // Serial.println(F("second sapbr"));
  gsm.write(sapbr2);
  gsmRead(2500L,v);
  v = "";
    
  // Serial.println(F("first sapbr"));
  gsm.write(sapbr);
  gsmRead(8500L,v);
  v = "";
  
  // Serial.println(F("first sapbr again"));
  gsm.write(sapbr2);
  gsmRead(2500L,v);
  v = "";

  // Serial.println(F("init http"));
  gsm.write(hinit);
  gsmRead(2500L,v);
  v = "";
  
  // Serial.println(F("CID init"));
  gsm.write(cid);
  gsmRead(2500L,v);
  v = "";

  
  // Serial.println(F("techy8 ip"));
  gsm.write(tchy);
  gsmRead(2500L,v);
  v="";

//  postData();


}
void buzzer(){

   gsm.write(htpara);gsm.write(qt); gsm.write(url); gsm.write(qt);gsm.write(cm);gsm.write(qt);gsm.write(u);
   gsm.write(buzz);gsm.write(buz);gsm.write(token);gsm.write(qt);gsm.write(endl);
   gsmRead(3500L,v);
   clearGsm();
   delay(100);
    Serial.println(F("Posting values.. 6.5 sec"));
    
    v="";
   gsm.write(POst);
   gsmRead(6500L, v);

   v="";
   Serial.println("data json:");
   gsm.write(REad);
   String b = jsonData(1000L);
   Serial.println(b);

   DeserializationError er =deserializeJson(resp, b);

   if(er){
    Serial.print(F("failed to deserialize json"));
    Serial.println(er.f_str());
    
   }

    const int valu = resp["value"];
    if(valu == 1){
      Serial.println(F("buzzer is on"));
      digitalWrite(4, 1);
      delay(100);
      digitalWrite(4,0);
      delay(500);
      digitalWrite(4, 1);
      delay(100);
      digitalWrite(4,0);
      delay(500);
      digitalWrite(4, 1);
      delay(1000);
      digitalWrite(4,0);
      delay(500);
      
        
      digitalWrite(4, 1);
      delay(500);
      digitalWrite(4,0);
      delay(500);
      
      digitalWrite(4, 1);
      delay(500);
      digitalWrite(4,0);
      delay(1000);
    }
    else{
      Serial.println("button not pressed");
    }
    
   
}
void toggle(){
  flag= 1;
}

void updateData(){

  gsm.flush();

  delay(100);
  pillsQuantity = 20 - 1;
  String dat_a = "&finger_print=";
  readGPS();
   float lat_h =-6.7702270;
   float lon_h = 39.2608260;
  gps_lat = lat_h;
  gps_lon = lon_h;
  dat_a.concat(String(fingerPrintAccepted));
  dat_a.concat("&quantity=");
  dat_a.concat(String(pillsQuantity));
  dat_a.concat("&lat=");
  dat_a.concat(String(gps_lat,7));
  dat_a.concat("&lon=");
  dat_a.concat(String(gps_lon,7));
  dat_a.concat("&token=");
  dat_a.concat(token);

  const char dataR[dat_a.length()+1];
  strcpy(dataR, dat_a.c_str());

  delay(2000);
  gsm.write(htpara);gsm.write(url);gsm.write(cm);gsm.write(qt);gsm.write(u);
  gsm.write(TRACKIP_HOST);gsm.write(URL);gsm.write(dataR);gsm.write(qt);gsm.write(endl);
  gsmRead(2500L, v);

  Serial.println(F("HTTPACtiON"));
  gsm.write(POst);
  gsmRead(1500L, v);
  v="";

  Serial.println(F("HTTP read"));
  gsm.write(REad);
  jsonData(2500L);
}
void postData(){
   clearGsm();
   delay(100);
   gsm.write(htpara);gsm.write(qt); gsm.write(url); gsm.write(qt);gsm.write(cm);gsm.write(qt);gsm.write(u);
   gsm.write(TRACKIP_HOST);gsm.write(ATH_URL);gsm.write(auth_var);gsm.write(device_id);gsm.write(qt);gsm.write(endl);
   gsmRead(2500L,v);
    v="";
   gsm.write(POst);
   gsmRead(6500L, v);
   v="";

   gsm.write(REad);
   gsmRead(1000L, v);

   
}

void clearGsm(){
  while(gsm.available()>0){
    gsm.read();
  }
}

String jsonData(uint32_t timeout_ms){

  uint32_t startMillis = millis();
  String jsonData="";
  int a = 0;
  String dataRaw="";
  do{
    while(gsm.available()){
      c = gsm.read();
      dataRaw.concat(c);
      if(c=='\n'){
//        Serial.println("end of data");
         continue;
      }
      if(c=='{'){
        a+=1;
      }
      if(c=='}'){
        a+=1;
      }
      if(a ==1 || a == 2){
        jsonData.concat(c);
      }
      if(a==2){
        break;
      }
    }

    jsonData.replace("OK","");

  }while(millis() - startMillis < timeout_ms);
  return jsonData;
//  

//    StaticJsonDocument<200> responseFile;
//  
//    // deserializeJson()
//    DeserializationError er = deserializeJson(responseFile, jsonData);
//
//    if(er){
//      Serial.print(F("DeserializeJson() failed: "));
//      Serial.println(er.f_str());
//  //    return;
//    }
//
////    strcpy(dataR, dat_a.c_str());
//    String tock= responseFile["token"];
//    const char d[tock.length()+1];
//    strcpy(token, tock.c_str());
//
//  Serial.println(F("token: "));
//  Serial.write(token);
//  Serial.println("");


}


void gsmRead(uint32_t timeout_ms, String gsmD){
  uint32_t startMillis = millis();
  do{
    while(gsm.available()){
      c = gsm.read();
      if(c=='\n'){
//        Serial.println("end of data");
         continue;
      }
      gsmD.concat(c);
    }
    gsmD.trim();
  }while(millis() - startMillis < timeout_ms);
  Serial.println(gsmD);
}

