#include "Filter.h"

#define SONAR_ECHO              5
#define SONAR_TRIG              4
#define SONAR_BUFFER_SIZE       16
int sonar;
int sonarBuffer [SONAR_BUFFER_SIZE];
int nextRunningAverage;

ExponentialFilter<int> FilteredSonar(20,0);

unsigned long currentMillis = 0;
unsigned long previousMillis = 0;

int averaging(float);
int updateSonar();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(SONAR_ECHO, INPUT);
  pinMode(SONAR_TRIG, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  currentMillis = millis();
  sonar = updateSonar();
  FilteredSonar.Filter(sonar);
  int SmoothSonar = FilteredSonar.Current();
  Serial.println(SmoothSonar);
  previousMillis = currentMillis;
}

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
  // Reads the echoPin, returns the sound wave travel time in microseconds, and then caculate distance
  return pulseIn(SONAR_ECHO, HIGH, 100000) *0.034/2 ;
}

int average() {
  int rawSonar = updateSonar();
  sonarBuffer[nextRunningAverage++] = rawSonar;
  if(nextRunningAverage >= SONAR_BUFFER_SIZE) {
    nextRunningAverage = 0;
  }
  int runningAverage = 0;
  for(int i = 0; i < SONAR_BUFFER_SIZE; ++i) {
    runningAverage += sonarBuffer[i];
  }
  runningAverage /= SONAR_BUFFER_SIZE;
  return runningAverage;
}


