#include "WirelessConnect.h"

//String desc;
uint8_t ret;
void setup() {

  //  DDRD &= ~(1 << IRQ);
  //  PORTB != (1 << IRQ);
  /*Init print serial*/
  Serial.begin(9600);
  D1Mini.begin(9600);
  Serial.println("initializing ....");
  //intializeFP sensor
  initFP();
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(A0, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(8, OUTPUT);
  Serial.println("connected!");
  Serial.println("interrupt set");

  attachInterrupt(digitalPinToInterrupt(IRQ), trigger, FALLING);

}

//Blue LED Comparison mode  Yellow LED Registration mode  Red Deletion mode
void loop() {

  int select = sikiliza();

  if(select == 2){
    Serial.println("Selected delete");
    deleteFingerprint();
  }
  if (flag == 1) {
    Serial.println("Sensed your finger!");
    mySerial.listen();
    fingerPrint(select);
    delay(1500);
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
//  fingerprint.ctrlLED(fingerprint.eBreathing, fingerprint.eLEDBlue, 0);
  Serial.println("Please put your finger on the fingerPrint sensor");

//  if ((fingerprint.collectionFingerprint(0) != ERR_ID809)) {

  switch (slct){
    case 3:
      while(!registerFingerprint());
      break;
    default:
      fingerMatch();
  }

//  }
//  else {
//    Serial.println("Try again..");
//  }
//  fingerprint.ctrlLED(/*LEDMode = */fingerprint.eNormalClose, /*LEDColor = */fingerprint.eLEDBlue, /*blinkCount = */0);
  
}

//Fingerprint Registration
uint8_t registerFingerprint(){
  finger.LEDcontrol(FINGERPRINT_LED_GRADUAL_ON, 100, FINGERPRINT_LED_PURPLE);
  int p = -1;
  Serial.println("starting reg...");
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(fpNumber);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  finger.LEDcontrol(FINGERPRINT_LED_FLASHING, 25, FINGERPRINT_LED_RED, 10);

  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(fpNumber+1);
  p = -1;
  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  Serial.print("Creating model for #");  Serial.println(fpNumber+1);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  Serial.print("ID "); Serial.println(fpNumber+1);
  p = finger.storeModel(fpNumber+1);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }
  finger.LEDcontrol(FINGERPRINT_LED_FLASHING, 25, FINGERPRINT_LED_BLUE, 10);
  return true;
}

uint32_t st = 0;
void deleteFingerprint(){
  Serial.println("Stating delete...");
  uint8_t p = -1;

  for(int i = 1; i <= 5; i ++){

    p = finger.deleteModel(i);
      
    if (p == FINGERPRINT_OK) {
      Serial.print(i);Serial.println(": Deleted!");
    } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
      Serial.println("Communication error");
    } else if (p == FINGERPRINT_BADLOCATION) {
      Serial.println("Could not delete in that location");
    } else if (p == FINGERPRINT_FLASHERR) {
      Serial.println("Error writing to flash");
    } else {
      Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
    }
   
  }

//  if (finger.verifyPassword()) {
//    Serial.println("Found fingerprint sensor!");
//  } else {
//    Serial.println("Did not find fingerprint sensor :(");
//    while (1);
//  }
//
//  finger.emptyDatabase();
//  finger.LEDcontrol(FINGERPRINT_LED_GRADUAL_ON, 100, FINGERPRINT_LED_RED);
//  delay(1500);
//  finger.LEDcontrol(FINGERPRINT_LED_GRADUAL_OFF, 100, FINGERPRINT_LED_RED);
//  delay(1500);
//  Serial.println("Now database is empty :)");
}

void fingerMatch(){
  uint32_t pt = millis();

  
  finger.LEDcontrol(FINGERPRINT_LED_FLASHING, 25, FINGERPRINT_LED_PURPLE, 5);
 
  uint8_t p = finger.getImage();
  delay(100);
  
  while(p!=FINGERPRINT_OK){
    Serial.println(digitalRead(3));
    uint32_t ct = millis();
    uint32_t dif = ct-pt;
    uint32_t wt = 1500;
    Serial.print("ct " );Serial.println(ct);
    Serial.print("dif " );Serial.println(dif);
    Serial.print("wt " );Serial.println(wt);
    Serial.print("pt " );Serial.println(pt);
    delay(50);
    if(p==FINGERPRINT_PACKETRECIEVEERR){
      break;
    }

    if(dif>=wt){
      pt=ct;
      Serial.println("No fingerprint on sensor");
      break;
    }

    p = finger.getImage();
  }
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  finger.LEDcontrol(FINGERPRINT_LED_BREATHING, 100, FINGERPRINT_LED_BLUE, 5);

  if (finger.fingerID != 0) {
//    fingerprint.ctrlLED(fingerprint.eKeepsOn, fingerprint.eLEDGreen, 0);
    Serial.print("Got one: ");
    Serial.println(ret);
    //      fingerPrintAccepted = 1;
    Serial.println(F("Writing to motor"));
    digitalWrite(8, 100);
    //  delay(3000);
    digitalWrite(9, 0);
    delay(150);
    digitalWrite(8, 0);
    digitalWrite(9, 0);
    delay(10000);
    digitalWrite(8, 0);
    digitalWrite(9, 100);
    delay(100);
    digitalWrite(8, 0);
    digitalWrite(9, 0);
    delay(500);
    Serial.println(F("dispenser"));
    analogWrite(A0, 250);
    digitalWrite(4,0);
    analogWrite(5, 200);
    D1Mini.listen();
    while(ck!='5'){
      digitalWrite(4,0);
      analogWrite(5, 90);
      delay(100);
      digitalWrite(4,0);
      analogWrite(5, 0);
      delay(200);
      digitalWrite(4,0);
      analogWrite(5, 90);
      delay(100);
      digitalWrite(4,0);
      analogWrite(5, 0);
      delay(200);
      digitalWrite(4,1);
      digitalWrite(5, 0);
      delay(30);
      digitalWrite(4,0);
      digitalWrite(5, 0);
      delay(200);
      digitalWrite(4,0);
      analogWrite(5, 90);
      delay(100);
      digitalWrite(4,0);
      analogWrite(5, 0);
      delay(200);
      digitalWrite(4,0);
      analogWrite(5, 90);
      delay(100);
      digitalWrite(4,0);
      analogWrite(5, 0);
      delay(200);
      digitalWrite(4,1);
      digitalWrite(5, 0);
      delay(30);
      digitalWrite(4,0);
      digitalWrite(5, 0);
      ck = D1Mini.read();
      if(ck == '5'){
        digitalWrite(4,0);
        analogWrite(5,0);
        delay(250);
        break;
      }
    } 
    
  }
  else {
//    fingerprint.ctrlLED(fingerprint.eKeepsOn, fingerprint.eLEDRed, 0);
    Serial.println("Matching failed");
    //      fingerPrintAccepted = 0;
  }
}


