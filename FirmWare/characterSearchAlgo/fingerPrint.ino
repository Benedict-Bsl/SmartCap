
void initFP(){

  FPSerial.begin(115200);
  FPSerial.listen();

  fingerprint.begin(FPSerial);

  while(!Serial);

  while(fingerprint.isConnected() == false){

    Serial.println(F("not communicating with device"));

    delay(1000);
  }
  Serial.println(F("Device connected!"));
}

void trigger(){
  flag = 1;
}
void fingerMatch(){
  uint8_t ret = 0;
  fingerprint.ctrlLED(fingerprint.eBreathing, fingerprint.eLEDBlue, 0);
  Serial.println(F("Please put your finger on the fingerPrint sensor"));

  if((fingerprint.collectionFingerprint(0) != ERR_ID809)){

    // while collecting fingerprint, sent fingerprint mode to quick blink
    fingerprint.ctrlLED(fingerprint.eFastBlink, fingerprint.eLEDYellow, 3);
    Serial.println(F("Capturing succeds"));
    Serial.println(F("release finger"));

    while(fingerprint.detectFinger());
    Serial.println(F("FingerReleased"));
    //search for the desired fingerprint
    ret = fingerprint.search();

    if(ret != 0){
      fingerprint.ctrlLED(fingerprint.eKeepsOn, fingerprint.eLEDGreen, 0);
      Serial.print(F("Got one: "));
      Serial.println(ret);
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
      Serial.println(F("Matching failed"));
     fingerPrintAccepted = 0;
    }
  }
  else{
    fingerprint.ctrlLED(0, 0, 0);
    Serial.println(F("Try again.."));
  }
  
}

