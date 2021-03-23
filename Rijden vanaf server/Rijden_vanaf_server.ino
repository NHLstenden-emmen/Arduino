#define MOVEFORWARD HIGH
#define MOVEBACKWARD LOW

#define STOP 'a'
#define FORWARD 'b'
#define BACKWARD 'c'
#define LEFT 'd'
#define RIGHT 'e'

/* Libraries */
#include <WiFi.h>
#include <WebSocketsClient.h>
#include <analogWrite.h>
#include "MPU9250.h";
#include "Adafruit_VL53L0X.h"
#include <stdio.h> 
#include <ctype.h>
    /* motor aansluitingen */ 
/* motor rechts */
int motor2pin1 = 16;
int motor2pin2 = 17;

/* motor links */
int motor1pin1 = 18;
int motor1pin2 = 5;

  /*infrarood aansluitingen */
int infraredSensorLeft = 39;
int statusInfraredSensorLeft;
int infraredSensorRight = 34;
int statusInfraredSensorRight;

  /*Gyroscoop instellingen */
MPU9250 IMU(Wire,0x68);
int IMUstatus;
int gyroValue = IMU.getMagY_uT(); 
int gyroValueOld = IMU.getMagY_uT();
int startValue;
int endValue;
int yValue;
int difference;
boolean bool1 = false;
 
  /* Wifi instellingen */
const char* BOT_NAME = "INF1B";
char* ssid = "Arduino";
char* pass = "overleggen";  
WebSocketsClient webSocket;

  /* Afstandssensor */ 
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

/* overige variabelen */
char wifiMessage = 0;
unsigned long previousMillis = 0; 
const long interval = 2000;
boolean isForward = false;
/*-------------------------------------------------------------Setup --------------------------------------------------------------------*/

void setup() 
{
  /* Pinmodes */
  pinMode(motor1pin1,OUTPUT);
  pinMode(motor1pin2,OUTPUT);
  pinMode(motor2pin1,OUTPUT);
  pinMode(motor2pin2,OUTPUT);
  pinMode(infraredSensorLeft,INPUT);
  pinMode(infraredSensorRight,INPUT);
  /* Setups */
  Serial.begin(115200);  
  IMUstatus = IMU.begin();
  lox.begin();
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
  if(IMUstatus < 0)
  {
    Serial.println("IMU connectie mislukt.");
  }
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }
  // server address, port and URL
  webSocket.begin("194.171.181.139", 49154, "/");
  
  // event handler
  webSocket.onEvent(webSocketEvent);
  
  // try ever 5000 again if connection has failed
  webSocket.setReconnectInterval(5000);

   bool1 = false;
   moveCar(STOP);
}
/*-------------------------------------------------------------Loop---------------------------------------------------------------------*/
void loop() 
{
  statusInfraredSensorLeft = analogRead(infraredSensorLeft);
  statusInfraredSensorRight = analogRead(infraredSensorRight);
  IMU.readSensor();    
/*  if(bool1 == false)
  {
    Serial.print(statusInfraredSensorLeft);
    Serial.print("\t");
    Serial.println(statusInfraredSensorRight);
   if(statusInfraredSensorLeft > 750 && statusInfraredSensorRight > 750)
   {
    move90Left();
    statusInfraredSensorLeft = analogRead(infraredSensorLeft);
    statusInfraredSensorRight = analogRead(infraredSensorRight);
    if(statusInfraredSensorLeft > 750 && statusInfraredSensorRight > 750)
    {
      move90Right();
      move90Right();
    }
   }
   else
   {
    moveCar(FORWARD);
   }    
  }  */   
  
  webSocket.loop(); 
  moveCar(wifiMessage);
  if(isForward)
  {
    VL53L0X_RangingMeasurementData_t measure;
    lox.rangingTest(&measure, false);
    Serial.println(measure.RangeMilliMeter);
    if(measure.RangeMilliMeter < 60 && measure.RangeMilliMeter > 0)
    {
      moveCar(STOP);
    }
  }
}


/*-------------------------------------------------------------Functies-------------------------------------------------------------------*/
 

void moveMotorRight(boolean direction, int speed)
{
  if(direction == true)
  {
    analogWrite(motor1pin1,speed);
    digitalWrite(motor1pin2,LOW);
  }
  else
  {
    digitalWrite(motor1pin1,LOW);
    analogWrite(motor1pin2,speed);
  }
}
void moveMotorLeft(boolean direction, int speed)
{
  if(direction == true)
  {
    analogWrite(motor2pin1,speed);
    digitalWrite(motor2pin2,LOW);
  }
  else
  {
    digitalWrite(motor2pin1,LOW);
    analogWrite(motor2pin2,speed);
  }
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
      // send message to server when Connected
      webSocket.sendTXT(BOT_NAME);
      break;
    case WStype_TEXT:
      Serial.printf("[WSc] get text: %s\n", payload);
      commandReceiver(payload);
      // send message to server
      break;
  }
}

void commandReceiver(uint8_t* command)
{
    Serial.print("byte   : ");
    Serial.println(F(command));
    wifiMessage = *command;
}

void moveCar(char moveDirection)
{
  isForward = false;
  moveDirection  = tolower(moveDirection);
  switch (moveDirection)
  {
    case STOP: // STOP
      stopCar();  
      break;
    case FORWARD: // Forward
    stopCar();
      moveMotorRight(MOVEFORWARD,220);
      moveMotorLeft(MOVEFORWARD,220);
      isForward = true;
      break;
    case BACKWARD: // backwards
      stopCar();
      moveMotorRight(MOVEBACKWARD,220);
      moveMotorLeft(MOVEBACKWARD,220);
      break;
    case LEFT: // move LEFT
      stopCar();
      moveMotorRight(MOVEFORWARD,250);
      break;
    case RIGHT: // move  RIGHT
      stopCar();
      moveMotorLeft(MOVEFORWARD,250);
      break;
  }
}

void stopCar()
{
  moveMotorRight(MOVEFORWARD,0);
  moveMotorLeft(MOVEFORWARD,0);
  moveMotorRight(MOVEBACKWARD,0);
  moveMotorLeft(MOVEBACKWARD,0);   
}

void move90Right()
{
  stopCar();
  IMU.readSensor();
  yValue = IMU.getMagY_uT() + 24;
  Serial.print("Y value is : ");
  Serial.println(yValue);
  if(yValue <= 15 && yValue > 5)
  {
    Serial.println("loop 1 move Right");
    gyroValue = IMU.getMagY_uT() + 73;
    endValue = gyroValue + 15;
    Serial.print(gyroValue);
    Serial.print(" < ");
    Serial.println(endValue);
    unsigned long currentMillis = millis();
    previousMillis = currentMillis;
    while(gyroValue < endValue)
    {
      unsigned long currentMillis = millis();
      IMU.readSensor();
      gyroValue = IMU.getMagY_uT() + 73;    
      Serial.print("\t");
      Serial.print("y in WHILE : ");
      Serial.println(gyroValue);
      moveMotorRight(MOVEBACKWARD,210);
      moveMotorLeft(MOVEFORWARD,210);
      if(currentMillis - previousMillis >= interval)
      {
        Serial.println("interval");
        previousMillis = currentMillis;
        break;    
      }
    }
  }
  else if(yValue <= 30 && yValue > 15)
  {
    Serial.println("loop 2 move Right");
    gyroValue = IMU.getMagY_uT() - 15;
    endValue = gyroValue - 12;
    Serial.print(gyroValue);
    Serial.print(" > ");
    Serial.println(endValue);
    unsigned long currentMillis = millis();
    previousMillis = currentMillis;
    while(gyroValue > endValue)
    {
      unsigned long currentMillis = millis();
      IMU.readSensor();
      gyroValue = IMU.getMagY_uT() - 15;
      Serial.print("\t");
      Serial.print("y in WHILE : ");
      Serial.println(gyroValue);
      moveMotorRight(MOVEBACKWARD,210);
      moveMotorLeft(MOVEFORWARD,210);
      if(currentMillis - previousMillis >= interval)
      {
        Serial.println("interval");
        previousMillis = currentMillis;
        break;    
      }
    }    
  }
  else if(yValue <= 5 && yValue > -5)
  {
    Serial.println("loop 3 move Right");
    gyroValue = IMU.getMagY_uT() - 5;
    endValue = gyroValue + 2;
    Serial.print(gyroValue);
    Serial.print(" < ");
    Serial.println(endValue);
    unsigned long currentMillis = millis();
    previousMillis = currentMillis;
    while(gyroValue < endValue)
    {
      unsigned long currentMillis = millis();
      IMU.readSensor(); 
      gyroValue = IMU.getMagY_uT() - 5;
      Serial.print("\t");
      Serial.print("y in WHILE : ");
      Serial.println(gyroValue);
      moveMotorRight(MOVEBACKWARD,210);
      moveMotorLeft(MOVEFORWARD,210);
      if(currentMillis - previousMillis >= interval)
      {
        Serial.println("interval");
        previousMillis = currentMillis;
        break;    
      }
    }    
  }
  else
  {
    Serial.println(yValue);
    moveMotorRight(MOVEBACKWARD,210);
    moveMotorLeft(MOVEFORWARD,210);
    delay(500);
  }
}

void move90Left()
{
  stopCar();
  IMU.readSensor();
  yValue = IMU.getMagY_uT() + 56;
  if(yValue <= 18 && yValue > 5)
  {
    Serial.println("loop 1 move LEFT");
    gyroValue = IMU.getMagY_uT() + 73;
    endValue = gyroValue + 50;
    Serial.print(gyroValue);
    Serial.print(" > ");
    Serial.println(endValue);
    unsigned long currentMillis = millis();
    previousMillis = currentMillis;
    while(gyroValue < endValue)
    {
      unsigned long currentMillis = millis();
      IMU.readSensor();
      gyroValue = IMU.getMagY_uT() + 73;    
      Serial.print("\t");
      Serial.print("y in WHILE : ");
      Serial.println(gyroValue);
      moveMotorRight(MOVEFORWARD,210);
      moveMotorLeft(MOVEBACKWARD,210);    
      if(currentMillis - previousMillis >= interval)
      {
        Serial.println("interval");
        previousMillis = currentMillis;
        break;    
      }
    }
  }
  else if(yValue <= 30 && yValue > 18)
  {
    Serial.println("loop 2 move LEFT");
    gyroValue = IMU.getMagY_uT() - 15;
    endValue = gyroValue + 12;
    Serial.print(gyroValue);
    startValue=  gyroValue;
    Serial.print(" < ");
    Serial.println(endValue);
    unsigned long currentMillis = millis();
    previousMillis = currentMillis;
    while(gyroValue < endValue)
    {
      unsigned long currentMillis = millis();
      IMU.readSensor();
      gyroValue = IMU.getMagY_uT() - 15; 
      Serial.print("\t");
      Serial.print("y in WHILE : ");
      Serial.println(gyroValue);
      moveMotorRight(MOVEFORWARD,210);
      moveMotorLeft(MOVEBACKWARD,210);
      if(currentMillis - previousMillis >= interval)
      {
        Serial.println("interval");
        previousMillis = currentMillis;
        break;    
      }
    }    
  }
  else if(yValue <= 5 && yValue > -5)
  {
    Serial.println("loop 3 move LEFT");
    gyroValue = IMU.getMagY_uT() - 5;
    endValue = gyroValue - 2;
    Serial.print(gyroValue);
    Serial.print(" > ");
    Serial.println(endValue);
    unsigned long currentMillis = millis();
    previousMillis = currentMillis;
    while(gyroValue > endValue)
    {
      unsigned long currentMillis = millis();
      IMU.readSensor();
      gyroValue = IMU.getMagY_uT() - 5;
      gyroValueOld = gyroValue;
      Serial.print("\t");
      Serial.print("y in WHILE : ");
      Serial.println(gyroValue);
      moveMotorRight(MOVEFORWARD,210);
      moveMotorLeft(MOVEBACKWARD,210);
      if(currentMillis - previousMillis >= interval)
      {
        Serial.println("interval");
        previousMillis = currentMillis;
        break;    
      }
    }    
  }
  else
  {
    Serial.println(yValue);
    moveMotorRight(MOVEFORWARD,210);
    moveMotorLeft(MOVEBACKWARD,210);
    delay(500);
  }
  
}
