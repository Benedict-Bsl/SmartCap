#include "SmartCap.h"
#define timerPreload 80000000
#define timeTowait 5

#define BUZZER_PIN 5
#define BUZZER_CHANNEL 0

int removed_d=0;
uint16_t rt;
uint16_t rc;
http con= http();
volatile boolean flag = false;
uint16_t proximity_data = 0;
volatile boolean flag2 = false;
bool sta; 
bool flag_id = false; // Signed to a pill to check the difference by which one pill is preceeded by the next
uint32_t prev;
uint32_t dettectionTime_t;
//updater flag
bool updater = false;
char ck='0';
int quantity2;
void setup(){
  
  // put your setup code here, to run once:
  Serial.begin(9600);
  sGps.begin(9600);
  stream->begin(9600);
  nano.begin(9600);
  pinMode(APDS9930_INT, INPUT);
  pinMode(33, OUTPUT);
  digitalWrite(33, HIGH);
  delay(200);
//  pinMode(TX, FUNCTION_3);
//  pinMode(TX,INPUT);

  //   readGPS();
  //init GSM
  //  initGsm();
//  Serial.println(F("Get extra mem"));
  //    Serial.println(F("AT commands start"));
  readGPS();
  delay(10000);
  if (!con.isReady()) {
    Serial.println(F("MT not ready"));
  }
  Serial.println(F("AT commands ready Now!"));
  Serial.println(con.getSignal());
  net();
  intSensor();
  // Initiate HTTP/S session with the module

  
  char tkn[] = "device_id=888888";
  //Serial.println(ATH_URL);
  strcat(ATH_URL,tkn);
  Serial.print(F("posting at:"));
  Serial.println(ATH_URL);
  rt = con.doPostD(ATH_URL, content_appCoded, 10000, 10000);
  while (rt != 200) {
    Serial.println(F("."));
    rt = con.doPostD(ATH_URL, content_appCoded, 10000, 10000);
    Serial.println(rt);
    if (rt >= 300 && rt <= 710) {
      Serial.println(F("Reconnecting..."));
      reconnect(apn);
    }
    Serial.print(rt);
    Serial.println(F("couldn't post"));
  }
  Serial.print(F(" data size: ")); Serial.println(con.getDataSizeReceived());
  const String jsResp = con.getDataReceived();
  quantity = jsonExtract(jsResp, "initialQuantity").toInt();
  t = jsonExtract(jsResp, "token");
  
  Serial.print(F("pills quantity: ")); Serial.println(quantity);
  Serial.print(F("token: ")); Serial.println(t);
  attachInterrupt(13, sens, FALLING);
  //sensing enclosure opened.
  attachInterrupt(33, trigger, RISING);
  //sensing object dettection

  
  Serial.println("Interrupt should start");
  sta = true;
  quantity2 = quantity;
//  interrupts();
}

void loop() {
  prev = 0;
  dettectionTime_t = 0;
  if(flag){
    Serial.println(F("cap opened..."));
    /* if the cap is opened, start the sensor:
     *  Do not do anything else than just checking pill dispense
     *  - flag2 is sensor
     *  - flag is cap opening(Triggerer) Closed and Opened
     *  
     *  if cap is opened, then, it means fP has been accepted,
     *  that means, the opening Trigger value has changed.
     *  if it has changed,:
     *  - Start running the sensor
     *  - init a value called checker, make it true
     *  - flag of trigger back to zero
     *  - if flag of trigger back to zero, changes back, and 
     *  the operation is still in while loops, exit while loop,
     *  since the cap has been closed(Trigger value changed)
     *  
     *  
     */
    Serial.println("Cap opened");
    flag = false;
    bool checker = true;
    while(checker){
       if(digitalRead(33) == LOW){
        nano.write("5");
        Serial.println("Wrote 5 to nano");
        checker = false;
        Serial.println("Cap closed");
      }
      Serial.println(F("checkder..."));
      delay(250);

      //Time before pill is out
      //time 
      if(flag2){
        //record the value to be substructed from the initial quantity
        int returnedValue = somaNumber();
        
        //time when something is dettected going out.
        uint32_t trigT = millis();// this time
        
        Serial.print("prev Dettection: ");Serial.println(dettectionTime_t);
        Serial.print("current Dettect now: ");Serial.println(trigT);
        //time difference since last dettection
        uint32_t interv = trigT - dettectionTime_t;
        
        // allowed difference in time so as to substract from initial quantity
        uint32_t allowed_difference = 266;

        if(interv > allowed_difference){
          Serial.print("differ time: ");Serial.println(interv);
          Serial.print("Previous quantity: ");
          Serial.println(quantity2);
          Serial.print("current ");
          Serial.println(quantity);
          quantity = quantity - returnedValue;
        // send 0 if everyThing has been removed quantity
          if(quantity <= 0){
            quantity = 0;
            //removed_d+1;
            removed_d = 0;       
          }
          updater = true;
        }
        Serial.println("one");
      }

      if(flag_id){
        // when dettection occur, record the time
        dettectionTime_t = millis();
        flag_id = false;
      }
      
      else{
      }

    }
  }

  if(updater){
    /*updater will be run only when the buzzer is triggered or 
    some pills have just been taken out of the bottle*/
    
    Serial.println(F("updating cap"));
    updateCap();
    updater = false;
  }else{
    
    Serial.println(F("Buzzer"));
    buzzer();
  }
  if(sta){
    nano.write("R");
    sta = false;
  }
}

ICACHE_RAM_ATTR void sens(){
  flag2 = true;
//  quantity = quantity - ((int)somaNumber());  
}
ICACHE_RAM_ATTR void trigger(){
  flag = true;
  
}
void updateCap(){
  
  //update cup
  Serial.println(F("updateCup"));
  String dt = "finger_print=" + t + "&quantity=" + String(quantity) +
              "&lat=" + String(gps_lat, 7) + "&lon=" + String(gps_lon, 7) + "&token=" + t;

  char dataTosend[(dt.length()) + 1];

  strcpy(dataTosend, dt.c_str());
//  strcat(URL,dt.c_str());
  Serial.print(URL);
  Serial.println(dataTosend);
  rt = con.doPostDU(URL,dataTosend,content_appCoded, 2000, 3000);
  
  while (rt != 200) {

    if (rt >= 300 && rt <= 710) {
      Serial.println(F("Reconnecting..."));
      reconnect(apn);
    }
    if(rt == 403){
      Serial.println(F("no updates..."));
      break;
    }
    rt = con.doPostDU(URL,dataTosend, content_appCoded,5000, 10000);
    Serial.println(rt);
  }
  
  Serial.println(F("updated successfully"));
//  Serial.println(con.getDataSizeReceived());
//  Serial.println(con.getDataReceived());

}

void buzzer() {
  String stat = "value=&token=" + t;
  char ch[(stat.length()) + 1];
  strcpy(ch, stat.c_str());
//  strcat(buzz, ch);
//  Serial.print(F("buzzer at:"));
//  Serial.print(buzz);Serial.println(ch);
  uint16_t rc = con.doPostDU(buzz,ch, content_appCoded,2000, 3000);

  while (rc != 200) {
    //    net();
    if (rt >= 300 && rt <= 710) {
      Serial.println(F("Reconnecting..."));
      reconnect(apn);
    }
    Serial.println(rc);
    rc = con.doPostDU(buzz,ch, content_appCoded,5000, 6000);
  }
//  const size_t cap = getDataSizeReceived();
  buzzinger = jsonExtract(con.getDataReceived(), "value").toInt();
  if (buzzinger == 1) {
    //Read GPS data when buzzer is requested.
    Serial.println(F("GPS data: "));
    sGps.listen();
    readGPS();
    
    //enable updating feature everytime the buzzer is triggered.
    updater = true;
    Serial.println(F("Writing to buzzer"));
    
    while(buzzinger==1){
      tone(BUZZER_PIN, NOTE_G1, 500, BUZZER_CHANNEL);
      noTone(BUZZER_PIN, BUZZER_CHANNEL);
      tone(BUZZER_PIN, NOTE_A2, 500, BUZZER_CHANNEL);
      noTone(BUZZER_PIN, BUZZER_CHANNEL);
      tone(BUZZER_PIN, NOTE_B3, 500, BUZZER_CHANNEL);
      noTone(BUZZER_PIN, BUZZER_CHANNEL);
      tone(BUZZER_PIN, NOTE_G1, 500, BUZZER_CHANNEL);
      noTone(BUZZER_PIN, BUZZER_CHANNEL);
      tone(BUZZER_PIN, NOTE_G1, 500, BUZZER_CHANNEL);
      noTone(BUZZER_PIN, BUZZER_CHANNEL);
      tone(BUZZER_PIN, NOTE_A2, 500, BUZZER_CHANNEL);
      noTone(BUZZER_PIN, BUZZER_CHANNEL);
      tone(BUZZER_PIN, NOTE_B3, 500, BUZZER_CHANNEL);
      noTone(BUZZER_PIN, BUZZER_CHANNEL);
      tone(BUZZER_PIN, NOTE_G1, 500, BUZZER_CHANNEL);
      noTone(BUZZER_PIN, BUZZER_CHANNEL);
      tone(BUZZER_PIN, NOTE_B2, 500, BUZZER_CHANNEL);
      noTone(BUZZER_PIN, BUZZER_CHANNEL);
      tone(BUZZER_PIN, NOTE_C3, 500, BUZZER_CHANNEL);
      noTone(BUZZER_PIN, BUZZER_CHANNEL);
      tone(BUZZER_PIN, NOTE_D4, 500, BUZZER_CHANNEL);
      noTone(BUZZER_PIN, BUZZER_CHANNEL);
      rc = con.doPostDU(buzz,ch, content_appCoded,5000, 6000);
      while (rc != 200) {
        //    net();
        if (rt >= 300 && rt <= 710) {
          Serial.println(F("Reconnecting..."));
          reconnect(apn);
        }
        Serial.println(rc);
        rc = con.doPostDU(buzz,ch, content_appCoded,5000, 6000);
      }
      buzzinger = jsonExtract(con.getDataReceived(), "value").toInt();
    }

  }else if(buzzinger == 4){
    Serial.println(F("Writing 4 to nano, bottom lock"));
    nano.write("4");
    reuploadZero();
  }else if(buzzinger == 2){
    Serial.println(F("Sending delete fingerprint"));
    nano.write("2");
    reuploadZero();
  }else if(buzzinger == 3){
    Serial.println(F("Sending register fingerprint"));
    nano.write("3");
    reuploadZero();
  }
  else {
    nano.write("0");
//    digitalWrite(16,LOW);
//    nano.write("0");
//    digitalWrite(4, 0);
  }
  Serial.print(F("Buzzer :"));
  Serial.println(buzzinger);

}

void reuploadZero(){
    Serial.println(F("Then reupload 0"));
    stat = "value=0&token=" + t;
    char ch[(stat.length()) + 1];
    strcpy(ch, stat.c_str());
  //  strcat(buzz, ch);
  //  Serial.print(F("buzzer at:"));
  //  Serial.print(buzz);Serial.println(ch);
    uint16_t rc = con.doPostDU(buzz,ch, content_appCoded,2000, 3000);
    while (rc != 200) {
      //    net();
      if (rt >= 300 && rt <= 710) {
        Serial.println(F("Reconnecting..."));
        reconnect(apn);
      }
      Serial.println(rc);
      rc = con.doPostDU(buzz,ch, content_appCoded,5000, 6000);
    }
    Serial.println(F("Successfully reuploaded 0"));
}
void net() {

  NetworkRegistration net = con.getRegistrationStatus();

  while (net != REGISTERED_HOME) {
    net = con.getRegistrationStatus();
  }
  Serial.println(F("Registered"));
  //setup GPRS internet apn
  con.disconnectGPRS();
  bool sucss = con.setupGPRS(apn);
  if(!sucss) {
    Serial.println("Getting internet...");
    sucss = con.setupGPRS(apn);
    delay(3000);
  }


  bool connected = con.connectGPRS();

  if (connected) {
    Serial.print(F("connected with ip"));
    Serial.println(con.getIP());
  }
  else {
    Serial.println(F("not connected"));
  }



}

void reconnect(const char *u){
  con.sendCommand_P(AT_CMD_HTTPTERM);
  if (!con.readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_OK,2)){}
  con.disconnectGPRS();
  con.setupGPRS(u);
  if(con.connectGPRS()){
    Serial.println(con.getIP());
  }
  
}

//void ISR(void){
//  timer0_write(ESP.getCycleCount() + timerPreload * timeTowait);
//  flag = true;
//}

void intSensor() {

  Serial.println(F("------------------------------"));
  Serial.println(F("APDS-9930 - ProximityInterrupt"));
  Serial.println(F("------------------------------"));

  // Initialize APDS-9930 (configure I2C and initial values)
  if ( apds.init() ) {
    Serial.println(F("APDS-9930 initialization complete"));
  } else {
    Serial.println(F("Something went wrong during APDS-9930 init!"));
  }
  
  // Adjust the Proximity sensor gain
  if ( !apds.setProximityGain(PGAIN_2X) ) {
    Serial.println(F("Something went wrong trying to set PGAIN"));
  }
  
  // Set proximity interrupt thresholds
  if ( !apds.setProximityIntLowThreshold(PROX_INT_LOW) ) {
    Serial.println(F("Error writing low threshold"));
  }
  if ( !apds.setProximityIntHighThreshold(PROX_INT_HIGH) ) {
    Serial.println(F("Error writing high threshold"));
  }
  
  // Start running the APDS-9930 proximity sensor (interrupts)
  if ( apds.enableProximitySensor(true) ) {
    Serial.println(F("Proximity sensor is now running"));
  } else {
    Serial.println(F("Something went wrong during sensor init!"));
  }
  if ( !apds.clearProximityInt() ) {
    Serial.println("Error clearing interrupt");
  }

#ifdef DUMP_REGS
  /* Register dump */
  uint8_t reg;
  uint8_t val;

  for(reg = 0x00; reg <= 0x19; reg++) {
    if( (reg != 0x10) && \
        (reg != 0x11) )
    {
      apds.wireReadDataByte(reg, val);
      Serial.print(reg, HEX);
      Serial.print(": 0x");
      Serial.println(val, HEX);
    }
  }
  apds.wireReadDataByte(0x1E, val);
  Serial.print(0x1E, HEX);
  Serial.print(": 0x");
  Serial.println(val, HEX);
#endif
}

uint16_t a = 0;
bool vl= 0;

int somaNumber(){
  int chek;
  // Read proximity level and print it out
  if ( !apds.readProximity(proximity_data) ) {
    Serial.println("Error reading proximity value");
    return 0;
  } else {
    Serial.print("Proximity detected! Level: ");
    Serial.println(proximity_data);
    if(proximity_data >= 200){
      prev = millis();
      chek = 1;

      //flag to record when something passes the sensor
      flag_id = true;
      removed_d+=1;
    }
    else{
      chek = 0;
    }
    
  } 
  
  
  // Reset flag and clear APDS-9930 interrupt (IMPORTANT!)
  flag2 = false;
  
  if ( !apds.clearProximityInt() ) {
    Serial.println("Error clearing interrupt");
  }

  return chek;
 
}
