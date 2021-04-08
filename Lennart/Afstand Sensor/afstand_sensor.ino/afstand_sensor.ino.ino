#include <Wire.h> 
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_VL53L0X.h>

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
