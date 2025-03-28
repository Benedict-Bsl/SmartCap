#include "WirelessConnect.h"

//String desc;

void setup(){

//  DDRD &= ~(1 << IRQ);
//  PORTB != (1 << IRQ);
  /*Init print serial*/
  Serial.begin(9600);
  D1Mini.begin(9600);
  Serial.println("initializing wifi connectivity....");
  /*Init FPSerial*/
  FPSerial.begin(115200);
  /*Take FPSerial as communication serial of fingerprint module*/
  fingerprint.begin(FPSerial);

  pinMode(MOTOR_OUTPUT, OUTPUT);
  /*Wait for Serial to open*/
  while(!Serial);
  /*Test whether the device can properly communicate with mainboard
    Return true or false
    */
  while(fingerprint.isConnected() == false){
    Serial.println("Communication with device failed, please check connection");
    /*Get error code information*/
    //desc = fingerprint.getErrorDescription();
    //Serial.println(desc);
    delay(1000);
  }
  pinMode(4,OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  
  Serial.println("connected!");
  Serial.println("interrupt set");
  intSensor();

  attachInterrupt(digitalPinToInterrupt(3), trigger, RISING);
  
}

//Blue LED Comparison mode  Yellow LED Registration mode  Red Deletion mode 
void loop(){

  if(flag==1){

    Serial.println("Sensed your finger!");
        FPSerial.listen();
//        noInterrupts();
        fingerMatch();
        flag = 0;
//    interrupts();
  }
//  else{
    sikiliza();
    
//    Serial.println("nope!");
//  }
  
//  if(digitalRead(IRQ)){
//    uint16_t i = 0;
//    /*Capture fingerprint image, 5s idle timeout, if timeout=0,Disable  the collection timeout function
//      Return 0 if succeed, otherwise return ERR_ID809
//     */
//    if((fingerprint.collectionFingerprint(/*timeout=*/5)) != ERR_ID809){
//      /*Get the time finger pressed down*/
//      /*Set fingerprint LED ring mode, color, and number of blinks 
//        Can be set as follows:
//        Parameter 1:<LEDMode>
//        eBreathing   eFastBlink   eKeepsOn    eNormalClose
//        eFadeIn      eFadeOut     eSlowBlink   
//        Paramerer 2:<LEDColor>
//        eLEDGreen  eLEDRed      eLEDYellow   eLEDBlue
//        eLEDCyan   eLEDMagenta  eLEDWhite
//        Parameter 3:<number of blinks> 0 represents blinking all the time
//        This parameter will only be valid in mode eBreathing, eFastBlink, eSlowBlink
//       */
//      fingerprint.ctrlLED(/*LEDMode = */fingerprint.eFastBlink, /*LEDColor = */fingerprint.eLEDBlue, /*blinkCount = */3);  //blue LED blinks quickly 3 times, means it's in fingerprint comparison mode now
//      /*Wait for finger to relase */
//      while(fingerprint.detectFinger()){
//        delay(50);
//        i++;
//        if(i == 15){             //Yellow LED blinks quickly 3 times, means it's in fingerprint regisrtation mode now
//          /*Set fingerprint LED ring to always ON in yellow*/
//          fingerprint.ctrlLED(/*LEDMode = */fingerprint.eFastBlink, /*LEDColor = */fingerprint.eLEDYellow, /*blinkCount = */3);
//        }else if(i == 30){      //Red LED blinks quickly 3 times, means it's in fingerprint deletion mode now 
//          /*Set fingerprint LED ring to always ON in red*/
//          fingerprint.ctrlLED(/*LEDMode = */fingerprint.eFastBlink, /*LEDColor = */fingerprint.eLEDRed, /*blinkCount = */3);
//        }
//      }
//    }
//    if(i == 0){
//      /*Fingerprint capturing failed*/
//    }else if(i > 0 && i < 15){
//      Serial.println("Enter fingerprint comparison mode");
//      /*Compare fingerprints*/
//      fingerprintMatching();
//    }else if(i >= 15 && i < 30){
//      Serial.println("Enter fingerprint registration mode");
//      /*Registrate fingerprint*/
//      fingerprintRegistration();
//    }else{
//      Serial.println("Enter fingerprint deletion mode");
//      /*Delete this fingerprint*/
//      fingerprintDeletion();
//    }
//  }
}

void trigger(){
  flag = 1;
}
void fingerMatch(){
  uint8_t ret = 0;
  fingerprint.ctrlLED(fingerprint.eBreathing, fingerprint.eLEDBlue, 0);
  Serial.println("Please put your finger on the fingerPrint sensor");

  if((fingerprint.collectionFingerprint(0) != ERR_ID809)){

    // while collecting fingerprint, sent fingerprint mode to quick blink
    fingerprint.ctrlLED(fingerprint.eFastBlink, fingerprint.eLEDYellow, 3);
    Serial.println("Capturing succeds");
    Serial.println("release finger");

    ret = fingerprint.search();

    if(ret != 0){
      fingerprint.ctrlLED(fingerprint.eKeepsOn, fingerprint.eLEDGreen, 0);
      Serial.print("Got one: ");
      Serial.println(ret);
//      fingerPrintAccepted = 1;
        Serial.println(F("Writing to motor"));
        digitalWrite(12, 0);
      //  delay(3000);
        digitalWrite(11,100);
        delay(200);
        digitalWrite(12, 0);
        digitalWrite(11, 0);
        delay(10000);
        digitalWrite(12,100);
        digitalWrite(11, 0);
        delay(100);
        digitalWrite(12, 0);
        digitalWrite(11, 0);
        delay(500);
        while(!somaNumber()){
          delay(250);
        }
    }
    else{
      fingerprint.ctrlLED(fingerprint.eKeepsOn, fingerprint.eLEDRed, 0);
      Serial.println("Matching failed");
//      fingerPrintAccepted = 0;
    }
  }
  else{
    Serial.println("Try again..");
  }
  fingerprint.ctrlLED(/*LEDMode = */fingerprint.eNormalClose, /*LEDColor = */fingerprint.eLEDBlue, /*blinkCount = */0);
  
}

//Compare fingerprints
void fingerprintMatching(){
  /*Compare the captured fingerprint with all fingerprints in the fingerprint library
    Return fingerprint ID number(1-80) if succeed, return 0 when failed
   */
  uint8_t ret = fingerprint.search();
  if(ret != 0){
    /*Set fingerprint LED ring to always ON in green*/
    fingerprint.ctrlLED(/*LEDMode = */fingerprint.eKeepsOn, /*LEDColor = */fingerprint.eLEDGreen, /*blinkCount = */0);
    Serial.print("Successfully matched,ID=");
    Serial.println(ret);
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
    
  }else{
    /*Set fingerprint LED Ring to always ON in red*/
    fingerprint.ctrlLED(/*LEDMode = */fingerprint.eKeepsOn, /*LEDColor = */fingerprint.eLEDRed, /*blinkCount = */0);
    Serial.println("Matching failed");
  }
  delay(1000);
  /*Turn off fingerprint LED Ring*/

  Serial.println("-----------------------------");
}

//Fingerprint Registration
void fingerprintRegistration(){
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


//Fingerprint deletion
void fingerprintDeletion(){
  uint8_t ret;
  /*Compare the captured fingerprint with all the fingerprints in the fingerprint library 
    Return fingerprint ID(1-80) if succeed, return 0 when failed 
   */
  ret = fingerprint.search();
  if(ret){
    /*Set fingerprint LED ring to always ON in green*/
    fingerprint.ctrlLED(/*LEDMode = */fingerprint.eKeepsOn, /*LEDColor = */fingerprint.eLEDGreen, /*blinkCount = */0);
    fingerprint.delFingerprint(ret);
    Serial.print("deleted fingerprint,ID=");
    Serial.println(ret);
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
