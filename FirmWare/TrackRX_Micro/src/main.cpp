#include <Arduino.h>

int main(){

  init();

  Serial.begin(9600);
  while(true){
    Serial.println(1);

  }
}