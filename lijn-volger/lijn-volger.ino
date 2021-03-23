#include <Wire.h> 

#define V_motor_L   16           // vooruit motor links
#define A_motor_L   17           // Achteruit motor links


#define V_motor_R   18          // vooruit motor Rechts
#define A_motor_R   5           // Achteruit motor Rechts


#define IR_Rechts 34          // Linker IR sensor 
#define IR_Links  39          // Rechter IR sensor

int  IR_Rechts_val = 0;
int  IR_Links_val = 0;

const int freq = 3000;
const int SV_motor_L = 0;
const int SA_motor_L = 1;
const int SV_motor_R = 2;
const int SA_motor_R = 3;
const int resolution = 8;
int Sens = 500;

void setup() 
{
  pinMode(V_motor_L, OUTPUT);
  pinMode(A_motor_L , OUTPUT);

  pinMode(V_motor_R, OUTPUT);
  pinMode(A_motor_R , OUTPUT);
  
  pinMode(IR_Rechts, INPUT);
  pinMode(IR_Links, INPUT);
  Serial.begin(9600);

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
  if(IR_Rechts_val < Sens && IR_Links_val < Sens)
  {
    ledcWrite(SV_motor_L, 120);
    ledcWrite(SA_motor_L, 0);
  
    ledcWrite(SV_motor_R, 120);
    ledcWrite(SA_motor_R, 0);

    while (IR_Rechts_val < Sens && IR_Links_val < Sens)
    {
      IR_Rechts_val = analogRead(IR_Rechts);
      IR_Links_val = analogRead(IR_Links);
      Serial.println(IR_Rechts_val);
      delay(10);
    }
  }
   
    else if(IR_Rechts_val < Sens && IR_Links_val > Sens)
    {

      ledcWrite(SV_motor_L, 180);
      ledcWrite(SA_motor_L, 0);
  
      ledcWrite(SV_motor_R, 80);
      ledcWrite(SA_motor_R, 0);

      while (IR_Rechts_val < Sens && IR_Links_val > Sens)
        {
          IR_Rechts_val = analogRead(IR_Rechts);
          IR_Links_val = analogRead(IR_Links);
          Serial.println(IR_Rechts_val);
          delay(10);
        }
    }
    else if (IR_Rechts_val > Sens && IR_Links_val < Sens)
    {
      ledcWrite(SV_motor_L, 80);
      ledcWrite(SA_motor_L, 0);
      ledcWrite(SV_motor_R, 180);
      ledcWrite(SA_motor_R, 0);
      
      while (IR_Rechts_val > Sens && IR_Links_val < Sens)
      {
          IR_Rechts_val = analogRead(IR_Rechts);
          IR_Links_val = analogRead(IR_Links);
          Serial.println(IR_Rechts_val);
          delay(10);
      }   
    }
    else if (IR_Rechts_val > Sens && IR_Links_val > Sens)
    {
      ledcWrite(SV_motor_L, 0);
      ledcWrite(SA_motor_L, 0);
      ledcWrite(SV_motor_R, 0);
      ledcWrite(SA_motor_R, 0);
      
       while (IR_Rechts_val > Sens && IR_Links_val > Sens)
        {
          IR_Rechts_val = analogRead(IR_Rechts);
          IR_Links_val = analogRead(IR_Links);
          Serial.println(IR_Rechts_val);
          delay(10);
        }
    }
    delay(10);
}
