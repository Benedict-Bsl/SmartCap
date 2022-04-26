
#include <DFRobot_ID809.h>

#if ((defined ARDUINO_AVR_UNO) || (defined ARDUINO_AVR_NANO))
    #include <SoftwareSerial.h>
    SoftwareSerial Serial1(8, 9);  //RX, TX
    #define FPSerial Serial1
#else
    #define FPSerial Serial1
#endif
DFRobot_ID809 fingerprint;

const String apn  ="internet";//airtel ->"airtelgprs.com"
const String user ="";
const String pass ="";
String auth_var = "device_id=";
String device_id =  "098765";
//String token;

SoftwareSerial gsm(6,7);

int comparison_flag = 0;
int registration_flag = 0;
int deletion_flag = 0;
int fingerPrintAccepted = 0;

const char TRACKIP_HOST[] = "170.187.154.110";
const char URL[] = "/api/v1/smartcap/update?";
const char ATH_URL[] = "/api/v1/smartcap/auth?";

String Data;
String Datax;
String url = "URL";

String token="5230ce32-2d0b-40e8-88d5-8f4ecde654aa";
void setup(){

  Serial.begin(115200);
  
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  
  pinMode(10, OUTPUT);
  pinMode(6, OUTPUT);
//  gsm.begin(9600);

  FPSerial.begin(115200);

  fingerprint.begin(FPSerial);

  while(!Serial);

  while(fingerprint.isConnected() == false){

    Serial.println("not communicated with device");

    delay(1000);
  }

  fingerprint.ctrlLED(fingerprint.eBreathing, fingerprint.eLEDYellow, 3); 
  Serial.println("Device connected!");
  Serial.println("touch the sensor");
  Serial.println("Please put your finger on the fingerPrint sensor");
}

void loop(){
  uint8_t ret = 0;


  if(fingerprint.detectFinger() == 1){
    Serial.println("You touched the FP sensor");
//    fingerprint.ctrlLED(fingerprint.eFastBlink, fingerprint.eLEDGreen, 2);

    fingerprint.ctrlLED(fingerprint.eBreathing, fingerprint.eLEDBlue, 0);
  
    if((fingerprint.collectionFingerprint(0) != ERR_ID809)){
  
      // while collecting fingerprint, sent fingerprint mode to quick blink
      fingerprint.ctrlLED(fingerprint.eFastBlink, fingerprint.eLEDYellow, 3);
      Serial.println("Capturing succeds");
      Serial.println("release finger");
  
      while(fingerprint.detectFinger());
      Serial.println("FingerReleased");
      //search for the desired fingerprint
      ret = fingerprint.search();
  
      if(ret != 0){
        fingerprint.ctrlLED(fingerprint.eFastBlink, fingerprint.eLEDGreen, 2);
        Serial.print("Got one: ");
        Serial.println(ret);
        
        digitalWrite(12,0);
        digitalWrite(11,250);
        delay(1500);
        Serial.println("opened!");
      }
      else{
        fingerprint.ctrlLED(fingerprint.eKeepsOn, fingerprint.eLEDRed, 0);
        Serial.println("Matching failed");
      }
    }
    else{
      Serial.println("Try again..");
    }
  }
}

void openCap() {
//  digitalWrite(11, 20);
//  delay(500);
//  digitalWrite(11,0);
//  delay(500);
//  digitalWrite(12, 20);
//  delay(500);
//  digitalWrite(12,0);
//  delay(500);
//
  if(Serial.available()){

    char c = Serial.read();

    if(c == '0'){
      digitalWrite(11,250);
      delay(500);
      digitalWrite(11,0);
    }
    if(c == '1'){
      digitalWrite(12,250);
      delay(600);
      digitalWrite(12,0);
    }
  }
}
