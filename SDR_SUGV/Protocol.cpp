#include "Arduino.h"
#include "Protocol.h"
#include "Localization.h"
#include "Sensing.h"
#include "SDR_SUGV.h"

char msgBuffer[MESSAGE_BUFFER_SIZE];
int msgBufferPointer = 0;

int mSpeedR = 0; // control message from the remote
int mSpeedL = 0; // control message from the remote



void evaluateStringCommand();
void evaluateBinaryCommand();
void echoCommand();

void init_buffer(){
  for (int i = 0; i < MESSAGE_BUFFER_SIZE; i++){
    msgBuffer[i] = 0x00;
  }
  msgBufferPointer = 0;
}

void updateCommand(){
  if (Serial.available() > 0){
    char tmpChar = Serial.read();      
    if ((msgBufferPointer == 0)&&(tmpChar == '<')){           // '<' 0x3C 
      msgBuffer[msgBufferPointer] = tmpChar; msgBufferPointer++;
    }else if (msgBufferPointer == 1){
      if  ((tmpChar == '@')||(tmpChar == '!')){     // '!'  (0x21 ==> Binary Format)or '@' (0x40 ==> String format)
        msgBuffer[msgBufferPointer] = tmpChar; msgBufferPointer++;      
      }else{
        msgBufferPointer = 0;
      }
    }else if (msgBufferPointer == 2){
      if (tmpChar == ' '){     // ' ' (0x20 ==> Space)
        msgBuffer[msgBufferPointer] = tmpChar; msgBufferPointer++;      
      }else{
        msgBufferPointer = 0;
      }
    }else if (msgBufferPointer > MESSAGE_BUFFER_SIZE){ 
      msgBufferPointer = 0; 
      if ( tmpChar == '<') {
        msgBuffer[0] = tmpChar;    
        msgBufferPointer = 1;          
      }
    }else if (msgBufferPointer > 2) {
      if (( msgBuffer[1] == '@')&& (tmpChar == '>')) {
        //echoCommand();
        evaluateStringCommand();
        msgBufferPointer = 0;        
      }else if  (( msgBuffer[1] == '!')&& (msgBuffer[3] == msgBufferPointer )) {
        evaluateBinaryCommand();
        msgBufferPointer = 0;
      }else{
        msgBuffer[msgBufferPointer] = tmpChar;
        msgBufferPointer++;
      }
    }else if((msgBufferPointer == 0) && (tmpChar == 'k')){
      kill();
      msgBufferPointer = 0;
    }else if((msgBufferPointer == 0) && (tmpChar == 'v')){
      manStop();
      msgBufferPointer = 0;
    }else if((msgBufferPointer == 0) && (tmpChar == 'w')){
      manFwd();
      msgBufferPointer = 0;
    }else if((msgBufferPointer == 0) && (tmpChar == 's')){
      manBkwd();
      msgBufferPointer = 0;
    }else if((msgBufferPointer == 0) && (tmpChar == 'a')){
      manLeft();
      msgBufferPointer = 0;
    }else if((msgBufferPointer == 0) && (tmpChar == 'd')){
      manRight();
      msgBufferPointer = 0;
    }
  }  
}

void evaluateStringCommand(){
  if (msgBuffer[3] == ATR_MSG_ECHO)   echoCommand();
  else if (msgBuffer[3] == ATR_STATUS)  statusCommand();
  else if (msgBuffer[3] == ATR_SET_MOTOR) setMotorPower();
  else if (msgBuffer[3] == ATR_SET_AUTO) setAuto();
  
}
void evaluateBinaryCommand(){
  Serial.print("N/A-");
  echoCommand();
}

void echoCommand(){
  Serial.print("[");
  for(int i = 0; i <= msgBufferPointer; i++){
    Serial.print(msgBuffer[i]);
  }
  Serial.println("]");
}

void statusCommand(){
  Serial.print("[");Serial.print(autoFlag); Serial.print("="); Serial.print(mSpeedR);Serial.print(" : "); Serial.print(mSpeedL); 
  Serial.print("  +  ");Serial.print(odometerR); Serial.print(" : "); Serial.print(odometerL); Serial.print(" + "); 
  Serial.print(lineR); Serial.print(" : "); Serial.print(lineL); Serial.print(" + ");  Serial.print(sonar); Serial.println("]"); 
}

void setMotorPower(){
    char *p = msgBuffer;
    String str;
    int cnt = 0;
    while ((str = strtok_r(p, " ", &p)) != NULL) { // delimiter is the space
//      if (cnt == 2) mSpeedR = map( str.toInt(), 0 , 1024, -255, 255);
//      if (cnt == 3) mSpeedL = map( str.toInt(), 0 , 1024, -255, 255);
      if (cnt == 2) mSpeedR = str.toInt();
      if (cnt == 3) mSpeedL = str.toInt();
      if (cnt == 4) autoFlag = str.toInt(); 
      cnt++;
    }
    statusCommand();
}

void setAuto(){
    char *p = msgBuffer;
    String str;
    int cnt = 0;
    while ((str = strtok_r(p, " ", &p)) != NULL) { // delimiter is the space
//      if (cnt == 2) mSpeedR = map( str.toInt(), 0 , 1024, -255, 255);
//      if (cnt == 3) mSpeedL = map( str.toInt(), 0 , 1024, -255, 255);
      if (cnt == 2) mSpeedR = str.toInt();
      if (cnt == 3) mSpeedL = str.toInt();
      cnt++;
    }
    
    if(autoFlag == 1) {
      Serial.println("Manual Override");
      autoFlag = 0;
    } else {
      Serial.println("Automatic Driving Enabled");
      autoFlag = 1;
    }
    statusCommand();
}

void kill() {
  Serial.println("Killing robot");
  autoFlag = -1;
  stopFlag = 1;
  mSpeedR = 0;
  mSpeedL = 0;
}

void manStop() {
  if(stopFlag == 1) {
    Serial.println("Resuming");
    stopFlag = 0;
  } else {
    Serial.println("Stopping");
    stopFlag = 1;
  }
}
void manFwd() {
  //Serial.print("Increasing Speed.. L/R: ");
  if(mSpeedL == 0) {
    mSpeedL = mSpeedR;
  } else if(mSpeedR == 0) {
    mSpeedR = mSpeedL;
  }
  if(mSpeedR < 231) {
    mSpeedR += 25;    
  } else {
    mSpeedR = 255;
  }
  if(mSpeedL < 231) {
    mSpeedL += 25;
  } else {
    mSpeedL = 255;
  }
  //Serial.print(mSpeedL);
  //Serial.print(" / ");
  //Serial.println(mSpeedR);
}
void manBkwd() {
  //Serial.println("Decreasing Speed.. L/R: ");
  if(mSpeedL == 0) {
    mSpeedL = mSpeedR;
  } else if(mSpeedR == 0) {
    mSpeedR = mSpeedL;
  }
  if(mSpeedR > -231) {
    mSpeedR -= 25;    
  } else {
    mSpeedR = -255;
  }
  if(mSpeedL > -231) {
    mSpeedL -= 25;
  } else {
    mSpeedL = -255;
  }
  //Serial.print(mSpeedL);
  //Serial.print(" / ");
  //Serial.println(mSpeedR);
}
void manLeft() {
  //Serial.println("Turning Left.. L/R: ");  
  mSpeedL = 0;
  //Serial.print(mSpeedL);
  //Serial.print(" / ");
  //Serial.println(mSpeedR);
}
void manRight() {
  //Serial.println("Turning Right.. L/R: ");  
  mSpeedR = 0;
  //Serial.print(mSpeedL);
  //Serial.print(" / ");
  //Serial.println(mSpeedR);
}

