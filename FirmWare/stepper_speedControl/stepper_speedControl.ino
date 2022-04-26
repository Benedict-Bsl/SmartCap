//
// /*
// Modified version of the original library Stepper and implementing PWM library To set a 100 khz frecuency 
// to control the TB6612FNG and a stepper Motor 17HS5425 
// */
//
//
//
//#include <Stepper.h>
//#include <PWM.h>
//
//
//#define motorSteps 200     // change this depending on the number of steps
//                           // per revolution of your motor 360/1.8 Degrees
//#define motorPin1 8 // AIN1
//#define motorPin2 9 // AIN2
//#define motorPin3 10 // BIN1
//#define motorPin4 11 // BIN2
//#define ledPin 13
//#define STBY 3 // SBTY
//int PWMA = 12;                // the pin that the PWMA and PWMB Is conected 
//int Cycle = 120;         // 
//int32_t frequency = 100000;
//
//// initialize of the Stepper library:
//Stepper myStepper(motorSteps, motorPin1,motorPin2, motorPin3, motorPin4); 
//
//void setup() {
//  InitTimersSafe(); 
//  // set the motor speed at 60 RPMS:
//  myStepper.setSpeed(60);
//  bool success = SetPinFrequencySafe(PWMA, frequency);
//  // Initialize the Serial port:
//  Serial.begin(9600);
//  // set up the LED pin:
//  pinMode(ledPin, OUTPUT);
//  pinMode(STBY, OUTPUT);
//  digitalWrite(STBY, LOW);
//  // blink the LED:
//  blink(3);
//}
//
//void loop() {
//  // Step forward 100 steps:  
//  pwmWrite(PWMA, Cycle);
//  digitalWrite(STBY, HIGH);
//  Serial.println("Forward");
//  myStepper.step(200);
//  delay(500);
//  // Step backward 100 steps:
//  Serial.println("Backward");
//  myStepper.step(-200);
//  delay(500); 
//
//}
//// Blink the reset LED:
//void blink(int howManyTimes) {
//  int i;
//  for (i=0; i< howManyTimes; i++) {
//    digitalWrite(ledPin, HIGH);
//    delay(200);
//    digitalWrite(ledPin, LOW);
//    delay(200);
//  }
//}

#include <SparkFun_TB6612.h>

#define stb 13

#define pwa 10

#define ain1 11
#define ain2 10

const int offsetA = 1;
Motor motor1 = Motor(ain1, ain2, pwa, offsetA, stb);

void setup(){

  Serial.begin(9600);
  pinMode(pwa, OUTPUT);
  pinMode(stb, OUTPUT);
  pinMode(ain1, OUTPUT);
  pinMode(ain2, OUTPUT);
}

void move(int n){

//  if(d == 1){
    
    digitalWrite(stb, HIGH);
  
    digitalWrite(ain1, HIGH);
    digitalWrite(ain2, LOW);
    analogWrite(pwa, n);
//  }
//  else{
//    digitalWrite(stb, HIGH);
//  
//    digitalWrite(ain1, LOW);
//    digitalWrite(ain2, HIGH);
//    analogWrite(pwa, n);    
//  }
}


void stop(){
  digitalWrite(stb, LOW);
}

void loop(){

//  Serial.println("on");
//  move(50);
//  delay(1500);
//  Serial.println("off");  
//  stop();
//  delay(2500);
//  Serial.println("on");
//  move(50);
//  delay(1500);
//  Serial.println("off");  
//  stop();
//  delay(2500);  
//  digitalWrite(stb, HIGH);
//Serial.println("backward,...");
//  digitalWrite(ain2, HIGH);
//  digitalWrite(ain1, LOW);
//  analogWrite(pwa, 120);
//  delay(5000);

  motor1.drive(100, 1000);
  motor1.drive(-100, 2000);
  motor1.brake();
  delay(1000);
}

