// #if CONFIG_FREERTOS_UNICORE
// static const BaseType_t app_cpu0 = 0;
// #else
// static const BaseType_t app_cpu1 = 1;
// #endif

#include "SmartCap.h"
#define timerPreload 80000000
#define timeTowait 5

#define BUZZER_PIN 5
#define BUZZER_CHANNEL 0


static const BaseType_t app_cpu1 = 1;
static const BaseType_t app_cpu0 = 0;

//Note: the loop and setup functions are set to priority 1 on cpu0
uint16_t rt;
uint16_t rc;
http con= http();
volatile boolean flag = false;
uint16_t proximity_data = 0;
volatile boolean flag2 = false;
bool sta;

bool pillCtrl=false;
char ck='0';
int quantity2;

static TaskHandle_t task1 = NULL;
static TaskHandle_t task2 = NULL;
static TaskHandle_t task3 = NULL;
static TaskHandle_t task4 = NULL;


void readGPS(void *p){
  while(true){
    Serial.println(F("GPS data: "));
    unsigned long age;
    float glat, glon;
    byte month, day, hour, minute, second, hundredths;
    int year;
    sGps.listen();
    for (int i = 0; i < 1000; i++) {
      gps.encode(sGps.read()); \
      gps.f_get_position(&glat, &glon, &age); vTaskDelay(3/portTICK_PERIOD_MS); gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
    }
    

    if (db) {
      Serial.print(F("lat: "));
      Serial.println(glat);
      Serial.print(F(" lon: "));
      Serial.println(glon);
    }
    //   sprintf(tsmp,"%02d/%02/%02d %02d:%02d:%02d ",month,day,year, hour, minute, second);
    // String b = String(month)+"/"+String(day)+"/"+String(year)+" "+String(hour)+":"+String(minute)+":"+String(second);
    // timeStamp = b;
    gps_lat = glat;
    gps_lon = glon;
  }
  //
}

// counting number of pills
void hesabuVidonge(void *p){

  while(true){
    // if(flag){
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
    // flag = false;
    bool checker = true;
    while(checker){
        if(digitalRead(33) == LOW){
        nano.write("5");
        Serial.println("Wrote 5 to nano");
        checker = false;
        Serial.println("Cap closed");
      }
      Serial.println(F("checkder..."));
      vTaskDelay(250/portTICK_PERIOD_MS);
      if(flag2){
        Serial.print("Previous quantity: ");
        Serial.println(quantity2);
        Serial.print("current ");
        Serial.println(quantity);
        quantity = quantity - somaNumber();        
        Serial.println("one");
      }

    }
    // }  
  }
  
}

void updateCap( void *p){
  while(true){
    Serial.println(F("updating cup.."));
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
}

void buzzer(void *p) {
  while(true){

    Serial.println(F("Buzzing/Status: "));
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
        vTaskDelay(100/portTICK_PERIOD_MS);
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
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(1200);
  vTaskDelay(100/portTICK_PERIOD_MS);
  
  sGps.begin(9600);
  stream->begin(9600);
  nano.begin(9600);
  pinMode(APDS9930_INT, INPUT);
  pinMode(33, OUTPUT);
  digitalWrite(33, HIGH);
//  delay(10000);
  while (!con.isReady()) {
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
  quantity2 = quantity;
  Serial.print(F("pills quantity: ")); Serial.println(quantity);
  Serial.print(F("token: ")); Serial.println(t);
  attachInterrupt(13, sens, FALLING);
  //sensing enclosure opened.
  attachInterrupt(33, trigger, RISING);
  //sensing object dettection

  
  Serial.println("Interrupt should start");
  sta = true;
  if(sta){
    nano.write("R");
    sta = false;
  }
  xTaskCreatePinnedToCore(buzzer, "Status", 5000, NULL, 1, NULL, app_cpu0);

//  xTaskCreatePinnedToCore(updateCap, "Current updates", 8000, NULL, 1, &task2, app_cpu0);

//  xTaskCreatePinnedToCore(hesabuVidonge, "Count medecine", 6000, NULL, 2, &task3, app_cpu1);

//  xTaskCreatePinnedToCore(readGPS, "Read GPS", 5000, NULL, 1, &task4, app_cpu0);
//  interrupts();

  /*
    - buzzer always should be checking for update
    - update only when possible
    -hesabuVidonge and do nothing else( iterrupt everything when interrupt occurs)
    - readGPS at last kabisa. when nothing important is happening.
  */
}


void loop() {

//  vTaskDelay(100/portTICK_PERIOD_MS);

//  if(flag){
//    Serial.println("up openned..");
//    vTaskResume(task3);
//    vTaskSuspend(task1);
//    vTaskSuspend(task2);
//    flag=false;
//
//
//  }else
//  {
//    Serial.println("nothing...");
//
//    if(eTaskGetState(task3) == eRunning){
//        vTaskSuspend(task3);
//    }
//    if(eTaskGetState(task1) == eSuspended){
//      vTaskResume(task1);
//    }
//    if(eTaskGetState(task2) == eSuspended){
//      vTaskResume(task2);
//    }
//    // vTaskResume(task4);
//  }

}

ICACHE_RAM_ATTR void sens(){
  flag2 = true;
//  quantity = quantity - ((int)somaNumber());  
}
ICACHE_RAM_ATTR void trigger(){
  flag = true;
  
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
      chek = 2;
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
