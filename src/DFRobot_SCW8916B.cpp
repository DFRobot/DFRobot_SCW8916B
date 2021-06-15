/*!
 * @file DFRobot_SCW8916B.cpp
 * @brief 隔空水位检测传感器，可隔空2mm准确检测到水和其他液体的位置变化。
 * @n 检测模式：
 * @n 1. 电平检测模式（电平一对一检测，通过检测电平来判断有水没水， 1（高电平）：有水， 0（低电平）： 无水）
 * @n 2. UART检测模式（通过发送串口命令检测有水没水）；
 *
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [Arya](xue.peng@dfrobot.com)
 * @version  V1.0
 * @date  2021-04-22
 * @https://github.com/DFRobot/DFRobot_SCW8916B
 */
#include <Arduino.h>
#include "DFRobot_SCW8916B.h"

DFRobot_Nilometer::DFRobot_Nilometer(Stream *s, int en)
  :_s(s),_out(-1),_test(-1),_en(en)
{
  _mode = eUARTDetecteMode;
  memset(&_rslt, 0, sizeof(_rslt));
}

DFRobot_Nilometer::DFRobot_Nilometer(int out, int en, int test, Stream *s)
 :_s(s),_out(out),_test(test),_en(en)
{
  _mode = eLevelDetecteMode;
  memset(&_rslt, 0, sizeof(_rslt));
}

DFRobot_Nilometer::~DFRobot_Nilometer(){}

//需要判定一下是否从来没有校准
int DFRobot_Nilometer::begin(){
  uint8_t val = 0,val1=0;
  int waitForTimeOutMs = 8000;
  int waitForTimeoutIncMs = 100;
  uint8_t count = 0;
  int t = 0;

  uCheckRslt_t rslt;
  if(_mode == eUARTDetecteMode){
      if(_s == NULL){
          DBG("Error: _s is NULL.");
          return -1;
      }
      while((val1 = _s->read()) != ERR_CALIBRATION_CODE){
          delay(waitForTimeoutIncMs);
          t += waitForTimeoutIncMs;
          memcpy(&rslt, &val1, 1);
          if(rslt.value + rslt.pad == 0x0F) return 0;
          if(t > waitForTimeOutMs){
              return 0;
          } 
      }
  }else{
      if(_out == -1){
          DBG("Error: _out = -1.");
          return -1;
      }
      pinMode(_out, INPUT);

      val = digitalRead(_out);
      while((val1 = digitalRead(_out)) == val){
          delay(waitForTimeoutIncMs);
          t += waitForTimeoutIncMs;
          if(t > waitForTimeOutMs){
              return 0;
          } 
      }
      val = val1;
      for(int i = 0; i < 5; i++){
          delay(100);
          val1 = digitalRead(_out);
          if(val + val1 == 1){
              count++;
              return 0;
          }
      }
      if(count < 2) return 0;
  }
  return ERR_CALIBRATION_CODE;
}

bool DFRobot_Nilometer::detectWater(){
  bool flag = false;
  uCheckRslt_t rslt;
  if(_mode == eUARTDetecteMode){
      readData(&rslt, 1);
      if(rslt.pad+rslt.value == 0x0F){
          flag = ( bool)rslt.ch1;
      }
      flush();
  }else{
      if(digitalRead(_out)){
          flag = true;
      }
      
  }
  delay(100);
  return flag;
}

bool DFRobot_Nilometer::selfCheck(){
  if(_mode == eUARTDetecteMode){
      return uartSelfCheck(_en);
  }else{
      return ioSelfCheck(_en, _test, _s);
  }
}

bool DFRobot_Nilometer::calibration(){
  bool flag;
  if(_mode == eUARTDetecteMode){
      flag = uartWaterLevelCalibration(_en, CALIB_UART_CMD_LWL);
      flush();
  }else{
      flag = ioWaterLevelCalibration(_en, _test, CALIB_IO_TIME_LWL);
  }
  return flag;
}

//正在进行下水位校准（空水箱校准），请不要触碰检测区域
bool DFRobot_Nilometer::uartWaterLevelCalibration(int en, uint8_t cmd){
  int remain;
  uint8_t val;
  uCheckRslt_t rslt;
  if(en < 0){
      return false;
  }
  enableSensor(en);
  writeData(&cmd, 1);
  cmd = ((cmd >> 4) | (cmd << 4));
  delay(1000);
  remain = _s->available();
  for(int i = 0; i < remain; i++){
      val = _s->read();
      if(val == cmd){
          return true;
      }
  }
  //flush();
  return false;
}

bool DFRobot_Nilometer::ioWaterLevelCalibration(int en, int test, uint8_t t){
  if(en < 0 || test < 0 || _out < 0) return false;
  uint32_t t1 = 0,curT;
  uint8_t val, val1;
  int interT1 = 0, interT2 = 0;
  pinMode(test, OUTPUT);
  digitalWrite(test, HIGH);
  enableSensor(en);
  digitalWrite(test, LOW);
  delay(t);
  digitalWrite(test, HIGH);
  t1 = millis();
  val1 = digitalRead(_out);
  curT = t1;
  while(1){
      val = digitalRead(_out);
      if(val1 != val){
          if(val1){
              if(interT1 >= (t-10)){
                  interT2 = millis() - t1;
                  if(interT2 > 500) return true;
                  else {
                      interT1 = 0;
                      interT2 = 0;
                  }
              }
          }else{
              interT1 = millis() - t1;
          }
          t1 = millis();
          val1 =val;
          
      }
      delay(5);
      if((millis() - curT) > 1000){
          break;
      }
  }
  return false;
}

uint8_t DFRobot_Nilometer::getCalibrationMode(){
  if(_rslt.pad + _rslt.value == 0xFF){
      return _rslt.topt;
  }
  return 0xFF;
}

bool DFRobot_Nilometer::uartSelfCheck(int en){
  if(en < 0 || _s == NULL){
      DBG("en and _s error!")
      return false;
  } 
  uint8_t cmd = SELF_CHECK_CMD;
  int remain;
  uint8_t buf[2] = {0};
  enableSensor(en);
  writeData(&cmd, 1);
  delay(1000);
  remain = _s->available();
  if(remain >= 2){
      buf[0] = _s->read();
      DBG(buf[0],HEX);
      for(int i = 0; i < remain - 1; i++){
          buf[1] =  _s->read();
          if(buf[0]+buf[1] == 0xFF){
              memcpy(&_rslt, buf, 2);
              DBG(_rslt.outs);
              DBG(_rslt.value,HEX);
              return true;
          }
          buf[0]=buf[1];
      }
  }
  return false;
}

bool DFRobot_Nilometer::ioSelfCheck(int en, int test, Stream *s){
  if((en < 0) || (test < 0) ||(_out < 0) || s == NULL) return false;

  uint8_t buf[2] = {0};
  int remain;
  pinMode(test, OUTPUT);
  digitalWrite(test, HIGH);
  enableSensor(en);
  digitalWrite(test, LOW);
  delay(500);
  digitalWrite(test, HIGH);
  delay(550);
  delay(1000);
  
  remain = s->available();
  if(remain >= 2){
       buf[0] = s->read();
       remain -= 1;
       for(int i = 0; i < remain; i++){
           buf[1] =  s->read();
           delay(6);
           //Serial.print("buf0 = ");Serial.println(buf[0],HEX);
           //Serial.print("buf1 = ");Serial.println(buf[1],HEX);
           if((buf[0] != 0 || buf[0] != 0xff) && (buf[0]+buf[1] == 0xFF)){
              memcpy(&_rslt, buf, 2);
              return true;
           }
           buf[0]=buf[1];
       }
  }
  return false;
}

String DFRobot_Nilometer::getCalibModeDescription(uint8_t mode){
  String str = "";
  switch(mode){
      case CALIBRATION_MODE_LOWER_LEVEL:
        str = "Only calibrate the lower water level mode";
        break;
      case CALIBRATION_MODE_LOWER_AND_UPPER_LEVEL:
        str = "Up and down water level calibration mode";
        break;
      default:
        str = "Error calibration mode";
        break;
  }
  return str;
}

void DFRobot_Nilometer::writeData(void *data, uint8_t len){
  uint8_t *pBuf = (uint8_t *)data;
  _s->write(pBuf, len);
}

uint8_t DFRobot_Nilometer::readData(void *data, uint8_t len){
  uint8_t *pBuf = (uint8_t *)data;
  uint8_t size = 0;
  uint8_t remain = _s->available();
  uint8_t val = 0;
  while(remain > len){
      val = (uint8_t)_s->read();
      remain--;
  }
  for(int i = 0; i < len; i++){
      pBuf[size++] = _s->read();
  }
  DBG(size);
  return size;
}

uint8_t DFRobot_Nilometer::getCs(void *data, uint8_t len){
  uint8_t sum = 0;
  uint8_t *pBuf = (uint8_t *)data;
  for(uint8_t i = 0; i < len; i++){
      sum += pBuf[i];
  }
  sum = sum & 0xff;
  return sum;
}

void DFRobot_Nilometer::enableSensor(int en){
  if(en > -1){
      pinMode(en, OUTPUT);
      digitalWrite(en, LOW);
      delay(200);
      if(_s != NULL){
          while(_s->available()){
              _s->read();
          }
      }
      digitalWrite(en, HIGH);
      delay(1000);
  }
}
void DFRobot_Nilometer::flush(){
  if(_s != NULL){
      uint32_t  t= millis();
      while(_s->available()){
          _s->read();
      }
  }
}

uint8_t DFRobot_Nilometer::getSensitivity(){
  if(_rslt.pad + _rslt.value == 0xFF){
      return _rslt.sen;
  }
  return 0xFF;
}

bool DFRobot_Nilometer::checkCalibrationState(){
  uCheckRslt_t rslt;
  uint8_t val, val1;
  int waitForTimeOutMs = 1000;
  int waitForTimeoutIncMs = 100;
  uint32_t t = 0;
  if(_mode == eUARTDetecteMode){
      while((val1 = _s->read()) != ERR_CALIBRATION_CODE){
          delay(waitForTimeoutIncMs);
          t += waitForTimeoutIncMs;
          memcpy(&rslt, &val1, 1);
          if(rslt.value + rslt.pad == 0x0f) return true;
          if(t > waitForTimeOutMs){
              return true;
          }
      }
  }else{
      val = digitalRead(_out);
      while((val1 = digitalRead(_out)) == val){
          delay(waitForTimeoutIncMs);
          t += waitForTimeoutIncMs;
          if(t > waitForTimeOutMs){
              return true;
          } 
      }
      val = val1;
      for(int i = 0; i < 5; i++){
          delay(100);
          val1 = digitalRead(_out);
          if(val + val1 != 1){
              return true;
          }
          val = val1;
      }
  }
  return false;
}

DFRobot_SCW8916B_IO::DFRobot_SCW8916B_IO(int out, int en, int test, Stream *s)
  :DFRobot_Nilometer(out, en, test, s){}

DFRobot_SCW8916B_UART::DFRobot_SCW8916B_UART(Stream *s, int en)
  :DFRobot_Nilometer(s, en){}

bool DFRobot_SCW8916B_UART::setSensitivityLevel(eSensitivityLevel_t level){
  return setSensitivityLevel((uint8_t) level);
}

bool DFRobot_SCW8916B_UART::setSensitivityLevel(uint8_t level){
  uint8_t buf[6] = {0x43, (uint8_t)(level&0x07), 7,7,7};
  uint8_t state = 0x53;
  int waitForTimeOutMs = 2000;
  int waitForTimeoutIncMs = 100;
  int t = 0;
  uint8_t val;
  if(_mode == eUARTDetecteMode){
      buf[5] = getCs(buf+1, 4);
      enableSensor(_en);

      writeData(buf, sizeof(buf));
      while(1){
          val = _s->read();
          if(state == val){
              flush();
              return true;
          }
          delay(waitForTimeoutIncMs);
          t += waitForTimeoutIncMs;
          if(t > waitForTimeOutMs) return false;
      }
  }
}