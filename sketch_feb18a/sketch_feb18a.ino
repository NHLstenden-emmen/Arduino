void setup() 
{
   Serial.begin(9600);
   pinMode(10, INPUT);
   pinMode(11, INPUT);

}

void loop() {
  int detect_r = digitalRead(10);
  int detect_l = digitalRead(11);
  
  Serial.write("Rechts ");
  Serial.println(detect_r);
  Serial.write("Links ");
  Serial.println(detect_l);
  delay(1000);
}
