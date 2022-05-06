#include <Arduino.h>
#include <SmartCap.h>

int main(void){

  init();
  DDRB |= (1 << 10); // pin 10 as OUTPUT(buzzer)
  // PORTB |= (1 << 10); 
  DDRB |= (1 << 12); // pin 12 as OUTPUT(motor)
  // PORTB |= (1 <<12);
  DDRB |= (1 << 11); //pin 11 as OUTPUT(motor)
  // PORTB |= (1<<11); 
  DDRB &= (1 << 3); // pin 4 as input
  // set pin 4 as OUTPUT
  pinMode(4, OUTPUT);

  //generate interrupt on risizing edge of interupt 1
  EICRA |= (1 << ISC11);
  EICRA &= ~(1 << ISC10);

  EIMSK |= (1 << INT1);

  //enable interrupts
  // sei();



  if(db){Serial.begin(9600);}
  initFP();
  if(db){Serial.println(F("Starts of the program"));}
  sGps.begin(9600);
  SoftwareSerial* gsm = new SoftwareSerial(7,8);
  gsm->begin(9600);
  // delay(1000);
  sim800c = new SIM800L((Stream *)gsm, 1, 200, 512);
  // setupModule();

  while(!sim800c->isReady()){
    if(db){Serial.println(F("Problem to connect wiht AT commands"));}
  }
  uint8_t signal = sim800c->getSignal();

  while(signal <= 0){
    signal = sim800c->getSignal();
  } 

  //reest if not connected
  if(db){Serial.print(F("Signal: "));Serial.println(signal);}
  NetworkRegistration net = sim800c->getRegistrationStatus();

  while(net!=REGISTERED_HOME && net!= REGISTERED_ROAMING){
    net = sim800c->getRegistrationStatus();
  }
  if(db){Serial.println(F("Registered"));}
  //internet connection
  bool sucss = sim800c->setupGPRS("Internet");
  while(!sucss){
    sucss = sim800c->setupGPRS("internet");
    customDelay(5000);
  }
  if(db){Serial.print(F("disconnect :"));}
  if(db){Serial.println(sim800c->disconnectGPRS());}

  bool connected = sim800c->connectGPRS();
  // connect to internet
  if(connected){ if(db){Serial.println(F("connected with ip: "));Serial.println(sim800c->getIP());}}
  else{if(db){Serial.println(F("Not connected"));}}

  const char tkn[] = "device_id=123456";
  if(db){Serial.println(F("geting token"));}
  uint8_t rt = sim800c->doPost(ATH_URL, content_appCoded,tkn, 10000, 10000);

  if(rt == 200){
      if(db){Serial.print(F("post successful: "));}
      const size_t cap = sim800c->getDataSizeReceived();
      if(db){Serial.print(cap);}
      if(db){Serial.println(F(" bytes"));}
      if(db){Serial.println(F("Received: "));}
      // Serial.println(sim800c->getDataReceived());
      const String jsonRespone = sim800c->getDataReceived();
      uint8_t val = jsonExtract(jsonRespone, "initialQuantity").toInt();
      if(db){Serial.println(val);}
      if(db){Serial.println(F(" got"));}
      delay(200);
      if(db){Serial.println(sim800c->getDataReceived());}
      if(db){Serial.println(jsonRespone);}
    }//else{
    intSensor();
  // }
  // http();

  //====== START THE LOOP ROUTINE HERE============
  while(true){
    //==== read gps values
    /*
    readGPS();
    if(db){Serial.print(F("gps lat; "));Serial.println(gps_lat);}
    if(db){Serial.print(F("gsp long; "));Serial.println(gps_lon);}
    if(db){Serial.println(F("Posting http content"));}
    // Datax = "id="+String(FingerPrint)+"&f="+String(pillsQuantity)+"&q="+String(pillsQuantity)+"&lat="+String(longitude, 7)+"&lon="+String(latitude, 7);
    // const char data[] = "{\"device_id\":123456}";

    //==== get value from server
    const char data[] = "value=&token=720b703c-a433-4c1f-92f4-fd6c49e63ec1";
    uint16_t rc = sim800c->doPost(buzz, content_appCoded,data, 15000, 15000); // post values with 10sec timeout
    int val=0;
    if(rc == 200){
      if(db){Serial.print(F("post successful: "));}
      const size_t cap = sim800c->getDataSizeReceived();
      if(db){Serial.print(cap);}
      if(db){Serial.println(F(" bytes"));}
      if(db){Serial.println(F("Received: "));}
      // Serial.println(sim800c->getDataReceived());

      // if(db){Serial.println(sim800c->getDataReceived());}
      // check for online status.
      // customDelay(2000);
      
      String val = jsonExtract(sim800c->getDataReceived(), "value");
      if(db){Serial.print(F("buzzer val: "));Serial.println(val);}
      if(val.toInt() == 1){
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
      }else{
        digitalWrite(4, 0);
      }
      
    }else{
      if(db){Serial.print(F("Failed ot post error: "));}
      if(db){Serial.println(rc);}
    }*/
    sens();

    if(sv >= 400){

    }
    if(flag == 1){
      if(db){Serial.print(F("Starting fp identificaiton..."));}
      FPSerial.listen();
      fingerMatch();
      flag = 0;

    }

    // if(flag2 == 1){
    //   sens();
    // }


  }
  
}

ISR (INT1_vect){
  flag = 1;
}