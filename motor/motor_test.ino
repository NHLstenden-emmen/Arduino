void setup() {
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(6, OUTPUT);
}

void loop() {

  digitalWrite(2,HIGH);
  digitalWrite(3,LOW);
  analogWrite(6, 200);

  delay(5000);

  digitalWrite(2,HIGH);
  digitalWrite(3,LOW);
  analogWrite(6, 100);

  delay(5000);
}
