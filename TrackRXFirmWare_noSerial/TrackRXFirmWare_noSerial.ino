#include "SmartCap.h"
#define timerPreload 80000000
#define timeTowait 5

uint16_t rt;
uint16_t rc;
http con= http();
volatile boolean flag = false;

void setup() {

  // put your setup code here, to run once:
  Serial.begin(115200);
  sGps.begin(9600);
  stream->begin(9600);
  nano.begin(9600);
  //   readGPS();
  //init GSM
  //  initGsm();
//  ////Serial.println(F("Get extra mem"));
  //    //Serial.println(F("AT commands start"));
  readGPS();
  delay(10000);
  if (!con.isReady()) {
    ////Serial.println(F("MT not ready"));
  }
  ////Serial.println(F("AT commands ready Now!"));
  ////Serial.println(con.getSignal());
  net();
  // Initiate HTTP/S session with the module
  
  char tkn[] = "device_id=123-000";
  ////Serial.println(ATH_URL);
  strcat(ATH_URL,tkn);
  ////Serial.print(F("posting at:"));
  ////Serial.println(ATH_URL);
  rt = con.doPostD(ATH_URL, content_appCoded, 10000, 10000);
  while (rt != 200) {
    ////Serial.println(F("."));
    rt = con.doPostD(ATH_URL, content_appCoded, 10000, 10000);
    //////Serial.println(rt);
    if (rt >= 300 && rt <= 710) {
      //Serial.println(F("Reconnecting..."));
      reconnect(apn);
    }
    //Serial.print(rt);
    //Serial.println(F("couldn't post"));
  }
  //Serial.print(F(" data size: ")); //Serial.println(con.getDataSizeReceived());
  const String jsResp = con.getDataReceived();
  quantity = jsonExtract(jsResp, "initialQuantity").toInt();
  t = jsonExtract(jsResp, "token");
  
  //Serial.print(F("pills quantity: ")); //Serial.println(quantity);
  //Serial.print(F("token: ")); //Serial.println(t);

  //   }
  //   else{
  //
  //   }
  //  //internet connection
  //  net();
  //  //get Token
  //  getToken();
  pinMode(16, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(16, LOW);
  digitalWrite(13, LOW);

  noInterrupts();
  timer0_isr_init();
  timer0_attachInterrupt(ISR);
  timer0_write(ESP.getCycleCount() + timerPreload * timeTowait);
  interrupts();
}
bool pillCtrl=false;
char ck;
void loop() {
  //  if(sGps.available()){
  //    //Serial.write(sGps.read());

  if(flag){
    //Serial.println(F("intterupts occured!"));
//    noInterrupts();
    //Serial.println(F("GPS data: "));
    sGps.listen();
    readGPS();
    //Serial.println(F("Buzzer"));
    buzzer();
    //Serial.println(F("updating cap"));
    updateCap();
    flag=false;
    timer0_write(ESP.getCycleCount() + timerPreload * timeTowait);
//    interrupts();   
  }
  nano.listen();
  while(nano.available()>0){
    //Serial.print(F("Got: "));
    ck = nano.parseInt();
    if(ck == 1){
      pillCtrl = true;
    }
    
  }
  if(pillCtrl){
    quantity = quantity - ck;
    //Serial.print(ck);
    //Serial.print(F(" Remaining: "));
    //Serial.println(quantity);
    pillCtrl = false;
  }
  //  if(stream->available()){
  //    //Serial.write(stream->read());
  //  }
  //  if(//Serial.available()){
  //    stream->write(//Serial.read());
  //  }


}

void updateCap(){
  
  //update cup
  //Serial.println(F("updateCup"));
  String dt = "finger_print=" + t + "&quantity=" + String(quantity) +
              "&lat=" + String(gps_lat, 7) + "&lon=" + String(gps_lon, 7) + "&token=" + t;

  char dataTosend[(dt.length()) + 1];

  strcpy(dataTosend, dt.c_str());
//  strcat(URL,dt.c_str());
  //Serial.print(URL);
  //Serial.println(dataTosend);
  rt = con.doPostDU(URL,dataTosend,content_appCoded, 2000, 3000);
  
  while (rt != 200) {

    if (rt >= 300 && rt <= 710) {
      //Serial.println(F("Reconnecting..."));
      reconnect(apn);
    }
    rt = con.doPostDU(URL,dataTosend, content_appCoded,5000, 10000);
    //Serial.println(rt);
  }
  
  //Serial.println(F("updated successfully"));
//  //Serial.println(con.getDataSizeReceived());
//  //Serial.println(con.getDataReceived());

}

void buzzer() {
  String stat = "value=&token=" + t;
  char ch[(stat.length()) + 1];
  strcpy(ch, stat.c_str());
//  strcat(buzz, ch);
//  //Serial.print(F("buzzer at:"));
//  //Serial.print(buzz);//Serial.println(ch);
  uint16_t rc = con.doPostDU(buzz,ch, content_appCoded,2000, 3000);

  while (rc != 200) {
    //    net();
    if (rt >= 300 && rt <= 710) {
      //Serial.println(F("Reconnecting..."));
      reconnect(apn);
    }
    //Serial.println(rc);
    rc = con.doPostDU(buzz,ch, content_appCoded,5000, 6000);
  }
//  const size_t cap = getDataSizeReceived();
  buzzinger = jsonExtract(con.getDataReceived(), "value").toInt();
  if (buzzinger == 1) {
//    digitalWrite(16,HIGH);
    //Serial.println(F("Writing 1 to nano"));
    nano.write("1");
//    digitalWrite(4, 1);
//    delay(100);
//    digitalWrite(4,0);
//    delay(500);
//    digitalWrite(4, 1);
//    delay(100);
//    digitalWrite(4,0);
//    delay(500);
//    digitalWrite(4, 1);
//    delay(1000);
  }else if(buzzinger == 4){
    //Serial.println(F("Writing 4 to nano"));
    nano.write("4");
    //Serial.println(F("Then reupload 0"));
    stat = "value=0&token=" + t;
    char ch[(stat.length()) + 1];
    strcpy(ch, stat.c_str());
  //  strcat(buzz, ch);
  //  //Serial.print(F("buzzer at:"));
  //  //Serial.print(buzz);//Serial.println(ch);
    uint16_t rc = con.doPostDU(buzz,ch, content_appCoded,2000, 3000);
    while (rc != 200) {
      //    net();
      if (rt >= 300 && rt <= 710) {
        //Serial.println(F("Reconnecting..."));
        reconnect(apn);
      }
      //Serial.println(rc);
      rc = con.doPostDU(buzz,ch, content_appCoded,5000, 6000);
    }
    //Serial.println(F("Successfully reuploaded 0"));
  }
  else {
    nano.write("0");
//    digitalWrite(16,LOW);
//    nano.write("0");
//    digitalWrite(4, 0);
  }
  //Serial.print(F("Buzzer :"));
  //Serial.println(buzzinger);

}
void net() {

  NetworkRegistration net = con.getRegistrationStatus();

  while (net != REGISTERED_HOME && net != REGISTERED_ROAMING) {
    net = con.getRegistrationStatus();
  }
  //Serial.println(F("Registered"));
  //setup GPRS internet apn
  con.disconnectGPRS();
  bool sucss = con.setupGPRS(apn);
  if(!sucss) {
    //Serial.println("Getting internet...");
    sucss = con.setupGPRS(apn);
    delay(3000);
  }


  bool connected = con.connectGPRS();

  if (connected) {
    //Serial.print(F("connected with ip"));
    //Serial.println(con.getIP());
  }
  else {
    //Serial.println(F("not connected"));
  }



}

void reconnect(const char *u){
  con.sendCommand_P(AT_CMD_HTTPTERM);
  if (!con.readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_OK,2)){}
  con.disconnectGPRS();
  con.setupGPRS(u);
  if(con.connectGPRS()){
    //Serial.println(con.getIP());
  }
  
}

void ISR(void){
  timer0_write(ESP.getCycleCount() + timerPreload * timeTowait);
  flag = true;
}
//void initGsm(){
//  //Serial.println(F("Starting gsm init"));
//  // delay(1000);
//  sim800c = new SIM800L((Stream *)gsm, 1, 200, 512);
//  // setupModule();
//  //Serial.println(F("Checking ready"));
//  while(!sim800c->isReady()){
//    if(db){//Serial.println(F("Problem to connect wiht AT commands"));}
//  }
//  uint8_t signal = sim800c->getSignal();
//
//  while(signal <= 0){
//    signal = sim800c->getSignal();
//  }
//
//  //reest if not connected
//  if(db){//Serial.print(F("Signal: "));//Serial.println(signal);}
//  NetworkRegistration net = sim800c->getRegistrationStatus();
//
//  while(net!=REGISTERED_HOME && net!= REGISTERED_ROAMING){
//    net = sim800c->getRegistrationStatus();
//  }
//  if(db){//Serial.println(F("Registered"));}
//
//}
//
//void net(){
//  bool sucss = sim800c->setupGPRS("Internet");
//  while(!sucss){
//    sucss = sim800c->setupGPRS("internet");
//    customDelay(5000);
//  }
//  if(db){//Serial.print(F("disconnect :"));}
//  if(db){//Serial.println(sim800c->disconnectGPRS());}
//
//  bool connected = sim800c->connectGPRS();
//  // connect to internet
//  if(connected){ if(db){//Serial.println(F("connected with ip: "));//Serial.println(sim800c->getIP());}}
//  else{if(db){//Serial.println(F("Not connected"));}}
//}
//
//void getToken(){
//  const char tkn[] = "device_id=123456";
//  if(db){//Serial.println(F("geting token"));}
//  uint8_t rt = sim800c->doPost(ATH_URL, content_appCoded,tkn, 10000, 10000);
//
//  if(rt == 200){
//      if(db){//Serial.print(F("post successful: "));}
//      const size_t cap = sim800c->getDataSizeReceived();
//      if(db){//Serial.print(cap);}
//      if(db){//Serial.println(F(" bytes"));}
//      if(db){//Serial.println(F("Received: "));}
//      // //Serial.println(sim800c->getDataReceived());
//      const String jsonResponse = sim800c->getDataReceived();
//      uint8_t val = jsonExtract(jsonResponse, "initialQuantity").toInt();
//      String token = jsonExtract(jsonResponse,"token");
//      if(db){//Serial.println(val);}
//      if(db){//Serial.println(token);}
//      if(db){//Serial.println(F(" got"));}
//      delay(200);
//      if(db){//Serial.println(sim800c->getDataReceived());}
//      if(db){//Serial.println(jsonResponse);}
//    }
//    else{
//      if(db){
//        //Serial.println(rt);
//        //Serial.println("failed to post");
//    }
//    }
//}
