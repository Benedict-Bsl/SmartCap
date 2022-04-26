#include <EEPROM.h>

void setup() {
  Serial.begin(115200);

  while(!Serial){
    ;
  }
  Serial.println("Reading from EEPROM");

  float v = -6.242342f;
  float f = 0.00f;
  String st = "hell no!";
  String s = "";
  int ad = 0;
//  Serial.print("Previous value: ");Serial.println(EEPROM.read(0));
//  Serial.println("initializing first memory to 0");
//  EEPROM.write(0,0);
//  delay(100);
//  Serial.print("current value: ");Serial.println(EEPROM.read(0));
//  delay(100);
//  Serial.print("Writing new value: ");Serial.println(v);
//  EEPROM.put(ad,v);
//  delay(100);
//    EEPROM.write(1,0);
//  Serial.print("now value: ");Serial.println(EEPROM.read(1)); 
//  EEPROM.put(0, st);
//    EEPROM.write(0,"");
    
  delay(100);
//  EEPROM.put(ad,v);
//  ad+=sizeof(st);
////  delay(100);
//  EEPROM.put(0,st);
//  delay(100);


  float ef = EEPROM.get(7,f);
//  Serial.print("Size: ");Serial.print(sizeof(r));Serial.print(" ");
  delay(1000);
//  Serial.println(" ");
 

//  delay(5000);

//
//  
//  if(ef == v){
//    Serial.println("Got same value");
//  }  
//  delay(100);
//////  delay(100);
//  EEPROM.put(7, v);
//  delay(100);
//  Serial.print("second address : ");Serial.println(EEPROM.get(7,f));

  String r = EEPROM.get(0,s);
  Serial.print("First address: ");Serial.println(r); 
  if(r == st){
    Serial.println("Got same string");
  }
}

void loop() {

}
