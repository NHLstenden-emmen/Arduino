#include <Wire.h> 

#define V_motor_L   16          // vooruit motor links
#define A_motor_L   17           // Achteruit motor links


#define V_motor_R   18          // vooruit motor Rechts
#define A_motor_R   5         // Achteruit motor Rechts

const int freq = 3000;
const int SV_motor_L = 0;
const int SA_motor_L = 1;
const int SV_motor_R = 2;
const int SA_motor_R = 3;
const int resolution = 8;
int Sens = 500;
int Speed = 130;

void setup() 
{
  pinMode(V_motor_L, OUTPUT);
  pinMode(A_motor_L , OUTPUT);

  pinMode(V_motor_R, OUTPUT);
  pinMode(A_motor_R , OUTPUT);

  ledcSetup(SV_motor_L, freq, resolution);
  ledcAttachPin(V_motor_L, SV_motor_L);

  ledcSetup(SA_motor_L, freq, resolution);
  ledcAttachPin(A_motor_L, SA_motor_L);

  ledcSetup(SV_motor_R, freq, resolution);
  ledcAttachPin(V_motor_R, SV_motor_R);

  ledcSetup(SA_motor_R, freq, resolution);
  ledcAttachPin(A_motor_R, SA_motor_R);
}

void loop() 
{
  ledcWrite(SV_motor_L, 0);
  ledcWrite(SA_motor_L, 0);
  
  ledcWrite(SV_motor_R, 0);
  ledcWrite(SA_motor_R, 0);

  delay(3000);
  
  ledcWrite(SV_motor_L, 0);
  ledcWrite(SA_motor_L, 150);
  
  ledcWrite(SV_motor_R, 0);
  ledcWrite(SA_motor_R, 0);

  delay(2400);

  ledcWrite(SV_motor_L, 0);
  ledcWrite(SA_motor_L, 0);
  
  ledcWrite(SV_motor_R, 0);
  ledcWrite(SA_motor_R, 0);

  delay(500);

  ledcWrite(SV_motor_L, 150);
  ledcWrite(SA_motor_L, 0);
  
  ledcWrite(SV_motor_R, 0);
  ledcWrite(SA_motor_R, 150);
  
  delay(300);

  ledcWrite(SV_motor_L, 0);
  ledcWrite(SA_motor_L, 150);
  
  ledcWrite(SV_motor_R, 0);
  ledcWrite(SA_motor_R, 150);

  delay(300);

  ledcWrite(SV_motor_L, 0);
  ledcWrite(SA_motor_L, 0);
  
  ledcWrite(SV_motor_R, 0);
  ledcWrite(SA_motor_R, 150);

 delay(1200);

  ledcWrite(SV_motor_L, 0);
  ledcWrite(SA_motor_L, 150);
  
  ledcWrite(SV_motor_R, 0);
  ledcWrite(SA_motor_R, 0);

  delay(1200);
}
