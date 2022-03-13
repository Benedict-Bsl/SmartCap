#include <Adafruit_GPS.h>

#include <SoftwareSerial.h>

SoftwareSerial gps_talk(9,10);

Adafruit_GPS GPS(&gps_talk);

String NMEA1;
String NMEA2;
char c;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  gps_talk.begin(9600);

  GPS.sendCommand("$PGCMD,33,0*6D"); // sending a command to turn off antenna update data.
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_10HZ); // setupdate rate to 10hz (speed of operation)

  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA); // request only RMC and GGA sentences only
  delay(1000);
  
}

void loop() {
  // put your main code here, to run repeatedly:
//  while(gps_talk.available()){
//    Serial.write(gps_talk.read());
//
//  }
//  Serial.println();
//  delay(500);

  readGPS();
}


void clearGPS(){

  while(!GPS.newNMEAreceived()){

    c = GPS.read();
  }

  GPS.parse(GPS.lastNMEA());

  while(!GPS.newNMEAreceived()){
    c=GPS.read();
  }
  GPS.parse(GPS.lastNMEA());
}

void readGPS(){

  clearGPS();
  while(!GPS.newNMEAreceived()){
    c = GPS.read();
    
  }
  GPS.parse(GPS.lastNMEA());
  NMEA1 = GPS.lastNMEA();

  while(!GPS.newNMEAreceived()){
    c = GPS.read();
    
  }
  GPS.parse(GPS.lastNMEA());
  NMEA2 = GPS.lastNMEA();  


  Serial.print("NMEA 1: ");
  Serial.println(NMEA1);
  Serial.print("NMEA 2: ");
  Serial.println(NMEA2);
  Serial.println(); 

  Serial.print("\nTime: ");
  if (GPS.hour < 10) { Serial.print('0'); }
  Serial.print(GPS.hour, DEC); Serial.print(':');
  if (GPS.minute < 10) { Serial.print('0'); }
  Serial.print(GPS.minute, DEC); Serial.print(':');
  if (GPS.seconds < 10) { Serial.print('0'); }
  Serial.print(GPS.seconds, DEC); Serial.print('.');
  if (GPS.milliseconds < 10) {

  Serial.print("Date: ");
  Serial.print(GPS.day, DEC); Serial.print('/');
  Serial.print(GPS.month, DEC); Serial.print("/20");
  Serial.println(GPS.year, DEC);
  Serial.print("Fix: "); Serial.print((int)GPS.fix);
  Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
  }

//  if (GPS.fix) {
  Serial.print("Location: ");
  Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
  Serial.print(", ");
  Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);

  Serial.print("Speed (knots): "); Serial.println(GPS.speed);
  Serial.print("Angle: "); Serial.println(GPS.angle);
  Serial.print("Altitude: "); Serial.println(GPS.altitude);
  Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
//  }
//  else{
//    Serial.println("GPS not fixed");
//  }

  
}



