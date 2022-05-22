#include "SmartCap.h"

uint16_t rt;
uint16_t rc;
http con= http();
const char ATH_URL[] = "170.187.154.110/api/v1/smartcap/auth?device_id=911910";
void setup() {

  // put your setup code here, to run once:
  Serial.begin(115200);
  sGps.begin(9600);
  stream->begin(9600);
//  nano.begin(9600);
  //   readGPS();
  //init GSM
  //  initGsm();
  Serial.println(F("Get extra mem"));
  //    Serial.println(F("AT commands start"));
  readGPS();
  if (!con.isReady()) {
    Serial.println(F("MT not ready"));
  }
  Serial.println(F("AT commands ready"));
  Serial.println(con.getSignal());
  net();
  // Initiate HTTP/S session with the module
  
  char tkn[] = "device_id=911910";
  //Serial.println(ATH_URL);
//  strcat(ATH_URL,tkn);
  Serial.println(ATH_URL);
  rt = con.doPostD(ATH_URL, content_appCoded, 10000, 10000);
  while (rt != 200) {
    Serial.println(F("."));
    rt = con.doPostD(ATH_URL, content_appCoded, 10000, 10000);
    Serial.println(rt);
    if (rt >= 700 && rt <= 710) {
      Serial.println(F("Reconnecting..."));
      reconnect(URL);
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

}

void loop() {
  //  if(sGps.available()){
  //    Serial.write(sGps.read());

  Serial.println(F("GPS data: "));
  readGPS();
  Serial.println(F("Buzzer"));
  buzzer();
  Serial.println(F("updating cap"));
  updateCap();
  //  if(stream->available()){
  //    Serial.write(stream->read());
  //  }
  //  if(Serial.available()){
  //    stream->write(Serial.read());
  //  }


}

void updateCap(){
  
  //update cup
  Serial.println(F("updateCup"));
  String dt = "finger_print=" + t + "&quantity=" + String(quantity) +
              "&lat=" + String(gps_lat, 7) + "&lon=" + String(gps_lon, 7) + "&token=" + t;

  char dataTosend[(dt.length()) + 1];

  strcpy(dataTosend, dt.c_str());
  strcat(URL,dataTosend);
  Serial.write(dataTosend);
  rt = con.doPostD(URL, content_appCoded, 10000, 10000);

  while (rt != 200) {

    if (rt >= 700 && rt<= 710) {
      Serial.println(F("Reconnecting..."));
      reconnect(URL);
    }
    rt = con.doPostD(URL, content_appCoded,10000, 10000);
    Serial.println(rt);
  }
  Serial.println(F("updated successfully"));
  Serial.println(con.getDataSizeReceived());
  Serial.println(con.getDataReceived());

}

void buzzer() {
  String stat = "value=&token=" + t;
  char ch[(stat.length()) + 1];
  strcpy(ch, stat.c_str());
  strcat(buzz, ch);
  Serial.write(ch);
  Serial.println();
  uint16_t rc = con.doPostD(buzz, content_appCoded,10000, 10000);

  while (rc != 200) {
    //    net();
    if (rc >= 700 && rc <= 710) {
      Serial.println(F("Reconnecting..."));
      reconnect(buzz);
    }
    Serial.println(rc);
    rc = con.doPostD(buzz, content_appCoded, 10000, 10000);
  }
//  const size_t cap = getDataSizeReceived();
  buzzinger = jsonExtract(con.getDataReceived(), "value").toInt();
  if (buzzinger == 1) {
//    nano.write("1");
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
  } else {
//    nano.write("0");
//    digitalWrite(4, 0);
  }
  Serial.print(F("Buzzer :"));
  Serial.println(buzzinger);

}
void net() {

  NetworkRegistration net = con.getRegistrationStatus();

  while (net != REGISTERED_HOME && net != REGISTERED_ROAMING) {
    net = con.getRegistrationStatus();
  }
  Serial.println(F("Registered"));
  //setup GPRS internet apn
  con.disconnectGPRS();
  bool sucss = con.setupGPRS("Internet");
  if(!sucss) {
    Serial.println("Getting internet...");
    sucss = con.setupGPRS("Internet");
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
//void initGsm(){
//  Serial.println(F("Starting gsm init"));
//  // delay(1000);
//  sim800c = new SIM800L((Stream *)gsm, 1, 200, 512);
//  // setupModule();
//  Serial.println(F("Checking ready"));
//  while(!sim800c->isReady()){
//    if(db){Serial.println(F("Problem to connect wiht AT commands"));}
//  }
//  uint8_t signal = sim800c->getSignal();
//
//  while(signal <= 0){
//    signal = sim800c->getSignal();
//  }
//
//  //reest if not connected
//  if(db){Serial.print(F("Signal: "));Serial.println(signal);}
//  NetworkRegistration net = sim800c->getRegistrationStatus();
//
//  while(net!=REGISTERED_HOME && net!= REGISTERED_ROAMING){
//    net = sim800c->getRegistrationStatus();
//  }
//  if(db){Serial.println(F("Registered"));}
//
//}
//
//void net(){
//  bool sucss = sim800c->setupGPRS("Internet");
//  while(!sucss){
//    sucss = sim800c->setupGPRS("internet");
//    customDelay(5000);
//  }
//  if(db){Serial.print(F("disconnect :"));}
//  if(db){Serial.println(sim800c->disconnectGPRS());}
//
//  bool connected = sim800c->connectGPRS();
//  // connect to internet
//  if(connected){ if(db){Serial.println(F("connected with ip: "));Serial.println(sim800c->getIP());}}
//  else{if(db){Serial.println(F("Not connected"));}}
//}
//
//void getToken(){
//  const char tkn[] = "device_id=123456";
//  if(db){Serial.println(F("geting token"));}
//  uint8_t rt = sim800c->doPost(ATH_URL, content_appCoded,tkn, 10000, 10000);
//
//  if(rt == 200){
//      if(db){Serial.print(F("post successful: "));}
//      const size_t cap = sim800c->getDataSizeReceived();
//      if(db){Serial.print(cap);}
//      if(db){Serial.println(F(" bytes"));}
//      if(db){Serial.println(F("Received: "));}
//      // Serial.println(sim800c->getDataReceived());
//      const String jsonResponse = sim800c->getDataReceived();
//      uint8_t val = jsonExtract(jsonResponse, "initialQuantity").toInt();
//      String token = jsonExtract(jsonResponse,"token");
//      if(db){Serial.println(val);}
//      if(db){Serial.println(token);}
//      if(db){Serial.println(F(" got"));}
//      delay(200);
//      if(db){Serial.println(sim800c->getDataReceived());}
//      if(db){Serial.println(jsonResponse);}
//    }
//    else{
//      if(db){
//        Serial.println(rt);
//        Serial.println("failed to post");
//    }
//    }
//}
