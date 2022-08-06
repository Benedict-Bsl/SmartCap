volatile bool flag;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(33, OUTPUT);
  digitalWrite(33, HIGH);
  delay(200);
  attachInterrupt(33, trigger, FALLING);
//  delay(5000);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(digitalRead(33));
  if(flag){
    Serial.println("Opened....");
    delay(250);
    flag=false;
  }
  else{
    Serial.println("Closed...");
    delay(250);
  }
//  Serial.println(

}
ICACHE_RAM_ATTR void trigger(){
  flag = true;
  Serial.println("Triggered");
}
