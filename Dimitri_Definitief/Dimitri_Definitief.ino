///////////////algemene modules///////////////////////
#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>

//////////////adafruit modules///////////////
#include <Adafruit_VL53L0X.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//////////////wifi modules/////////////////
#include <WiFi.h>
#include <WebSocketsClient.h>

//////////////Hardware Conecties////////////
#define V_motor_L   16           // vooruit motor links
#define A_motor_L   17           // Achteruit motor links


#define V_motor_R   18          // vooruit motor Rechts
#define A_motor_R   5           // Achteruit motor Rechts


#define IR_Rechts 34          // Linker IR sensBlack
#define IR_Links  39          // Rechter IR sensBlack

///////IR trigger values///////////
int  IR_Rechts_val = 0;
int  IR_Links_val = 0;

////////motor PWM setup///////////
const int freq = 1000;
const int SV_motor_L = 0;
const int SA_motor_L = 1;
const int SV_motor_R = 2;
const int SA_motor_R = 3;
const int resolution = 8;
int sensBlack = 200;

/////////race condities//////////
boolean RaceStart = false;
boolean TekeningStart = false;
boolean DoolhofStart = false;
boolean SPSStart = false;

/////////Wifi Connectie info//////////
const char* BOT_NAME = "Dimitri";         
char* ssid = "Dimitri";
char* pass = "Dimitri1";
WebSocketsClient webSocket;

//////////Afstand Sensor//////////
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

////////////////////Oledscherm setup/////////////////////////////////
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void setup()                
{
  Serial.begin(115200);
  lox.begin();
  
  pinMode(V_motor_L, OUTPUT);
  pinMode(A_motor_L , OUTPUT);

  pinMode(V_motor_R, OUTPUT);
  pinMode(A_motor_R , OUTPUT);

  pinMode(IR_Rechts, INPUT);
  pinMode(IR_Links, INPUT);

  ledcSetup(SV_motor_L, freq, resolution);
  ledcAttachPin(V_motor_L, SV_motor_L);

  ledcSetup(SA_motor_L, freq, resolution);
  ledcAttachPin(A_motor_L, SA_motor_L);

  ledcSetup(SV_motor_R, freq, resolution);
  ledcAttachPin(V_motor_R, SV_motor_R);

  ledcSetup(SA_motor_R, freq, resolution);
  ledcAttachPin(A_motor_R, SA_motor_R);
  
  //Wifi Connectie
  WiFi.begin(ssid, pass);
  WiFi.mode(WIFI_STA);

  for(int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) 
  {
    Serial.print(".");
    delay(1000);
  }

  if(WiFi.status() != WL_CONNECTED) 
  {   
    Serial.println("No Wifi!");
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,14);
    display.println("NO WIFI");
    display.display();
    delay(2000);
    display.clearDisplay();
    return;
  }

  //Server address, port and URL
  webSocket.begin("194.171.181.139", 49154, "/");

  //Event handler
  webSocket.onEvent(webSocketEvent);

  //Probeer over 5000 ms opnieuw
  webSocket.setReconnectInterval(5000);
  
  //Initialize with the I2C addr 0x3C
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  

  //Clear Display 
  display.clearDisplay();

  //Beschikbaar
  display.setCursor(0,15);
  display.println("Beschikbaar");
  display.display();
  delay(2000);
  display.clearDisplay();
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) 
{
  switch(type) 
  {
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0,14);
      display.println("Disconnected!");
      break;
    case WStype_CONNECTED:
      Serial.printf("[WSc] Connected to url: %s\n", payload);
      webSocket.sendTXT(BOT_NAME);
      break;
    case WStype_TEXT:
      Serial.printf("[WSc] get text: %s\n", payload);
      commandReceiver(*payload);
      break;
  }
}

void loop()                    // loop met alle spellen 
{
  webSocket.loop();
  StartRace();
  StartTekening();
  StartDoolhof();
  StartSPS();
}

void commandReceiver(uint8_t command)      // haalt server command op.
{
  switch(command)                          // de server server selecteerd welke case moet worden uitgevoerd.
  { 
  case 48://Stop
    display.setCursor(0,15);
    display.println("Stop");
    display.display();
    display.clearDisplay();
    RaceStart = false;
    TekeningStart = false;
    DoolhofStart = false;
    SPSStart = false;
    break;
       
  case 49://Race
    display.setCursor(0,15);
    display.println("Race");
    display.display();
    display.clearDisplay();
    RaceStart = true;
    TekeningStart = false;
    DoolhofStart = false;
    SPSStart = false;
    break;

  case 50://Tekening
    display.setCursor(0,15);
    display.println("Tekening");
    display.display();
    display.clearDisplay();
    RaceStart = false;
    TekeningStart = true;
    DoolhofStart = false;
    SPSStart = false; 
    break;
    
  case 51://Doolhof
    display.setCursor(0,15);
    display.println("Doolhof");
    display.display();
    display.clearDisplay();
    RaceStart = false;
    TekeningStart = false;
    DoolhofStart = true;
    SPSStart = false;
    break;

  case 52://Steen-Papier-Schaar 

    display.setCursor(0,15);
    display.println("Steen-Papier-Schaar");
    display.display();
    display.clearDisplay();
    RaceStart = false;
    TekeningStart = false;
    DoolhofStart = false;
    SPSStart = true;
    break;

  case 56://Gewonnen
    display.clearDisplay();
    display.setCursor(0,15);
    display.println("Winaar!");
    display.display();
    display.clearDisplay();
    break;

   case 57://Verloren
    display.clearDisplay();
    display.setCursor(0,15);
    display.println("Loser");
    display.display();
    display.clearDisplay();
    break;

  case 104:// Gelijkspel
    display.clearDisplay();
    display.setCursor(0,15);
    display.println("Tie");
    display.display();
    display.clearDisplay();
    break;
  }
}

////////////////////////////////////////Race/////////////////////////////////////////         
void StartRace()
{
  //RaceStart = true;
  if(RaceStart == true)
  {
    IR_Rechts_val = analogRead(IR_Rechts);
    IR_Links_val = analogRead(IR_Links);
    VL53L0X_RangingMeasurementData_t measure;

    if(IR_Rechts_val > sensBlack && IR_Links_val > sensBlack)
    {
    plankGasRace();
    } 
    else if(IR_Rechts_val > sensBlack && IR_Links_val < sensBlack) 
    {
    turnRightRace();
    } 
    else if (IR_Rechts_val < sensBlack && IR_Links_val > sensBlack)
    {
    turnLeftRace();
    }
    else if (IR_Rechts_val < sensBlack && IR_Links_val < sensBlack)
    {
    driveBackRace();
    }
  } 
}  

/////////////////////////////////////////functies_Race//////////////////////////////////////////
void plankGasRace()
{
    ledcWrite(SV_motor_L, 220);  //220 (max goedgaande geteste snelheid)
    ledcWrite(SA_motor_L, 0);
    ledcWrite(SV_motor_R, 220);
    ledcWrite(SA_motor_R, 0);
    
    delay(20);
    stopDrivingRace();
}
void turnLeftRace()
{
    ledcWrite(SV_motor_L, 0);
    ledcWrite(SA_motor_L, 100);
    ledcWrite(SV_motor_R, 0);
    ledcWrite(SA_motor_R, 0);
    
    delay(20);
    stopDrivingRace();
}
void turnRightRace()            // laat robot naar rechts sturen.
{
    ledcWrite(SV_motor_L, 0);
    ledcWrite(SA_motor_L, 0);
    ledcWrite(SV_motor_R, 0);
    ledcWrite(SA_motor_R, 100);

    delay(20);
    stopDrivingRace();
}
void driveBackRace()             //laat robot achteruit rijden.
{
    ledcWrite(SV_motor_L, 0);
    ledcWrite(SA_motor_L, 100);
    ledcWrite(SV_motor_R, 0);
    ledcWrite(SA_motor_R, 100);

    delay(100);
    stopDrivingRace();
}
void stopDrivingRace()          // brengt robot tot stilstand.
{
    ledcWrite(SV_motor_L, 0);
    ledcWrite(SA_motor_L, 0);
    ledcWrite(SV_motor_R, 0);
    ledcWrite(SA_motor_R, 0);
}


////////////////////////////////////Tekening///////////////////////////////////////         

void StartTekening()
{
  //TekeningStart = true;
  if(TekeningStart == true)
  {
  // code van de tekening
  ledcWrite(SV_motor_L, 0);
  ledcWrite(SA_motor_L, 0);
  
  ledcWrite(SV_motor_R, 0);
  ledcWrite(SA_motor_R, 0);

  delay(3000);
  
  ledcWrite(SV_motor_L, 0);
  ledcWrite(SA_motor_L, 110);
  
  ledcWrite(SV_motor_R, 0);
  ledcWrite(SA_motor_R, 0);

  delay(2400);

  ledcWrite(SV_motor_L, 0);
  ledcWrite(SA_motor_L, 0);
  
  ledcWrite(SV_motor_R, 0);
  ledcWrite(SA_motor_R, 0);

  delay(500);

  ledcWrite(SV_motor_L, 110);
  ledcWrite(SA_motor_L, 0);
  
  ledcWrite(SV_motor_R, 0);
  ledcWrite(SA_motor_R, 110);
  
  delay(300);

  ledcWrite(SV_motor_L, 0);
  ledcWrite(SA_motor_L, 110);
  
  ledcWrite(SV_motor_R, 0);
  ledcWrite(SA_motor_R, 110);

  delay(300);

  ledcWrite(SV_motor_L, 0);
  ledcWrite(SA_motor_L, 0);
  
  ledcWrite(SV_motor_R, 0);
  ledcWrite(SA_motor_R, 110);

  delay(1200);

  ledcWrite(SV_motor_L, 0);
  ledcWrite(SA_motor_L, 110);
  
  ledcWrite(SV_motor_R, 0);
  ledcWrite(SA_motor_R, 0);

  delay(1200);
  display.display();
  display.clearDisplay();
  display.println("Einde Tekening");
  webSocket.sendTXT("12");             // laat server weten dat robot klaar is met de tekening.
  TekeningStart = false;               // stopt het spel.
  }
}

/////////////////////////////////Doolhof////////////////////////////////////////////     

void StartDoolhof()
{
  //DoolhofStart = true;
  if(DoolhofStart == true)
  {
    IR_Rechts_val = analogRead(IR_Rechts);
    IR_Links_val = analogRead(IR_Links);
    VL53L0X_RangingMeasurementData_t measure;
    lox.rangingTest(&measure, false);
    delay(10);

    if (measure.RangeMilliMeter < 150)          // check of er geen obstakel in de weg zit.
    {
      stopDrivingDoolhof();
      display.clearDisplay();
      display.println("Obstakel in de weg");
      display.display();
      delay(200);
    }
    else if(IR_Rechts_val < sensBlack && IR_Links_val < sensBlack)
    {//plankgas vooruit
      plankGasDoolhof();
    } 
    else if(IR_Rechts_val > sensBlack && IR_Links_val < sensBlack) 
    {// Naar links
    turnLeftDoolhof();
    } 
    else if (IR_Rechts_val < sensBlack && IR_Links_val > sensBlack)
    {// Naar rechts
    turnRightDoolhof();
    }
    else if (IR_Rechts_val > sensBlack && IR_Links_val > sensBlack)
    {// Achteruit en draaien
    driveBackwardsDoolhof();
    }
  } 
}
/////////////////////////////////////////functies_Doolhof//////////////////////////////////////////
void plankGasDoolhof()
{
    ledcWrite(SV_motor_L, 215);
    ledcWrite(SA_motor_L, 0);
    ledcWrite(SV_motor_R, 200);
    ledcWrite(SA_motor_R, 0);

    delay(20);
    stopDrivingDoolhof();
}
void turnLeftDoolhof()
{
    driveBackwardsDoolhof();

    ledcWrite(SV_motor_L, 0);
    ledcWrite(SA_motor_L, 120);
    ledcWrite(SV_motor_R, 120);
    ledcWrite(SA_motor_R, 0);
    delay(50);
}
void turnRightDoolhof()
{
    driveBackwardsDoolhof();

    ledcWrite(SV_motor_L, 90);
    ledcWrite(SA_motor_L, 0);
    ledcWrite(SV_motor_R, 0);
    ledcWrite(SA_motor_R, 90);
    delay(50);
}
 void driveBackwardsDoolhof()
 {
    ledcWrite(SV_motor_L, 0);
    ledcWrite(SA_motor_L, 140);
    ledcWrite(SV_motor_R, 0);
    ledcWrite(SA_motor_R, 140);
    delay(200);
}

void stopDrivingDoolhof(){
    ledcWrite(SV_motor_L, 0);
    ledcWrite(SA_motor_L, 0);
    ledcWrite(SV_motor_R, 0);
    ledcWrite(SA_motor_R, 0);
}

///////////////////////////////////Steen Papier Schaar////////////////////////////////////////// 

void StartSPS()
{
  if(SPSStart == true)
  {
    int number = random(1,4);          // genereedt een radom waarde om keuze voor SPS te maken.
     
    if (number == 1)
    {
      delay(2000);
      display.clearDisplay();
      display.setCursor(0, 5);
      display.println("Ik kies papier.");
      display.display(); 
      webSocket.sendTXT("6");
      SPSStart = false;
    }

    if (number == 2)
    {
     delay(2000);
     display.clearDisplay();
     display.setCursor(0, 5);
     display.println("Ik kies steen.");
     display.display(); 
     webSocket.sendTXT("5");
     SPSStart = false;
    }

    if (number == 3)
    {
      delay(2000);
      display.clearDisplay();
      display.setCursor(0, 5);
      display.println("Ik kies schaar.");
      display.display(); 
      webSocket.sendTXT("7");
      SPSStart = false;
    }
  } 
}
