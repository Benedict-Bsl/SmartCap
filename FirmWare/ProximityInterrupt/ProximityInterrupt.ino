/****************************************************************
ProximityInterrupt.ino
APDS-9930 Ambient light and proximity sensor
Davide Depau
December 11, 2015
https://github.com/Davideddu/APDS9930

Shawn Hymel @ SparkFun Electronics
October 24, 2014
https://github.com/sparkfun/APDS-9930_RGB_and_Gesture_Sensor

Tests the proximity interrupt abilities of the APDS-9930.
Configures the APDS-9930 over I2C and waits for an external
interrupt based on high or low proximity conditions. Move your
hand near the sensor and watch the LED on pin 13.

Hardware Connections:

IMPORTANT: The APDS-9930 can only accept 3.3V!
 
 Arduino Pin  APDS-9930 Board  Function
 
 3.3V         VCC              Power
 GND          GND              Ground
 A4           SDA              I2C Data
 A5           SCL              I2C Clock
 2            INT              Interrupt
 13           -                LED

Resources:
Include Wire.h and APDS9930.h

Development environment specifics:
Written in Arduino 1.6.5
Tested with Arduino Uno and Mega

This code is beerware; if you see me (or any other SparkFun 
employee) at the local, and you've found our code helpful, please
buy us a round!

Distributed as-is; no warranty is given.
****************************************************************/

#define DUMP_REGS

#include <APDS9930.h>

// Pins
#define APDS9930_INT    13  // Needs to be an interrupt pin
#define LED_PIN         22 // LED for showing interrupt

// Constants
#define PROX_INT_HIGH   500 // Proximity level for interrupt
#define PROX_INT_LOW    0  // No far interrupt

// Global variables
APDS9930 apds = APDS9930();
uint16_t proximity_data = 0;
volatile bool isr_flag = false;
volatile bool flag= false;

void setup() {
  
  // Set LED as output
  pinMode(LED_PIN, OUTPUT);
  pinMode(APDS9930_INT, INPUT);
  pinMode(33, OUTPUT);
  digitalWrite(33, HIGH);
  delay(200);
  
  // Initialize Serial port
  Serial.begin(9600);
  delay(2000);
  Serial.println();
  Serial.println(F("------------------------------"));
  Serial.println(F("APDS-9930 - ProximityInterrupt"));
  Serial.println(F("------------------------------"));
  
  // Initialize interrupt service routine
  attachInterrupt(APDS9930_INT, interruptRoutine, FALLING);
  attachInterrupt(33, trigger, RISING);
  Serial.println("interrupt set...");
//  attachInterrupt(APDS9930_INT, interruptRoutine, FALLING);
  
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
  // Initialize interrupt service routine

  if ( !apds.clearProximityInt() ) {
    Serial.println("Error clearing interrupt");
  }
}
int quantity = 40;
int quantity2 = quantity;
void loop() {
//  Serial.println("Nothing yet");
//  // If interrupt occurs, print out the proximity level
//  if ( isr_flag ) {
//  
//    // Read proximity level and print it out
//    if ( !apds.readProximity(proximity_data) ) {
//      Serial.println("Error reading proximity value");
//    } else {
//      Serial.print("Proximity detected! Level: ");
//      Serial.println(proximity_data);
//      if(proximity_data >= 390){
//        Serial.println("1 pill out");
//       
//        quantity2 = quantity2 - 1;
//      }
//      
//    }
//    Serial.println("initial quantity: ");
//    Serial.println(quantity);
//    Serial.println("Remaining " );
//    Serial.println(quantity2);
//    // Turn on LED for a half a second
//    digitalWrite(LED_PIN, HIGH);
//    delay(500);
//    digitalWrite(LED_PIN, LOW);
//    
//    // Reset flag and clear APDS-9930 interrupt (IMPORTANT!)
//    isr_flag = false;
//    if ( !apds.clearProximityInt() ) {
//      Serial.println("Error clearing interrupt");
//      
//    }
//    
//  }
  
  if(flag){
    Serial.println("Cap opened");
    flag = false;
    bool checker = true;
    while(checker){
       if(digitalRead(33) == LOW){
//        nano.write("5");
        Serial.println("Wrote 5 to nano");
        checker = false;
        Serial.println("Cap closed");
      }
      Serial.println(F("checkder..."));
      delay(250);
      if(isr_flag){
        Serial.print("Previous quantity: ");
        Serial.println(quantity);
        Serial.print("current ");
        Serial.println(quantity2);
        quantity2 = quantity2 - somaNumber();        
        Serial.println("one");
      }

    }
  }
//  if(isr_flag){ 
//    Serial.print("Previous quantity: ");
//    Serial.println(quantity);
//    Serial.print("current ");
//    Serial.println(quantity2);
//    quantity2 = quantity2 - somaNumber();        
//    Serial.println("one");
//
//  }
}

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
      chek = 1;
    }
    else{
      chek = 0;
    }
    
  }
  
  // Turn on LED for a half a second
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  
  // Reset flag and clear APDS-9930 interrupt (IMPORTANT!)
  isr_flag = false;
  
  if ( !apds.clearProximityInt() ) {
    Serial.println("Error clearing interrupt");
  }

  return chek;
 
}

ICACHE_RAM_ATTR void interruptRoutine() {
  isr_flag = true;
}
ICACHE_RAM_ATTR void trigger() {
  flag = true;
}
