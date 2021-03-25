#include <Wire.h> 
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_VL53L0X.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

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

int number = 1;
void setup() {
  pinMode(V_motor_L, OUTPUT);
  pinMode(A_motor_L , OUTPUT);

  pinMode(V_motor_R, OUTPUT);
  pinMode(A_motor_R , OUTPUT);

  pinMode(IR_Rechts, INPUT);
  pinMode(IR_Links, INPUT);
  Serial.begin(9600);
  
  // wait until serial port opens for native USB devices
  while (! Serial) {
    delay(1);
  }
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }

  ledcSetup(SV_motor_L, freq, resolution);
  ledcAttachPin(V_motor_L, SV_motor_L);

  ledcSetup(SA_motor_L, freq, resolution);
  ledcAttachPin(A_motor_L, SA_motor_L);

  ledcSetup(SV_motor_R, freq, resolution);
  ledcAttachPin(V_motor_R, SV_motor_R);

  ledcSetup(SA_motor_R, freq, resolution);
  ledcAttachPin(A_motor_R, SA_motor_R);


  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  // Display static text
  display.println("Hello, world!");
  display.display();
  delay(100);
}

void loop() {
  IR_Rechts_val = analogRead(IR_Rechts);
  IR_Links_val = analogRead(IR_Links);
  VL53L0X_RangingMeasurementData_t measure;
  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!
  delay(10);
  // look for when to stop
  if (measure.RangeMilliMeter < 150){
    Serial.print("Distance (mm): "); Serial.println(measure.RangeMilliMeter);
    stopGame();
  } // rechtdoor rijden 
  else if(IR_Rechts_val < sensBlack && IR_Links_val < sensBlack){
    plankGas();
  } // turn left
  else if(IR_Rechts_val > sensBlack && IR_Links_val < sensBlack) {
  turnLeft();
  } // turn right
  else if (IR_Rechts_val < sensBlack && IR_Links_val > sensBlack){
  turnRight();
  }// drive back and turn
  else if (IR_Rechts_val > sensBlack && IR_Links_val > sensBlack){
    driveBack();
  } 
}

void plankGas(){
    resetDisplay();
    display.println("recht Door");
    display.display();

    ledcWrite(SV_motor_L, 200);
    ledcWrite(SA_motor_L, 0);
    ledcWrite(SV_motor_R, 200);
    ledcWrite(SA_motor_R, 0);

    delay(20);
  stopDriving();
  
}
void turnLeft(){
    resetDisplay();
    display.println("turn links");
    display.display();

    driveBackwards();

    ledcWrite(SV_motor_L, 0);
    ledcWrite(SA_motor_L, 120);
    ledcWrite(SV_motor_R, 120);
    ledcWrite(SA_motor_R, 0);
    delay(50);
}
void turnRight(){
    resetDisplay();
    display.println("turn rechts");
    display.display();

    driveBackwards();

    ledcWrite(SV_motor_L, 90);
    ledcWrite(SA_motor_L, 0);
    ledcWrite(SV_motor_R, 0);
    ledcWrite(SA_motor_R, 90);
    delay(50);
}
void driveBack(){
    resetDisplay();
    display.println("achteruit");
    display.display();

    driveBackwards();

    ledcWrite(SV_motor_L, 0);
    ledcWrite(SA_motor_L, 100);
    ledcWrite(SV_motor_R, 100);
    ledcWrite(SA_motor_R, 0);

    delay(100);
  stopDriving();
}
void stopGame(){
    resetDisplay();
    display.println("STOP");
    display.display();

    stopDriving();
    delay(500);
}
void driveBackwards(){
  ledcWrite(SV_motor_L, 0);
    ledcWrite(SA_motor_L, 140);
    ledcWrite(SV_motor_R, 0);
    ledcWrite(SA_motor_R, 140);
    delay(200);
}
void stopDriving(){
    ledcWrite(SV_motor_L, 0);
    ledcWrite(SA_motor_L, 0);
    ledcWrite(SV_motor_R, 0);
    ledcWrite(SA_motor_R, 0);
}
void resetDisplay(){
  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0,10);             // Start at top-left corner
}
