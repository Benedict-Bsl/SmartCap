#include <SoftwareSerial.h>

//SoftwareSerial gsm(14,12);
SoftwareSerial gsm(26,27);

void setup(){
  Serial.begin(9600);
//  pinMode(1,FUNCTION_3);
  gsm.begin(9600);
 
}
void loop() {
//
    if (Serial.available()) {
      gsm.write(Serial.read());
  
//          Serial.println(Serial.read());
    }
//  
  
    if (gsm.available()) {
      Serial.write(gsm.read());
    }

  
}
