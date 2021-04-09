//Library's
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_VL53L0X.h>
#include <WiFi.h>
#include <WebSocketsClient.h>
#include <stdlib.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define V_motor_L   16           // vooruit motor links
#define A_motor_L   17           // Achteruit motor links


#define V_motor_R   18          // vooruit motor Rechts
#define A_motor_R   5           // Achteruit motor Rechts


#define IR_Rechts 34          // Linker IR sensBlackor 
#define IR_Links  39          // Rechter IR sensBlackor

int  IR_Rechts_val = 0;
int  IR_Links_val = 0;

const int freq = 1000;
const int SV_motor_L = 0;
const int SA_motor_L = 1;
const int SV_motor_R = 2;
const int SA_motor_R = 3;
const int resolution = 8;
int sensBlack = 500;

//Afstand Sensor
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

//Wifi Connectie
const char* BOT_NAME = "Lennart";
// wifi hotspot naam
char* ssid = "kevins hotspot";
// wifi ww 
char* pass = "weetikniet";
WebSocketsClient webSocket;

void setup()
{
  Serial.begin(9600);

  //Pin Output & Input
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
    return;
  }

  //Server address, port and URL
  webSocket.begin("194.171.181.139", 49154, "/");

  //Event handler
  webSocket.onEvent(webSocketEvent);

  //Try ever 5000 again if connection has failed
  webSocket.setReconnectInterval(5000);

  // wait until serial port opens for native USB devices
  while (! Serial) {
    delay(1);
  }
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  //Clear Display Buffer
  display.clearDisplay();

  //Display Text werkt
  delay(2000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  // Display static text
  display.println("Hello, world!");
  display.display();
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) 
{
  switch(type) 
  {
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      break;
    case WStype_CONNECTED:
      Serial.printf("[WSc] Connected to url: %s\n", payload);
      webSocket.sendTXT(BOT_NAME);
      break;
    case WStype_TEXT:
      Serial.printf("[WSc] get text: %s\n", payload);
      commandReceiver(((char*) payload));
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

void commandReceiver(char* command)
{
  long StartCommand = strtol(command, NULL, 16);
  switch(StartCommand)
  {
    case 0://Standby/Stop
	  resetDisplay()
      display.println("Stop");
      display.display();
      RaceStart = false;
      TekeningStart = false;
      DoolhofStart = false;
      SPSStart = false;
      Stop();
      break;
  case 1://Race
    RaceStart = true;
    TekeningStart = false;
    DoolhofStart = false;
    SPSStart = false;
    break;
  case 2://Tekening
    RaceStart = false;
    TekeningStart = true;
    DoolhofStart = false;
    SPSStart = false; 
    break;
  case 3://Doolhof
    RaceStart = false;
    TekeningStart = false;
    DoolhofStart = true;
    SPSStart = false;
    break;
  case 4://Steen-Papier-Schaar (SPS)
    RaceStart = false;
    TekeningStart = false;
    DoolhofStart = false;
    SPSStart = true;
    break;
  case 8://SPS Gewonnen
    resetDisplay()
    display.println("Steen-Papier-Schaar Gewonnen");
    display.display();
    break;
   case 9://SPS Verloren
    resetDisplay()
    display.println("Steen-Papier-Schaar Verloren");
    display.display();
    break;
  case 10://SPS Gelijkspel
  	resetDisplay()
    display.println("Steen-Papier-Schaar Gelijkspel");
    display.display();
    break;
  }
}

//race code blokie

void StartRace()
{
  if(RaceStart == true)
  {
 IR_Rechts_val = analogRead(IR_Rechts);
  IR_Links_val = analogRead(IR_Links);
  VL53L0X_RangingMeasurementData_t measure;
  delay(10);
  // rechtdoor rijden 
  if(IR_Rechts_val < sensBlack && IR_Links_val < sensBlack){
    plankGasRace();
  } // turn left
  else if(IR_Rechts_val < sensBlack && IR_Links_val > sensBlack) {
  turnLeftRace();
  } // turn right
  else if (IR_Rechts_val > sensBlack && IR_Links_val < sensBlack){
  turnRightRace();
  }
  else if (IR_Rechts_val > sensBlack && IR_Links_val > sensBlack){
    driveBackRace();
  }  else if (measure.RangeMilliMeter < 200){
  stopGameRace();
  }
  }
}

void plankGasRace(){
    ledcWrite(SV_motor_L, 100);
    ledcWrite(SA_motor_L, 0);
    ledcWrite(SV_motor_R, 100);
    ledcWrite(SA_motor_R, 0);

    resetDisplay();
    display.println("recht Door");
    display.display();
    delay(20);
    stopDrivingRace();
}
void turnLeftRace(){
    ledcWrite(SV_motor_L, 0);
    ledcWrite(SA_motor_L, 100);
    ledcWrite(SV_motor_R, 0);
    ledcWrite(SA_motor_R, 0);

    resetDisplay();
    display.println("turn links");
    display.display();

    delay(20);
    stopDrivingRace();
  
}
void turnRightRace(){
    ledcWrite(SV_motor_L, 0);
    ledcWrite(SA_motor_L, 0);
    ledcWrite(SV_motor_R, 0);
    ledcWrite(SA_motor_R, 100);

    resetDisplay();
    display.println("turn rechts");
    display.display();

    delay(20);
    stopDrivingRace();
}
void driveBackRace(){
    ledcWrite(SV_motor_L, 0);
    ledcWrite(SA_motor_L, 100);
    ledcWrite(SV_motor_R, 0);
    ledcWrite(SA_motor_R, 100);

    resetDisplay();
    display.println("achteruit");
    display.display();

    delay(100);
    stopDrivingRace();
}
void stopGameRace(){
    resetDisplay();
    display.println("STOP");
    display.display();

  	stopDrivingRace();
    delay(1000);
}
void stopDrivingRace(){
    ledcWrite(SV_motor_L, 0);
    ledcWrite(SA_motor_L, 0);
    ledcWrite(SV_motor_R, 0);
    ledcWrite(SA_motor_R, 0);
}
// alle race functies

//tekening code blokie

void StartTekening()
{
  if(TekeningStart == true)
  {
	// code van de tekening
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

//doolhof code blokie

void StartDoolhof()
{
  if(DoolhofStart == true)
  {
  IR_Rechts_val = analogRead(IR_Rechts);
	IR_Links_val = analogRead(IR_Links);
	VL53L0X_RangingMeasurementData_t measure;
	lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!
	delay(10);
	// look for when to stop
	if (measure.RangeMilliMeter < 150){
		Serial.print("Distance (mm): "); Serial.println(measure.RangeMilliMeter);
		stopGameDoolhof();
	} // rechtdoor rijden 
	else if(IR_Rechts_val < sensBlack && IR_Links_val < sensBlack){
		plankGasDoolhof();
	} // turn left
	else if(IR_Rechts_val > sensBlack && IR_Links_val < sensBlack) {
	turnLeftDoolhof();
	} // turn right
	else if (IR_Rechts_val < sensBlack && IR_Links_val > sensBlack){
	turnRightDoolhof();
	}// drive back and turn
	else if (IR_Rechts_val > sensBlack && IR_Links_val > sensBlack){
		driveBackDoolhof();
	} 
}
// alle Doolhof functies
void plankGasDoolhof(){
    resetDisplay();
    display.println("recht Door");
    display.display();

    ledcWrite(SV_motor_L, 200);
    ledcWrite(SA_motor_L, 0);
    ledcWrite(SV_motor_R, 200);
    ledcWrite(SA_motor_R, 0);

    delay(20);
  stopDrivingDoolhof();
  
}
void turnLeftDoolhof(){
    resetDisplay();
    display.println("turn links");
    display.display();

    driveBackwardsDoolhof();

    ledcWrite(SV_motor_L, 0);
    ledcWrite(SA_motor_L, 120);
    ledcWrite(SV_motor_R, 120);
    ledcWrite(SA_motor_R, 0);
    delay(50);
}
void turnRightDoolhof(){
    resetDisplay();
    display.println("turn rechts");
    display.display();

    driveBackwardsDoolhof();

    ledcWrite(SV_motor_L, 90);
    ledcWrite(SA_motor_L, 0);
    ledcWrite(SV_motor_R, 0);
    ledcWrite(SA_motor_R, 90);
    delay(50);
}
void driveBackDoolhof(){
    resetDisplay();
    display.println("achteruit");
    display.display();

    driveBackwardsDoolhof();

    ledcWrite(SV_motor_L, 0);
    ledcWrite(SA_motor_L, 100);
    ledcWrite(SV_motor_R, 100);
    ledcWrite(SA_motor_R, 0);

    delay(100);
  stopDrivingDoolhof();
}
void stopGameDoolhof(){
    resetDisplay();
    display.println("STOP");
    display.display();

    stopDrivingDoolhof();
    delay(500);
}
void driveBackwardsDoolhof(){
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
//sps code blokie

void StartSPS()
{
  if(SPSStart == true)
  {
    number = random(1,4);

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
	}
	delay(3000);
  }
}

//resetdisplay code blokie

void resetDisplay(){
  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0,10);             // Start at top-left corner
}
