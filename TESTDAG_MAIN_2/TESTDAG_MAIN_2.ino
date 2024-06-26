//Library's
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
//adafruit modules
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_VL53L0X.h>
//wifi modules
#include <WiFi.h>
#include <WebSocketsClient.h>

#include <stdlib.h>

// Hardware pin defines
#define V_motor_L   16           // vooruit motor links
#define A_motor_L   17           // Achteruit motor links


#define V_motor_R   18          // vooruit motor Rechts
#define A_motor_R   5           // Achteruit motor Rechts


#define IR_Rechts 34          // Linker IR sensBlackor 
#define IR_Links  39          // Rechter IR sensBlackor

// IR trigger values
int  IR_Rechts_val = 0;
int  IR_Links_val = 0;

// moter PWM setup
const int freq = 1000;
const int SV_motor_L = 0;
const int SA_motor_L = 1;
const int SV_motor_R = 2;
const int SA_motor_R = 3;
const int resolution = 8;
int sensBlack = 200;

// race conditions
boolean RaceStart = false;
boolean TekeningStart = false;
boolean DoolhofStart = false;
boolean SPSStart = false;


//Wifi Connectie
const char* BOT_NAME = "Dimitri";         
char* ssid = "Dimitri";
char* pass = "Dimitri1";
WebSocketsClient webSocket;

//Afstand Sensor
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

//Oled Scherm
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void setup()                
{
  Serial.begin(115200);
  
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

  //Clear Display Buffer
  display.clearDisplay();

  //Display Booted Text
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,14);
  display.println("Beschikbaar");
  display.display();
  delay(2000);
  display.clearDisplay();

  //Afstandssensor
  while (! Serial) {
    delay(1);
  }
  if (!lox.begin()) {
    Serial.println(F("Afstandsensor niet beschikbaar"));
    while(1);
  }
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

void loop()
{
  webSocket.loop();
  StartRace();
  StartTekening();
  StartDoolhof();
  StartSPS();
}

void commandReceiver(uint8_t command)
{
  switch(command)
  { 
  case 48://Stop
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,14);
    display.println("Stop");
    display.display();
    display.clearDisplay();
    RaceStart = false;
    TekeningStart = false;
    DoolhofStart = false;
    SPSStart = false;
    break;
       
  case 49://Race
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,14);
    display.println("Race");
    display.display();
    display.clearDisplay();
    RaceStart = true;
    TekeningStart = false;
    DoolhofStart = false;
    SPSStart = false;
    break;

  case 50://Tekening
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,14);
    display.println("Tekening");
    display.display();
    display.clearDisplay();
    RaceStart = false;
    TekeningStart = true;
    DoolhofStart = false;
    SPSStart = false; 
    break;
    
  case 51://Doolhof
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,14);
    display.println("Doolhof");
    display.display();
    display.clearDisplay();
    RaceStart = false;
    TekeningStart = false;
    DoolhofStart = true;
    SPSStart = false;
    break;

  case 52://Steen-Papier-Schaar (SPS)
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,14);
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
    display.setCursor(0,14);
    display.println("Winaar!");
    display.display();
    display.clearDisplay();
    break;

   case 57://Verloren
    display.clearDisplay();
    display.setCursor(0,14);
    display.println("Loser");
    display.display();
    display.clearDisplay();
    break;

  case 104:// Gelijkspel
    display.clearDisplay();
    display.setCursor(0,14);
    display.println("Tie");
    display.display();
    display.clearDisplay();
    break;
  }
}

//---------------------Race Code---------------------    check geen problemen.        
void StartRace()
{
  //RaceStart = true;
  if(RaceStart == true)
  {
  IR_Rechts_val = analogRead(IR_Rechts);
  IR_Links_val = analogRead(IR_Links);
  VL53L0X_RangingMeasurementData_t measure;
  //delay(10);
  // rechtdoor rijden 
  if(IR_Rechts_val > sensBlack && IR_Links_val > sensBlack){
    plankGasRace();
  } // turn left
  else if(IR_Rechts_val > sensBlack && IR_Links_val < sensBlack) {
  turnRightRace();
  } // turn right
  else if (IR_Rechts_val < sensBlack && IR_Links_val > sensBlack){
  turnLeftRace();
  }
  else if (IR_Rechts_val < sensBlack && IR_Links_val < sensBlack){
    driveBackRace();
  }  else if (measure.RangeMilliMeter < 200){
  stopGameRace();
  } 
  } 
}
void plankGasRace(){
    ledcWrite(SV_motor_L, 220);  //220
    ledcWrite(SA_motor_L, 0);
    ledcWrite(SV_motor_R, 220);
    ledcWrite(SA_motor_R, 0);

    //resetDisplay();
   // display.println("Recht Door");
    //display.display();
    delay(20);
    stopDrivingRace();
}
void turnLeftRace(){
    ledcWrite(SV_motor_L, 0);
    ledcWrite(SA_motor_L, 100);
    ledcWrite(SV_motor_R, 0);
    ledcWrite(SA_motor_R, 0);

   // resetDisplay();
   // display.println("links");
   // display.display();

    delay(20);
    stopDrivingRace();
  
}
void turnRightRace(){
    ledcWrite(SV_motor_L, 0);
    ledcWrite(SA_motor_L, 0);
    ledcWrite(SV_motor_R, 0);
    ledcWrite(SA_motor_R, 100);

    //resetDisplay();
    //display.println("Rechts");
    //display.display();

    delay(20);
    stopDrivingRace();
}
void driveBackRace(){
    ledcWrite(SV_motor_L, 0);
    ledcWrite(SA_motor_L, 100);
    ledcWrite(SV_motor_R, 0);
    ledcWrite(SA_motor_R, 100);

   // resetDisplay();
   // display.println("achteruit");
   // display.display();

    delay(100);
    stopDrivingRace();
}
void stopGameRace(){
    resetDisplay();
    display.println("Finish!");
    display.display();

    webSocket.sendTXT("11");
    RaceStart = false;
    
    stopDrivingRace();
    delay(1000);
}
void stopDrivingRace(){
    ledcWrite(SV_motor_L, 0);
    ledcWrite(SA_motor_L, 0);
    ledcWrite(SV_motor_R, 0);
    ledcWrite(SA_motor_R, 0);
}


//---------------------Tekening Code---------------------           check maar disconect op het laatst

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
  webSocket.sendTXT("12");
  TekeningStart = false;
  }
}

//---------------------Doolhof Code---------------------       check geen problemen

void StartDoolhof()
{
  //DoolhofStart = true;
  if(DoolhofStart == true)
  {
  IR_Rechts_val = analogRead(IR_Rechts);
  IR_Links_val = analogRead(IR_Links);
  //VL53L0X_RangingMeasurementData_t measure;
  //lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!
  delay(10);
  // rechtdoor rijden 
   if(IR_Rechts_val < sensBlack && IR_Links_val < sensBlack)
   {
    plankGasDoolhof();
  } // turn left
  else if(IR_Rechts_val > sensBlack && IR_Links_val < sensBlack) 
  {
  turnLeftDoolhof();
  } // turn right
  else if (IR_Rechts_val < sensBlack && IR_Links_val > sensBlack)
  {
  turnRightDoolhof();
  }// drive back and turn
  else if (IR_Rechts_val > sensBlack && IR_Links_val > sensBlack)
  {
    driveBackwardsDoolhof();
  } 
/*
  else if (measure.RangeMilliMeter < 150)
  {
      Serial.print("Distance (mm): "); Serial.println(measure.RangeMilliMeter);
      stopDrivingDoolhof();
      display.display();
      display.clearDisplay();
      display.println("Einde Doolhof");
      webSocket.sendTXT("13");
      DoolhofStart = false;
   }
*/   
  } 
}
///////////////////////////////////////////////////////functies
void plankGasDoolhof(){
   // resetDisplay();
    //display.println("recht Door");
   // display.display();

    ledcWrite(SV_motor_L, 215);
    ledcWrite(SA_motor_L, 0);
    ledcWrite(SV_motor_R, 200);
    ledcWrite(SA_motor_R, 0);

    delay(20);
    stopDrivingDoolhof();
}
void turnLeftDoolhof()
{
    //resetDisplay();
    //display.println("Links");
    //display.display();

    driveBackwardsDoolhof();

    ledcWrite(SV_motor_L, 0);
    ledcWrite(SA_motor_L, 120);
    ledcWrite(SV_motor_R, 120);
    ledcWrite(SA_motor_R, 0);
    delay(50);
}
void turnRightDoolhof()
{
    //resetDisplay();
    //display.println("Rechts");
    //display.display();

    driveBackwardsDoolhof();

    ledcWrite(SV_motor_L, 90);
    ledcWrite(SA_motor_L, 0);
    ledcWrite(SV_motor_R, 0);
    ledcWrite(SA_motor_R, 90);
    delay(50);
}
 void driveBackwardsDoolhof()
 {
   // resetDisplay();
   // display.println("Rechts");
    //display.display();
    
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

//---------------------Steen Papier Schaar Code---------------------   check

void StartSPS()
{
  if(SPSStart == true)
  {
    int number = random(1,4);

  Serial.println("-----------");
  Serial.println(number);
  if (number == 1){
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  // Display static text
  display.println("Ik kies papier.");
  display.display(); 
  webSocket.sendTXT("6");
  SPSStart = false;
  }

  if (number == 2){
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  // Display static text
  display.println("Ik kies steen.");
  display.display(); 
  webSocket.sendTXT("5");
  SPSStart = false;
  }

  if (number == 3){
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  // Display static text
  display.println("Ik kies schaar.");
  display.display(); 
  webSocket.sendTXT("7");
  SPSStart = false;
  }

    } 
  }


// reset display
void resetDisplay(){
  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0,10);             // Start at top-left corner
}
