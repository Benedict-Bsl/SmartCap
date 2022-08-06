bool db = true;

#include <SoftwareSerial.h>
#include "DFRobot_ID809.h"
//#include <APDS9930.h>

#define COLLECT_NUMBER 3  //Fingerprint sampling times, can be set to 1-3
#define IRQ         3  //IRQ pin 

//D1MINI 10,9
//fp 6,5
SoftwareSerial Serial1(11,10);
SoftwareSerial D1Mini(12,13);

#define FPSerial Serial1
//APDS9930 apds = APDS9930();
uint16_t proximity_data = 0;

DFRobot_ID809 fingerprint;


bool FingerPrint;
bool mtCtrl = false;

int vn;
int m = 0;
char c;
//String v;


//finger print
int comparison_flag = 0;
int registration_flag = 0;
int deletion_flag = 0;
int fingerPrintAccepted = 0;

int volatile flag = 0;

void trigger();

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



char x;
char ck='0';
int sikiliza(){
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
  delay(100);
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
    else if(ck == '2'){
      Serial.println(F("FP deletion subroutine"));
      return 2;
    }

    else if(ck == '3'){
      Serial.println(F("FP registration subroutine"));
      return 3;
    }
    else if(mtCtrl){
      Serial.println(F("Bottom locking mechanism.."));
      digitalWrite(7,1);
      digitalWrite(6,0);
      delay(150);
      digitalWrite(7,0);
      digitalWrite(6,0); 
      delay(15000);
      Serial.println(F("Back again..."));
      digitalWrite(7,0);
      digitalWrite(6,1);
      delay(130);
      digitalWrite(7,0);
      digitalWrite(6,0); 
      delay(10000);

      mtCtrl = false;
    }
    else if(ck=='0'){
      digitalWrite(4,0);  
      digitalWrite(7,0);
      digitalWrite(6,0);
    }
  return 0;

}

