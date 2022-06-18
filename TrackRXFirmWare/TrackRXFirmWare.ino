#include "SmartCap.h"
#define timerPreload 80000000
#define timeTowait 5

uint16_t rt;
uint16_t rc;
http con= http();
volatile boolean flag = false;
uint16_t proximity_data = 0;
volatile boolean flag2 = false;
void setup() {

  // put your setup code here, to run once:
  Serial.begin(115200);
  sGps.begin(9600);
  stream->begin(9600);
  nano.begin(9600);
  pinMode(A0, INPUT);
//  pinMode(TX, FUNCTION_3);
//  pinMode(TX,INPUT);

  //   readGPS();
  //init GSM
  //  initGsm();
//  Serial.println(F("Get extra mem"));
  //    Serial.println(F("AT commands start"));
  intSensor();
  readGPS();
  delay(10000);
  if (!con.isReady()) {
    Serial.println(F("MT not ready"));
  }
  Serial.println(F("AT commands ready Now!"));
  Serial.println(con.getSignal());
  net();
  // Initiate HTTP/S session with the module
  
  char tkn[] = "device_id=123-000";
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


//  noInterrupts();
//  timer0_isr_init();
//  timer0_attachInterrupt(ISR);
//  timer0_write(ESP.getCycleCount() + timerPreload);
  Serial.println("Interrupt should start");
//  attachInterrupt(1, sens, FALLING);
//  interrupts();
}
bool pillCtrl=false;
char ck='0';
void loop() {
  //  if(sGps.available()){
  //    Serial.write(sGps.read());

//  Serial.println(F("intterupts occured!"));
////    noInterrupts();
  Serial.println(F("GPS data: "));
  sGps.listen();
  readGPS();
  Serial.println(F("Buzzer"));
  buzzer();
  Serial.println(F("updating cap"));
  updateCap();
//    flag=false;
//    timer0_write(ESP.getCycleCount() + timerPreload);
//    interrupts();   
  Serial.print(F("Trg value: "));
  Serial.println(analogRead(A0));
//  if(flag2){
  Serial.println(F("Sensor..."));
  int trg = analogRead(A0);
  Serial.print(F("trigger value: "));
  Serial.println(trg);
  while(trg < 1020){
    pillCtrl = somaNumber();
    if(pillCtrl){
      quantity = quantity - 1;
      Serial.print(ck);
      Serial.print(F(" Remaining: "));
      Serial.println(quantity);
      pillCtrl = false;
    }
    trg = analogRead(A0);
    if(trg > 1020){
      flag2 = false;
    }
  }
//  }

}

void IRAM_ATTR sens(){
  flag2 = true;
  
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
//    digitalWrite(16,HIGH);
    Serial.println(F("Writing 1 to nano"));
    nano.write("1");
  }else if(buzzinger == 4){
    Serial.println(F("Writing 4 to nano"));
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

  while (net != REGISTERED_HOME && net != REGISTERED_ROAMING) {
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

void ISR(void){
  timer0_write(ESP.getCycleCount() + timerPreload * timeTowait);
  flag = true;
}

bool intSensor() {
  
  // Initialize APDS-9930 (configure I2C and initial values)
  if ( apds.init() ) {
    if(db){Serial.println(F("APDS-9930 initialization complete"));}
  } else {
    if(db){Serial.println(F("Something went wrong during APDS-9930 init!"));}
    return false;
  }

  if ( apds.enableProximitySensor(false) ) {
    if(db){Serial.println(F("Proximity sensor is now running"));}
  } else {
    if(db){Serial.println(F("Something went wrong during sensor init!"));}
    return false;
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
      if(db){
        Serial.print(reg, HEX);
        Serial.print(F(": 0x"));
        Serial.println(val, HEX);
      }
    }
  }
  apds.wireReadDataByte(0x1E, val);
  if(db){
    Serial.print(0x1E, HEX);
    Serial.print(F(": 0x"));
    Serial.println(val, HEX);
  }

#endif
  return true;
}

uint16_t a = 0;
bool vl= 0;

bool somaNumber(){
  if ( !apds.readProximity(proximity_data) ) {
    Serial.println(F("Error reading proximity value"));
  } else {
    if(proximity_data > 280 && proximity_data <315){
      a= 300;
      
    }else if(proximity_data > 315 && proximity_data < 410){
      a = 400;
    }else if(proximity_data > 410 && proximity_data < 510){
      a = 500;
    }

    Serial.println(proximity_data);
    if(a >= 400){
      
    Serial.print(" ");
    Serial.print("removed: ");       
    Serial.println(1);
      vl=true;
      delay(2000);
    }
    else{
      vl = false;
    }
    
  }
  return vl;
}
