#include <SoftwareSerial.h>

#define WIRING "AT\n"
#define NETWORK "AT+CGREG?\n"
#define APN "internet"
#define STATUS "AT+CIPSTATUS\n"
#define CONNECT "AT+CSTT="
#define ENNET   "AT+CIICR\n"
#define GETIP   "AT+CIFSR\n"
#define GETDNSIP  "AT+CDNSGIP=\"www.techy8.com\"\n" //delay 4secs
#define PING    "AT+CIPPING\n"
#define CONSERVER  "AT+CIPSTART="
const String SENDTOSERVER = "AT+CIPSEND=";
#define CLOSECON    "AT+CIPCLOSE\n"
#define ENTER     "\n"
#define QT    "\""
#define COMA ","


#define HOST "sql336.main-hosting.eu"
#define USER "u108916292_trakrx"
const String PASS = "F4tukub0ny3";
#define DB   "u108916292_trakrx"
#define PORT "3306"


//STATE OF NETWORK
const String IpInitial = "IP INITIAL";
const String IpStart = "IP START";

const String IpConfig = "IP CONFIG";

const String IpGprsact = "IP GPRSACT";

const String IpStatus = "IP STATUS";

const String IpTcpUdp = "TCP CONNECTING/UDP CONNECTING/";

const String ServerList = "SERVER LISTENING";

const String ConnectOk = "CONNECT OK";

const String TcpUdpClosingTCP = "CLOSING/UDP CLOSING";

const String TcpUdpClosed = "TCP CLOSED/UDP CLOSED";

const String pdpDeact = "PDP DEACT";


SoftwareSerial gsm(2, 3); // rx, tx

int v;
int m = 0;

int *n;



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  gsm.begin(9600);
  delay(1000);

  //define apn connection to be used ex: (fast.t-mobile.com), T-Mobile USA
//  String APN = "internet";
  if (checkConn()) {
    internet_connection();
  }

}



bool checkConn() {
  Serial.print("Intializing");

  bool flag;
  gsm.write(WIRING);
  delay(1000);
  while (gsm.available()) {

    int m = gsm.read();
    if (checkAscii(m) != 'E' || checkAscii(m) != 'R') {
      Serial.println("good wiring");
      flag = true;
    }
    else {
      flag = false;
      //      Serial.print(".");
      Serial.println("wiring problem");
    }
  }
  Serial.println();

  if (flag) {
    gsm.write(NETWORK);
    delay(1000);
    Serial.println("Checking network ");
    while (gsm.available()) {
      v = gsm.read();
      //      Serial.print(checkAscii(v));Serial.print(" ");
      if (checkAscii(v) == '1') {
        Serial.println("Network is good!");
        delay(500);
        return true;
        break;
      }
      else {
        Serial.print(".");
        //        return false;


        //      delay(500);
      }

    }
  }
  else {
    return flag;
  }


}

String if_Available_read() {
  String fromBuffer = "";

  while (gsm.available()) {

    fromBuffer += checkAscii(gsm.read());
  }

  return fromBuffer;
}



bool internet_connection() {

  #define CONQ CONNECT QT APN QT COMA QT QT COMA QT QT ENTER
  #define TCP "TCP"
  
  #define SERVER CONSERVER QT TCP QT COMA QT HOST QT COMA QT PORT QT ENTER
  if (checkConn) {
    gsm.write(STATUS);
    delay(100);
    Serial.println(if_Available_read());
//    connect to the accesss point
    // don't runt the bellow code if apn is already assigened.
    gsm.write(CONQ);
    delay(1000);
    Serial.println(if_Available_read());

    gsm.write(ENNET);
    delay(1000);
    Serial.println(if_Available_read());

    gsm.write(GETIP);
    delay(1000);
    Serial.println(if_Available_read());

    gsm.write(GETDNSIP);
    delay(2000);
    Serial.println(if_Available_read());

    Serial.println("Trying to connect to the server...");
    gsm.write(SERVER);
    delay(3000);
    while (gsm.available()){
      Serial.write(gsm.read());
    }
//    Serial.println(if_Available_read());
    int lengthCmd = PASS.length();
    String CMD = "";
    CMD = SENDTOSERVER+lengthCmd;

    Serial.print("Writing ");Serial.print(CMD);Serial.println(" ");
    gsm.println(CMD);
//    gsm.write(CMD);
    delay(4000);
    while (gsm.available()){
      Serial.write(gsm.read());
    }

//    gsm.println("SHOW DATABASES;");
//    delay(3000);
//
////    Serial.println("Sending SQL COMMAND");
////    gsm.println("SHOW DATABASES;");
////    delay(3000);
//    while (gsm.available()){
//      Serial.write(gsm.read());
//      delay(100);
//    }    
  }
  else {
    Serial.println("Couldn't connect to the net");
  }
}

int cmndLength(int a){
  return a;
}

char checkAscii(int k) {

  char arrU[] = { 'A', 'B', 'C', 'D', 'E'\
                  , 'F', 'G', 'H', 'I', 'J'\
                  , 'K', 'L', 'M', 'N', 'O'\
                  , 'P', 'Q', 'R', 'S', 'T'\
                  , 'U', 'V', 'W', 'X', 'Y', 'Z'
                };

  char arrL[] = { 'a', 'b', 'c', 'd', 'e'\
                  , 'f', 'g', 'h', 'i', 'j'\
                  , 'k', 'l', 'm', 'n', 'o'\
                  , 'p', 'q', 'r', 's', 't'\
                  , 'u', 'v', 'w', 'x', 'y', 'z'
                };

  char arrInt[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};



  //  for(int c = 0; c < 26; c++){
  //
  //    int ascii = 65;
  //
  //    if(k== acii & ( arrUL[c] ==
  //  }

  //
  if (k == 65) {
    return arrU[0];
  } else if (k == 66) {
    return arrU[1];
  }
  else if (k == 67) {
    return arrU[2];
  } else if (k == 68) {
    return arrU[3];
  }
  else if (k == 69) {
    return arrU[4];
  } else if (k == 70) {
    return arrU[5];
  }
  else if (k == 71) {
    return arrU[6];
  } else if (k == 72 ) {
    return arrU[7];
  }
  else if (k == 73) {
    return arrU[8];
  } else if (k == 74 ) {
    return arrU[9];
  }
  else if (k == 75 ) {
    return arrU[10];
  } else if (k == 76 ) {
    return arrU[11];
  }

  if (k == 77) {
    return arrU[12];
  } else if (k == 78 ) {
    return arrU[13];
  }
  else if (k == 79) {
    return arrU[14];
  } else if (k == 80 ) {
    return arrU[15];
  }

  if (k == 81 ) {
    return arrU[16];
  } else if (k == 82 ) {
    return arrU[17];
  }
  else if (k == 83 ) {
    return arrU[18];
  } else if (k == 84  ) {
    return arrU[19];
  }
  else if (k == 85 ) {
    return arrU[20];
  } else if (k == 86  ) {
    return arrU[21];
  }
  else if (k == 87 ) {
    return arrU[22];
  } else if (k == 88  ) {
    return arrU[23];
  }
  else if (k == 89 ) {
    return arrU[24];
  } else if (k == 90 ) {
    return arrU[25];
  }

  // small character
  else if (k == 97) {
    return arrL[0];
  } else if (k == 98) {
    return arrL[1];
  }
  else if (k == 99) {
    return arrL[2];
  } else if (k == 100) {
    return arrL[3];
  }
  else if (k == 101) {
    return arrL[4];
  } else if (k == 102) {
    return arrL[5];
  }
  else if (k == 103) {
    return arrL[6];
  } else if (k == 104) {
    return arrL[7];
  }
  else if (k == 105) {
    return arrL[8];
  } else if (k == 106 ) {
    return arrL[9];
  }
  else if (k == 107 ) {
    return arrL[10];
  } else if (k == 107 ) {
    return arrL[11];
  }
  else if (k == 108) {
    return arrL[12];
  } else if (k == 109 ) {
    return arrL[13];
  }
  else if (k == 110) {
    return arrL[14];
  } else if (k == 111 ) {
    return arrL[15];
  }
  else if (k == 112 ) {
    return arrL[16];
  } else if (k == 113 ) {
    return arrL[17];
  }
  else if (k == 114 ) {
    return arrL[18];
  } else if (k == 115  ) {
    return arrL[19];
  }
  else if (k == 116 ) {
    return arrL[20];
  } else if (k == 117  ) {
    return arrL[21];
  }
  else if (k == 118 ) {
    return arrL[22];
  } else if (k == 119  ) {
    return arrL[23];
  }
  else if (k == 120 ) {
    return arrL[24];
  } else if (k == 121 ) {
    return arrL[25];
  }

  else if (k == 48) {
    return arrInt[0];
  } else if (k == 49) {
    return arrInt[1];
  }
  else if (k == 50) {
    return arrInt[2];
  } else if (k == 51) {
    return arrInt[3];
  }
  else if (k == 52) {
    return arrInt[4];
  } else if (k == 53) {
    return arrInt[5];
  }
  else if (k == 54) {
    return arrInt[6];
  } else if (k == 55) {
    return arrInt[7];
  }
  else if (k == 56) {
    return arrInt[8];
  } else if (k == 57) {
    return arrInt[9];
  }
  else return ' ';
}

void loop() {


  if (Serial.available()) {
    gsm.write(Serial.read());

    //    Serial.println(Serial.read());
  }


  if (gsm.available()) {
    v = gsm.read();
    Serial.write(v);

    m++;

  }


  //  for (int i = 0; i < sizeof(v); i++) {
  //    Serial.print(" values: ");
  //    Serial.println(v[i]);
  //  }
}
