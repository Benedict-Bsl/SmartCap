void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(16, INPUT);
  pinMode(13, INPUT);
}
   // 10 from Nano activates 16 of D1MINI
   // 9 from Nano activates 13 of D1MINI
void loop() {
  // put your main code here, to run repeatedly:
  Serial.print(F("Reading 16: "));Serial.println(analogRead(16));
  Serial.print(F("Reading 13: "));Serial.println(analogRead(13));
  delay(500);
 
}
