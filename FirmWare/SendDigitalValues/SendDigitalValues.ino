void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(F("Writing to 10"));
  digitalWrite(10, HIGH);
  delay(5000);
  Serial.println(F("LoW to 10"));
  digitalWrite(10, LOW);
  delay(5000);
  Serial.println(F("Writing to 9"));
  digitalWrite(9, HIGH);
  delay(5000);
  Serial.println(F("LoW to 9"));
  digitalWrite(9, LOW);
  delay(5000);
}
