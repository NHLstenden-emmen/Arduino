#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

#define S_motor_L  6           // snelheid motor links
#define RL_motor_L  2          // Richting Links motor links
#define RR_motor_L  3          // Richting Rechts motor links

#define IR_Rechts 10
#define IR_Links  11

int  IR_Rechts_val = 0;
int  IR_Links_val = 0;

void setup() 
{
  pinMode(RL_motor_L, OUTPUT);
  pinMode(RR_motor_L , OUTPUT);
  pinMode(S_motor_L, OUTPUT);
  pinMode(IR_Rechts, INPUT);
  pinMode(IR_Links, INPUT);
}

void loop() 
{
   IR_Rechts_val = digitalRead(IR_Rechts);
   IR_Links_val = digitalRead(IR_Links);

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
   }
   
    else if(IR_Rechts_val == 1 && IR_Links_val == 0)
    {

       lcd.setCursor(0,1);
       lcd.print(IR_Rechts_val);
       lcd.setCursor(15,1);
       lcd.print(IR_Links_val);
       lcd.setCursor(3,0);
       lcd.print("stuur links!");
    }
    else if (IR_Rechts_val == 0 && IR_Links_val == 1)
    {

       lcd.setCursor(0,1);
       lcd.print(IR_Rechts_val);
       lcd.setCursor(15,1);
       lcd.print(IR_Links_val);
       lcd.setCursor(3,0);
       lcd.print("stuur rechts!");
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
    }
  
    delay(10);
}
