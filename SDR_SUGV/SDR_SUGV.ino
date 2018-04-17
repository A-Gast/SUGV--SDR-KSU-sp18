#include "Arduino.h"
#include "SDR_SUGV.h"
#include "Protocol.h"
#include "Localization.h"
#include "Sensing.h"
#include "Action.h"

int stopFlag = 0;
int autoFlag = -1;   // Auto or Manual Control

unsigned long sonarClock = 0;
unsigned long lineClock = 0;
unsigned long odometerClock = 0;
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
int cnt = 0;

//int tmSpeedL = 200;         //hard-coded speed for testing
//int tmSpeedR = 200;         //hard-coded speed for testing

void setup() {
  // put your setup code here, to run once:
  initialization();
  //autoFlag = 1;
}

void loop() {
  currentMillis = millis();
  updateCommand();                  //take in message from user
  updateSensors();                  //gather data about environment
  updatePlan();                     //plan based on environment data
  updateAction();                   //execute plan
  previousMillis = currentMillis;  
}

void updatePlan() {
  if (autoFlag == 1){ //Auto-driving
    //set speeds and actions using sensors
    Serial.print("speedL: ");
    Serial.println(speedL);
    Serial.print("speedR: ");
    Serial.println(speedR);

    //if sonar detects an obstacle, stop
    if(sonar > 0 && sonar < SONAR_STOP_DISTANCE) {
      stopFlag = 1;
      //Serial.print("Object detected at ");
      //Serial.print(sonar);
      //Serial.println("cm..");
      speedL = 0;
      speedR = 0;

      //if line detectors both detect a line, stop
    } else if(lineL && lineR) {
      stopFlag = 1;
      //Serial.println("Line detected head on, stop!");
      speedL = 0;
      speedR = 0;

      //if no reason to stop, reset stopFlag to drive again
    } else {
      stopFlag = 0;
    }
    
    if(stopFlag == 0) {
      if(lineR) {
        //Serial.println("Line detected on the right side!");
        //lineRight();
        speedL = 0;         //stop left wheel to turn at a left angle
        speedR = mSpeedR;
        //speedR -= (speedR * 0.2);        //slow down right wheel to turn slower
        
      } else if(lineL) {
        //Serial.println("Line detected on the left side!");
        //lineLeft();
        speedL = mSpeedL;
        //speedL -= (speedL * 0.2);    //slow down the left wheel to turn slower
        speedR = 0;     //stop right wheel to turn at a right angle
        
      } else {
        //no obstacle or line, move forward
        speedL = mSpeedL;
        speedR = mSpeedR;
      }
    }
    
  } else if(autoFlag == 0 && stopFlag == 0){ //Manual Driving
    //set speed and actions using user input
    speedL = mSpeedL;
    speedR = mSpeedR;
    
  } else {
    //wait for the mode to be selected
    speedL = 0;
    speedR = 0;
  }
}

////////////////////////////////////////////////////////////////////////////////////

void updateAction(){
  updateMotor();
}

void updateSensors(){
//  updateLineSensor();
//  if((cnt % 100)){
//    sonar = updateSonar();
//  }
    if(lineClock > 300) {
    updateLineSensor();
  } else {
    lineClock += (currentMillis - previousMillis);
  }
  
  if(sonarClock > 100) {
    sonar = updateSonar();
  } else {
    sonarClock += (currentMillis - previousMillis);
  }
}

void initialization(){
  currentMillis = millis();
  init_communication();
  init_pin();
}

void init_communication(){
  Serial.begin(115200);
  Serial.println("Program Start...v01");
  init_buffer(); 
}

void init_pin(){
  //Motor pin setting
  pinMode(MOTOR_LEFT_ENABLE, OUTPUT); pinMode(MOTOR_LEFT_A, OUTPUT); pinMode(MOTOR_LEFT_B, OUTPUT);
  pinMode(MOTOR_RIGHT_ENABLE, OUTPUT); pinMode(MOTOR_RIGHT_A, OUTPUT); pinMode(MOTOR_RIGHT_B, OUTPUT);
  //Sonar pin setting
  pinMode(SONAR_ECHO, INPUT); pinMode(SONAR_TRIG, OUTPUT);
  pinMode(LINE_LEFT, INPUT_PULLUP); pinMode(LINE_RIGHT, INPUT_PULLUP); 
  attachInterrupt(0, rightOdometer, CHANGE);
  attachInterrupt(1, leftOdometer, CHANGE);
}
