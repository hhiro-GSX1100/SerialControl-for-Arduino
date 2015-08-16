//********************************************************************************************************************************
//* SerialControl シリアルにより入出力コントロール
//* command format: { Command, pin, state, value}
//*  使用可能なピンの値を得る       / Get the value of the available pin　         {GUP,0,0,0}
//*  ピンの入出力をコントロールする / It will pin control the input and output of　{SPS, pin, state, value}
//*　ピンのステータスを得る         / Get the pin status　                         {GPS,pin,0,0}  戻り形式も同様
//*
//*  I2Cをオン、オフする            / You want to turn on or off the I2C　         {I2C,0,0, value}
//*　読み込の戻り値                 / return value　　　　                         {CPV, pin, state, value}、　{I2C,・・・}
//********************************************************************************************************************************
#include <Wire.h>
#include "SerialAnalysis.h"
#include "PinClass.h"
#define USE_BME280  //Please comment out if you do not want to use the BME280
//#define DEBUG

#ifdef USE_BME280
  #include "BME280.h"
#endif

//**************************************************************
//* pin state
//**************************************************************
#define PIN_OFF       0
#define DIGITAL_READ  1
#define DIGITAL_WRITE 2
#define ANALOG_READ   3
#define ANALOG_WRITE  4

//**************************************************************
// 以下を各マイコンの値で設定すること / Please set according to the microcomputer to be used .
//#define USE_SERIAL Serial <- define by SerialAnalysis.h 
#define NUM_OF_PINS 20              //ピンの数を指定
#define CHECK_TIME_SPAN 100         //チェックする時間間隔　milli sec
#define CHECK_TIME_SPAN_I2C 1000    //I2Cをチェックする時間間隔　milli sec

//Set Pin State -->
PinType pin_type[NUM_OF_PINS] =
  {PT_DIGITAL  // 0
  ,PT_DIGITAL  // 1
  ,PT_DIGITAL  // 2
  ,PT_ANALOG_W  // 3
  ,PT_DIGITAL  // 4
  ,PT_ANALOG_W  // 5
  ,PT_ANALOG_W  // 6
  ,PT_DIGITAL  // 7
  ,PT_DIGITAL  // 8
  ,PT_ANALOG_W  // 9
  ,PT_ANALOG_W  //10
  ,PT_ANALOG_W  //11
  ,PT_DIGITAL  //12
  ,PT_DIGITAL  //13
  ,PT_ANALOG_R  //14
  ,PT_ANALOG_R  //15
  ,PT_ANALOG_R  //16
  ,PT_ANALOG_R  //17
  ,PT_ANALOG_R  //18
  ,PT_ANALOG_R  //19
  }; 
//Set Useable Pin
int useable_pins[NUM_OF_PINS] =
  {false  // 0
  ,false  // 1
  ,true  // 2
  ,true  // 3
  ,true  // 4
  ,true  // 5
  ,true  // 6
  ,true  // 7
  ,true  // 8
  ,true  // 9
  ,true  //10
  ,true  //11
  ,true  //12
  ,true  //13
  ,true  //14
  ,true  //15
  ,true  //16
  ,true  //17
  ,false  //18
  ,false  //19
  };  
// <--
//**************************************************************

PinClass *myPin[NUM_OF_PINS];
int pin_value[NUM_OF_PINS] = {};

#ifdef USE_BME280
  BME280 bme280;
#endif

SerialAnalysis serial_analysus;
char my_command[COMMAND_MAX + 1];
int my_pin = 0;
int my_state = 0;
int my_value = 0;
boolean i2c = false;
unsigned long chktime = 0;
unsigned long chktime_I2C = 0;

// the setup routine runs once when you press reset:
void setup() {    
  for(int i = 0; i < NUM_OF_PINS; ++i){
    myPin[i] = new PinClass(i, pin_type[i], useable_pins[i]);
    pin_value[i] = -1;
  }  
   //USE_SERIAL.begin(BAUD);
   #ifdef USE_BME280
     bme280.setup();
   #endif
   
   serial_analysus.setup();
   
   #ifdef DEBUG
      USE_Serial.print("BAUD RATE:");
      USE_Serial.println(BAUD);
   #endif
}

// the loop routine runs over and over again forever:
void loop() {
   
  if(serial_analysus.check()){
    serial_analysus.getCommand(my_command, &my_pin, &my_state, &my_value);
    #ifdef DEBUG
      USE_Serial.print("command:");
      USE_Serial.println(my_command);
      USE_Serial.print("Pin:");
      USE_Serial.println(my_pin);
      USE_Serial.print("state:");
      USE_Serial.println(my_state);
      USE_Serial.print("value:");
      USE_Serial.println(my_value);
   #endif
   String tmp_com = String(my_command);
   if(tmp_com == "SPS"){
     if(checkPinRange(my_pin))
       myPin[my_pin]->setPinState((PinState)my_state, my_value);
       pin_value[my_pin] = -1;
   }
   if(tmp_com == "I2C"){
     if(my_value > 0) i2c = true;
     else i2c = false;
   }
   if(tmp_com == "GUP"){
     getUsablePins();
   }
   if(tmp_com == "GPS"){
     getPinState(my_pin);
   }
  }else{
    //ex here
  }
  
  unsigned long now = millis();
  if(now > (chktime + CHECK_TIME_SPAN)){
    checkReadValue();
    chktime = millis();
  }else if(now < chktime){
    chktime = millis();
    #ifdef DEBUG
      USE_Serial.println("tmptime < chktime");
    #endif
  }
  
}

boolean checkPinRange(int pin){
  if(0 <= pin && pin < NUM_OF_PINS) return true;
  else return false;
}

void getUsablePins(){
  USE_Serial.print(" {GUP;");
  for(int i = 0; i < NUM_OF_PINS; ++i){
    //UseablePin
    if(useable_pins[i]) USE_Serial.print(1);
    else USE_Serial.print(0);
    USE_Serial.print(':');
    //PinState
    USE_Serial.print((int)pin_type[i]);
    if(i < NUM_OF_PINS - 1) USE_Serial.print(',');
  }
  USE_Serial.println('}');
}

void getPinState(int pin){
  if(!checkPinRange(pin)) return;
  if(!useable_pins[pin]) return;
    USE_Serial.print(" {GPS;");
    USE_Serial.print(pin);
    USE_Serial.print(',');
    USE_Serial.print((int)myPin[pin]->getPinState());
    USE_Serial.print(',');
    USE_Serial.print(myPin[pin]->getPinValue());
    USE_Serial.println('}');
}
void checkReadValue(){
  #ifdef DEBUG
    //USE_Serial.println("checkReadValue");
  #endif
  for(int i = 0; i < NUM_OF_PINS; ++i){
    if(!useable_pins[i]) continue;
    int tmp_value = myPin[i]->execute();
    if(tmp_value == pin_value[i]) continue;
    
    String str = "";
    pin_value[i] = tmp_value;
    PinState ps = myPin[i]->getPinState();
    if(ps == PS_DIGITAL_READ){
          str = " {CPV;" + String(i) + "," + String((int)PS_DIGITAL_READ) + "," + String(tmp_value) + "}";
          USE_Serial.println(str);
          #ifdef DEBUG
            USE_Serial.println("PS_DIGITAL_READ");
          #endif
    }else if(ps == PS_ANALOG_READ){
          str = " {CPV;" + String(i) + "," + String((int)PS_ANALOG_READ) + "," + String(tmp_value) + "}";
          USE_Serial.println(str);
          #ifdef DEBUG
            USE_Serial.println("PS_ANALOG_READ");
          #endif
    }
  }
  //***************************************************************************
  // I2C OUTPUT
  // If you want the output of the I2C, here to write a program.
  //***************************************************************************
  if(i2c){
    if(chktime_I2C > millis()) return;
    
    #ifdef USE_BME280
      double my_tmp = 0.0, my_press = 0.0, my_hum = 0.0;
      bme280.getData(&my_tmp, &my_press, &my_hum);
      USE_Serial.print(" {I2C;TEMP:");
      USE_Serial.print(my_tmp);
      USE_Serial.print(" DegC  PRESS:");
      USE_Serial.print(my_press);
      USE_Serial.print(" hPa  HUM:");
      USE_Serial.print(my_hum);
      USE_Serial.println(" %}");
    #endif
    
    chktime_I2C = millis() + CHECK_TIME_SPAN_I2C;
  }
}
