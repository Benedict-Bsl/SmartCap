bool db = true;

#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>
//#include <APDS9930.h>

#define COLLECT_NUMBER 3  //Fingerprint sampling times, can be set to 1-3
#define IRQ         3  //IRQ pin 

//D1MINI/ LOLIN32 12,13
//fp 10,11
//fingerpirng
SoftwareSerial mySerial(10, 11);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);


SoftwareSerial D1Mini(12, 13);

//APDS9930 apds = APDS9930();
uint16_t proximity_data = 0;


bool FingerPrint;
bool mtCtrl = false;

int vn;
int m = 0;
char c;
//String v;


//finger print
int comparison_flag = 0;
int registration_flag = 0;
int deletion_flag = 0;
int fingerPrintAccepted = 0;

int volatile flag = 0;

void trigger();
int fpNumber=0;
void initFP() {

  Serial.println("\n\nAdafruit finger detect test");

//  mySerial.listen();
  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);

  fpNumber=finger.getTemplateCount();

  if (fpNumber == 0) {
    Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  }
  else {
    Serial.println("Waiting for valid finger...");
      Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  }
}



char x;
char ck = '0';
int sikiliza() {
  int r = 0;
  D1Mini.listen();

  Serial.print("Receiving: ");
  while (D1Mini.available()) {
    Serial.print(" Got:");
    ck = D1Mini.read();

    Serial.print(ck);
    if (ck == '4') {
      mtCtrl = true;
    }
    break;


    Serial.print("loop: ");

  }
  delay(100);
  Serial.print(ck); Serial.print(" ");
  Serial.println("continuing");
  Serial.println();
  if (ck == '1') {
    Serial.println(F("Writing to buzzer"));
  }
  else if (ck == '2') {
    finger.LEDcontrol(FINGERPRINT_LED_BREATHING, 100, FINGERPRINT_LED_RED, 10);
    Serial.println(F("FP deletion subroutine"));
    r=2;
  }

  else if (ck == '3') {
    finger.LEDcontrol(FINGERPRINT_LED_BREATHING, 100, FINGERPRINT_LED_PURPLE, 10);
    Serial.println(F("FP registration subroutine"));
    r=3;
  }
  else if (mtCtrl) {
    Serial.println(F("Bottom locking mechanism.."));
    digitalWrite(7, 1);
    digitalWrite(6, 0);
    delay(150);
    digitalWrite(7, 0);
    digitalWrite(6, 0);
    delay(50000);
    Serial.println(F("Back again..."));
    digitalWrite(7, 0);
    digitalWrite(6, 1);
    delay(130);
    digitalWrite(7, 0);
    digitalWrite(6, 0);
    delay(1500);

    mtCtrl = false;
  }
  else if (ck == '5'){
    digitalWrite(4,0);
    analogWrite(5,0);
    delay(250);
  }
  else if(ck == '6'){
    Serial.println(F("dispenser"));
//    analogWrite(A0, 250);
//    digitalWrite(4,0);
//    analogWrite(5, 200);
//    delay(1500);
//    digitalWrite(4,0);
//    analogWrite(5, 80);
  }
  else if(ck == 'R'){
    finger.LEDcontrol(FINGERPRINT_LED_BREATHING, 200, FINGERPRINT_LED_BLUE, 15);
  }
  
  return r;

}

