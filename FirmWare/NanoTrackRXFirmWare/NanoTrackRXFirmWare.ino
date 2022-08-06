#include "WirelessConnect.h"

//String desc;
uint8_t ret;
void setup() {

  //  DDRD &= ~(1 << IRQ);
  //  PORTB != (1 << IRQ);
  /*Init print serial*/
  Serial.begin(115200);
  D1Mini.begin(9600);
  Serial.println("initializing ....");
  /*Init FPSerial*/
  FPSerial.begin(115200);
  /*Take FPSerial as communication serial of fingerprint module*/
  fingerprint.begin(FPSerial);
  /*Wait for Serial to open*/
  while (!Serial);
  /*Test whether the device can properly communicate with mainboard
    Return true or false
  */
  while (fingerprint.isConnected() == false) {
    Serial.println("Communication with device failed, please check connection");
    /*Get error code information*/
    //desc = fingerprint.getErrorDescription();
    //Serial.println(desc);
    delay(1000);
  }
  pinMode(4, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(8, OUTPUT);
  Serial.println("connected!");
  Serial.println("interrupt set");

  attachInterrupt(digitalPinToInterrupt(IRQ), trigger, RISING);

}

//Blue LED Comparison mode  Yellow LED Registration mode  Red Deletion mode
void loop() {

  int select = sikiliza();
  if (flag == 1) {

    Serial.println("Sensed your finger!");
    FPSerial.listen();
    fingerPrint(select);
    flag = 0;
  }


//  if(somaNumber()) {
//    Serial.println("Writing to d1..");
//    D1Mini.write("1111");
//    delay(250);
//  }
}

void trigger() {
  flag = 1;
}
void fingerPrint(int slct) {
  ret = 0;
  fingerprint.ctrlLED(fingerprint.eBreathing, fingerprint.eLEDBlue, 0);
  Serial.println("Please put your finger on the fingerPrint sensor");

  if ((fingerprint.collectionFingerprint(0) != ERR_ID809)) {

    switch (slct){
      case 2:
        deleteFingerprint(5);
        break;
      case 3:
        registerFingerprint();
        break;
      default:
        fingerMatch();
    }

  }
  else {
    Serial.println("Try again..");
  }
  fingerprint.ctrlLED(/*LEDMode = */fingerprint.eNormalClose, /*LEDColor = */fingerprint.eLEDBlue, /*blinkCount = */0);

}

//Fingerprint Registration
void registerFingerprint(){
  uint8_t ID,i;
  /*Compare the captured fingerprint with all fingerprints in the fingerprint library
    Return fingerprint ID number(1-80) if succeed, return 0 when failed
    Function: clear the last captured fingerprint image
   */
  fingerprint.search();       //Can add "if else" statement to judge whether the fingerprint has been registered. 
  /*Get a unregistered ID for saving fingerprint 
    Return ID number when succeed 
    Return ERR_ID809 if failed
   */
  if((ID = fingerprint.getEmptyID()) == ERR_ID809){
    while(1){
      /*Get error code imformation*/
      //desc = fingerprint.getErrorDescription();
      //Serial.println(desc);
      delay(1000);
    }
  }
  Serial.print("Unregistered ID,ID=");
  Serial.println(ID);
  i = 0;   //Clear sampling times 
  /*Fingerprint Sampling 3 times */
  while(i < COLLECT_NUMBER){
    /*Set fingerprint LED ring to breathing lighting in blue*/
    fingerprint.ctrlLED(/*LEDMode = */fingerprint.eBreathing, /*LEDColor = */fingerprint.eLEDBlue, /*blinkCount = */0);
    Serial.print("The fingerprint sampling of the");
    Serial.print(i+1);
    Serial.println("(th) time is being taken");
    Serial.println("Please press down your finger");
    /*Capture fingerprint image, 10s idle timeout 
      If succeed return 0, otherwise return ERR_ID809
     */
    if((fingerprint.collectionFingerprint(/*timeout = */10)) != ERR_ID809){
      /*Set fingerprint LED ring to quick blink in yellow 3 times*/
      fingerprint.ctrlLED(/*LEDMode = */fingerprint.eFastBlink, /*LEDColor = */fingerprint.eLEDYellow, /*blinkCount = */3);
      Serial.println("Capturing succeeds");
      i++;   //Sampling times +1 
    }else{
      Serial.println("Capturing fails");
      /*Get error code information*/
      //desc = fingerprint.getErrorDescription();
      //Serial.println(desc);
    }
    Serial.println("Please release your finger");
    /*Wait for finger to release
      Return 1 when finger is detected, otherwise return 0 
     */
    while(fingerprint.detectFinger());
  }
  
  /*Save fingerprint information into an unregistered ID*/
  if(fingerprint.storeFingerprint(/*Empty ID = */ID) != ERR_ID809){
    Serial.print("Saving succeed，ID=");
    Serial.println(ID);
    /*Set fingerprint LED ring to always ON in green*/
    fingerprint.ctrlLED(/*LEDMode = */fingerprint.eKeepsOn, /*LEDColor = */fingerprint.eLEDGreen, /*blinkCount = */0);
    delay(1000);
    /*Turn off fingerprint LED ring */
    fingerprint.ctrlLED(/*LEDMode = */fingerprint.eNormalClose, /*LEDColor = */fingerprint.eLEDBlue, /*blinkCount = */0);
  }else{
    Serial.println("Saving failed");
    /*Get error code information*/
    //desc = fingerprint.getErrorDescription();
    //Serial.println(desc);
  }
  Serial.println("-----------------------------");
}

void deleteFingerprint(uint8_t id){
  /*Compare the captured fingerprint with all the fingerprints in the fingerprint library 
    Return fingerprint ID(1-80) if succeed, return 0 when failed 
   */
    if(id){
      /*Set fingerprint LED ring to always ON in green*/
      fingerprint.ctrlLED(/*LEDMode = */fingerprint.eKeepsOn, /*LEDColor = */fingerprint.eLEDGreen, /*blinkCount = */0);

      for(ret = 1; ret < id; ret ++){
        if(fingerprint.verify(ret)>0){
          fingerprint.delFingerprint(ret);
          Serial.print("deleted fingerprint,ID=");
          Serial.println(ret);
        }
      }
    }else{
      /*Set fingerprint LED ring to always ON in red*/
      fingerprint.ctrlLED(/*LEDMode = */fingerprint.eKeepsOn, /*LEDColor = */fingerprint.eLEDRed, /*blinkCount = */0);
      Serial.println("Matching failed or the fingerprint hasn't been registered");
    }
    delay(1000);
    /*Turn off fingerprint LED ring*/
    fingerprint.ctrlLED(/*LEDMode = */fingerprint.eNormalClose, /*LEDColor = */fingerprint.eLEDBlue, /*blinkCount = */0);
    Serial.println("-----------------------------");
}
void fingerMatch(){
  // while collecting fingerprint, sent fingerprint mode to quick blink
  fingerprint.ctrlLED(fingerprint.eFastBlink, fingerprint.eLEDYellow, 3);
  Serial.println("Capturing succeds");
  Serial.println("release finger");

  ret = fingerprint.search();

  if (ret != 0) {
    fingerprint.ctrlLED(fingerprint.eKeepsOn, fingerprint.eLEDGreen, 0);
    Serial.print("Got one: ");
    Serial.println(ret);
    //      fingerPrintAccepted = 1;
    Serial.println(F("Writing to motor"));
    digitalWrite(8, 0);
    //  delay(3000);
    digitalWrite(9, 100);
    delay(200);
    digitalWrite(8, 0);
    digitalWrite(9, 0);
    delay(10000);
    digitalWrite(8, 100);
    digitalWrite(9, 0);
    delay(100);
    digitalWrite(8, 0);
    digitalWrite(9, 0);
    delay(500);

  }
  else {
    fingerprint.ctrlLED(fingerprint.eKeepsOn, fingerprint.eLEDRed, 0);
    Serial.println("Matching failed");
    //      fingerPrintAccepted = 0;
  }
}


