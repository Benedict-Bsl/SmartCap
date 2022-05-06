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


#define TINY_GSM_MODEM_SIM800
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <TinyGsmClient.h>
#include "DFRobot_ID809.h"
#include <TinyGPS.h>
#include <TimerOne.h>
// #include <Adafruit_GPS.h>

#define MOTOR_OUTPUT 8

#define COLLECT_NUMBER 3  //Fingerprint sampling times, can be set to 1-3
#define IRQ         6  //IRQ pin 

/*Use software serial when using UNO or NANO*/
#include <SoftwareSerial.h>

//gps 10,9
//fp 6,5
//gsm 7, 8
SoftwareSerial Serial1(6,5);
#define FPSerial Serial1

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

String Data;
String Datax;
const char url[] = "URL";
int vn;
int m = 0;
char c;
String gsmD="";
String v;


//finger print
int comparison_flag = 0;
int registration_flag = 0;
int deletion_flag = 0;
int fingerPrintAccepted = 0;

//GPS initials
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

//GSM Variables;
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

// const String apn  ="internet";//airtel ->"airtelgprs.com"
// const String user ="";
// const String pass ="";
const char auth_var[] = "device_id=";
// const char device_id[] =  "098765";
const char device_id[] = "123456";
String tokenV="";
String quantity="";

int volatile flag = 0;

SoftwareSerial gsm(7,8);
// TinyGsm modem(gsm);
// TinyGsmSim800 modemNet(gsm);
// TinyGsmClient clientModem(modemNet);

// TinyGsmClientSecure gsm_client_secure_modem(modem, 0);
// TinyGsmClient gsm_client_secure_modem(modem, 0);

// HttpClient http_client = HttpClient(gsm_client_secure_modem, FIREBASE_HOST, SSL_PORT);
// HttpClient http_client = HttpClient(gsm_client_secure_modem, TRACKRX_HOST, HTTP_PORT );

unsigned long previousMillis = 0;

void initGSM();
void data();

//to send send data with post method http to Alfred's API database 
void dataRx(int FingerPrint, float pillsQuantity, float longitude, float latitude);

// firebase method post
// void SendToFirebase(const char* method, const String & path, const String & data, HttpClient *http);

// initialize connection and send data.
void ConnectionSubroutine();

// // send data to firebase using joseph's api Server.
// void dataToIp();
// // first handshake to get token.
// char *requestToken(const char* method, const String & path, const String & data, HttpClient *http);
// // send to trackRx api IP method.
// void sendToTrackRxIP(const char* method, const String & path, const String & data, HttpClient *http);
// // send to trackRX alfred's domain name.
// void sendToTrackRX(const char* method, const String & path, const String & data, HttpClient *http);

void gsmInitialization();
void internetAccess();

//finger print functions
void initFP();
//void fingerMatch();
//void trigger();
// bool MatchFingerPrint();
// bool RegisterFingerPrint();

// http connection start
void httpConnectionStart(const String & data, const char & URL);
void http(const char* url_path,const char* body);
bool httpCon();
void http(const char* a, const char* b, const char* c);
void http(const char* url_path,const char* body);
void gsmRead(uint32_t timeout_ms, String gsmD);
String gsmRead_JsonOnly(uint32_t timeout_ms, String gsmD);
void httpPost();
void getAuthId();
void gsmReadResponse(uint32_t timeout_ms, String gsmD);
//GPs functions.
void initGPS();
void readGPS();
void smartDelay(unsigned long ms);
void httpCon(String dataR);
void gsmRead();


void readGPS(){
  a = 1;
  unsigned long age;
  float glat, glon;
  byte month, day, hour, minute, second, hundredths;
  int year;

  for(int i = 0; i < 2000; i++){gps.encode(sGps.read());\   
  gps.f_get_position(&glat, &glon, &age);delay(2);gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);}

  // Serial.print("lat: ");Serial.println(glat);Serial.print(" lon: ");Serial.println(glon);
//   sprintf(tsmp,"%02d/%02/%02d %02d:%02d:%02d ",month,day,year, hour, minute, second);
  String b = String(month)+"/"+String(day)+"/"+String(year)+" "+String(hour)+":"+String(minute)+":"+String(second);
  timeStamp = b;
  gps_lat = glat;
  gps_lon = glon;

  // Serial.print("timeStamp: ");
//  Serial.println(timeStamp);
//   Serial.println(b);

}


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

//void trigger(){
//  flag = 1;
//}

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

void gsmReadResponse(uint32_t timeout_ms, String gsmD){
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
  }while(millis() - startMillis < timeout_ms);
  Serial.println(gsmD);
}

String gsmRead_JsonOnly(uint32_t timeout_ms, String gsmD){
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
  //  Serial.println("replaced version");
    gsmD.replace("+HTTPREAD:","");
    gsmD.replace("104","");
    
    gsmD.replace("\n","");
    gsmD.replace("OK","");
    gsmD.trim();
  }while(millis() - startMillis < timeout_ms);
  Serial.println(gsmD);
  return gsmD;
}

//void fingerMatch(){
//  uint8_t ret = 0;
//  fingerprint.ctrlLED(fingerprint.eBreathing, fingerprint.eLEDBlue, 0);
//  Serial.println("Please put your finger on the fingerPrint sensor");
//
//  if((fingerprint.collectionFingerprint(0) != ERR_ID809)){
//
//    // while collecting fingerprint, sent fingerprint mode to quick blink
//    fingerprint.ctrlLED(fingerprint.eFastBlink, fingerprint.eLEDYellow, 3);
//    Serial.println("Capturing succeds");
//    Serial.println("release finger");
//
//    while(fingerprint.detectFinger());
//    Serial.println("FingerReleased");
//    //search for the desired fingerprint
//    ret = fingerprint.search();
//
//    if(ret != 0){
//      fingerprint.ctrlLED(fingerprint.eKeepsOn, fingerprint.eLEDGreen, 0);
//      Serial.print("Got one: ");
//      Serial.println(ret);
//     fingerPrintAccepted = 1;
//        digitalWrite(12,100);
//        digitalWrite(11, 0);
//        delay(200);
//        digitalWrite(12, 0);
//        digitalWrite(11, 0);
//        delay(10000);
//        digitalWrite(12, 0);
//      //  delay(3000);
//        digitalWrite(11,100);
//        delay(200);
//        digitalWrite(12, 0);
//        digitalWrite(11, 0);
//        delay(5000);
//    }
//    else{
//      fingerprint.ctrlLED(fingerprint.eKeepsOn, fingerprint.eLEDRed, 0);
//      Serial.println("Matching failed");
//     fingerPrintAccepted = 0;
//    }
//  }
//  else{
//    fingerprint.ctrlLED(0, 0, 0);
//    Serial.println("Try again..");
//  }
//  
//}

