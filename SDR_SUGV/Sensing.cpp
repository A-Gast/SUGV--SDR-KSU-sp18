#include "Arduino.h"
#include "Sensing.h"
#include "Protocol.h"
#include "Localization.h"
#include "SDR_SUGV.h"

int lineR = 0; 
int lineL = 0;
int sonar = 0; 

volatile long odometerR = 0; 
volatile long odometerL = 0; 

char sonarBuffer [SONAR_BUFFER_SIZE];

int updateSonar(){
  sonar = 0;
  long duration, distance;
  // Clears the trigPin
  digitalWrite(SONAR_TRIG, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(SONAR_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(SONAR_TRIG, LOW);
  sonarClock = 0;
  // Reads the echoPin, returns the sound wave travel time in microseconds, and then caculate distance
  return pulseIn(SONAR_ECHO, HIGH, 100000) *0.034/2 ;
}

void updateLineSensor(){
  lineR = digitalRead(LINE_RIGHT);
  lineL = digitalRead(LINE_LEFT);
  lineClock = 0;
}

void updateLineBuffer() {
  
}

void updateSonarBuffer() {
  //average buffer
  //min/max cut off
    //throw out min & max
}

void rightOdometer(){
  if (speedR > 0){
    odometerR++;
  }else if (speedR < 0){
    odometerR--;
  }
}
void leftOdometer(){
  if (speedL > 0){
    odometerL++;
  }else if (speedR < 0){
    odometerL--;
  }
}

