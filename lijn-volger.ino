#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

#define S_motor_L  6           // snelheid motor links
#define V_motor_L  2          // vooruit motor links
#define A_motor_L  3          // Richting Rechts motor links

#define S_motor_R  5           // snelheid motor Rechts
#define V_motor_R  9          // Richting Links motor Rechts
#define A_motor_R  4          // Richting Rechts motor Rechts

#define IR_Rechts 10
#define IR_Links  11

int  IR_Rechts_val = 0;
int  IR_Links_val = 0;

void setup() 
{
  pinMode(V_motor_L, OUTPUT);
  pinMode(A_motor_L , OUTPUT);
  pinMode(S_motor_L, OUTPUT);

  pinMode(V_motor_R, OUTPUT);
  pinMode(A_motor_R , OUTPUT);
  pinMode(S_motor_R, OUTPUT);
  
  pinMode(IR_Rechts, INPUT);
  pinMode(IR_Links, INPUT);
  Serial.begin(9600);
}

void loop() 
{
  if(IR_Rechts_val == 0 && IR_Links_val == 0)
  {
   lcd.init();
   lcd.backlight();
   lcd.setCursor(0,1);
   lcd.print(IR_Rechts_val);
   lcd.setCursor(15,1);
   lcd.print(IR_Links_val); 
   lcd.setCursor(3,0);
   lcd.print("rechtdoor!");

    digitalWrite(V_motor_L,HIGH);
    digitalWrite(A_motor_L,LOW);
    analogWrite(S_motor_L , 60);

    digitalWrite(V_motor_R,HIGH);
    digitalWrite(A_motor_R,LOW);
    analogWrite(S_motor_R , 70);
    
   while (IR_Rechts_val == 0 && IR_Links_val == 0)
   {
     IR_Rechts_val = digitalRead(IR_Rechts);
     IR_Links_val = digitalRead(IR_Links);
     //Serial.println(IR_Rechts_val);
     delay(50);
   }
  }
   
    else if(IR_Rechts_val == 1 && IR_Links_val == 0)
    {
       lcd.setCursor(0,1);
       lcd.print(IR_Rechts_val);
       lcd.setCursor(15,1);
       lcd.print(IR_Links_val);
       lcd.setCursor(3,0);
       lcd.print("stuur links!");
       
    digitalWrite(V_motor_L,LOW);
    digitalWrite(A_motor_L,LOW);
    analogWrite(S_motor_L , 0);

    digitalWrite(V_motor_R,LOW);
    digitalWrite(A_motor_R,LOW);
    analogWrite(S_motor_R , 0);

     while (IR_Rechts_val == 1 && IR_Links_val == 0)
   {
     IR_Rechts_val = digitalRead(IR_Rechts);
     IR_Links_val = digitalRead(IR_Links);
     //Serial.println(IR_Rechts_val);
     delay(50);
   }
    }
    else if (IR_Rechts_val == 0 && IR_Links_val == 1)
    {

       lcd.setCursor(0,1);
       lcd.print(IR_Rechts_val);
       lcd.setCursor(15,1);
       lcd.print(IR_Links_val);
       lcd.setCursor(3,0);
       lcd.print("stuur rechts!");

       digitalWrite(V_motor_L,LOW);
       digitalWrite(A_motor_L,LOW);
       analogWrite(S_motor_L , 0);

       digitalWrite(V_motor_R,LOW);
       digitalWrite(A_motor_R,LOW);
       analogWrite(S_motor_R , 0);

      while (IR_Rechts_val == 0 && IR_Links_val == 1)
      {
        IR_Rechts_val = digitalRead(IR_Rechts);
         IR_Links_val = digitalRead(IR_Links);
        //Serial.println(IR_Rechts_val);
         delay(50);
      }
    }
    else
    {
       lcd.init();
       lcd.backlight();
       lcd.setCursor(0,1);
       lcd.print(IR_Rechts_val);
       lcd.setCursor(15,1);
       lcd.print(IR_Links_val); 
       lcd.setCursor(3,0);
       lcd.print("Stop!");

       digitalWrite(V_motor_L,LOW);
       digitalWrite(A_motor_L,LOW);
       analogWrite(S_motor_L , 0);

       digitalWrite(V_motor_R,LOW);
       digitalWrite(A_motor_R,LOW);
       analogWrite(S_motor_R , 0);

       while (IR_Rechts_val == 1 && IR_Links_val == 1)
      {
        IR_Rechts_val = digitalRead(IR_Rechts);
         IR_Links_val = digitalRead(IR_Links);
       // Serial.println(IR_Rechts_val);
         delay(50);
      }
    }
  
    delay(10);
}
