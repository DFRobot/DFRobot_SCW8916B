/*!
 * @file calibration.ino
 * @brief 这个demo提供了水位传感器校准功能，如果水位传感器不准或从未校准，可以运行此demo来校准传感器以获得正确
 * @n 水位检测状态。
 * @n Experimental phenomena: If there is water at this level that will print digital 1  or print digital 0
 * @n There are two methods to detect:
 * @n (1)Level one-to-one detection
 * @n (2) UART detection.
 * @n There two calibration mode:
 * @n (1) CALIBRATION_MODE_LOWER_LEVEL or 0            :   Only calibrate the lower water level（本传感器支持此种模式）
 * @n (2) CALIBRATION_MODE_LOWER_AND_UPPER_LEVEL or 1  :   Calibrate the upper and lower water levels.
 * @n (3) 2~255                                        :   Error mode.
 * @n 校准方法：
 * @n step1: 硬件连接，如果使用IO口校准，需要正确连接传感器的OUT，EN，TEST，和TX引脚，如果使用UART校准，需正确连接TX，RX和EN等引脚;
 * @n step2: 自检获取校准模式是否正确,延时2秒，在2秒期间保持检水区域无水，且不要触摸检水区域;
 * @n step3: 开始校准(校准期间保持检水区域无水，且不要触摸检水区域)
 * @n step4: 如果校准成功，则调用checkCalibrationState检测校准是否完成。
 *
 * @n connected table in eLevelDetecteMode(not support microbit)
 * --------------------------------------------------------------------------------------------------
 * sensor pin |             MCU                | Leonardo/Mega2560/M0 |    UNO    | ESP8266 | ESP32 |
 *     TEST   | Connected to the IO pin of MCU |          13          |     13    |  13/D2  |  13/D7|
 *     OUT    | Connected to the IO pin of MCU |          10          |     10    |  10/D3  |  5/D8 |
 *     EN     | Connected to the IO pin of MCU |          2           |     2     |  2/D5   |  2/D9 |
 *     VCC    |            3.3V/5V             |        VCC           |    VCC    |   VCC   |  VCC  |
 *     GND    |              GND               |        GND           |    GND    |   GND   |  GND  |
 *     RX     |    Not connected, floating     |               Not connected, floating(-1)          |
 *     TX     |              RX                |     Serial1 RX1      |     4     |   4/D6  |  D3   |
 * --------------------------------------------------------------------------------------------------
 *
 * @n connected table in eUARTDetecteMode(not support microbit)
 * ---------------------------------------------------------------------------------------------------------------
 * sensor pin |             MCU                | Leonardo/Mega2560/M0 |    UNO    | ESP8266 | ESP32 |  microbit  |
 *     TEST   |    Not connected, floating     |               Not connected, floating(-1)          |     X      |
 *     OUT    |    Not connected, floating     |               Not connected, floating(-1)          |     X      |
 *     EN     |    Not connected, floating(-1) |          2           |     2     |  2/D5   |  2/D9 |
 *     VCC    |            3.3V/5V             |        VCC           |    VCC    |   VCC   |  VCC  |     X      |
 *     GND    |              GND               |        GND           |    GND    |   GND   |  GND  |     X      |
 *     RX     |              TX                |     Serial1 RX1      |     5     |   5/D6  |  D2   |     X      |
 *     TX     |              RX                |     Serial1 TX1      |     4     |   4/D7  |  D3   |     X      |
 * ---------------------------------------------------------------------------------------------------------------
 *
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [Arya](xue.peng@dfrobot.com)
 * @version  V1.0
 * @data  2021-05-14
 * @get from https://www.dfrobot.com
 * @url https://github.com/DFRobot/DFRobot_SCW8916B
 */
#include "DFRobot_SCW8916B.h"
#if defined(ARDUINO_AVR_UNO)||defined(ESP8266)
#include <SoftwareSerial.h>
#endif

#define STEP1_START_CALIB       1
#define STEP2_END_CALIB         2

//默认打开，此时使用UART通信， 屏蔽后使用IO口通信
#define UART_COMMUNICATION

#define EN     2      /**<The IO pin of MCU which is connected to the EN pin of Non-contact liquid level sensor>*/

#ifdef UART_COMMUNICATION
/**
 * @brief DFRobot_SCW8916B_UART abstract class constructor. Construct serial port detection object.(eUARTDetecteMode)
 * @param s:  The class pointer object of Abstract class， here you can fill in the pointer to the serial port object
 * @param en: The IO pin of MCU which is connected to the EN pin of Non-contact liquid level sensor.when you call selfCheck,
 * @n setSensitivityLevel, LowerWaterLevelCalibration,or UpperWaterLevelCalibration function, you must use EN pin.
 */
  #if defined(ARDUINO_AVR_UNO)||defined(ESP8266)
  SoftwareSerial mySerial(/*rx =*/4, /*tx =*/5);
  DFRobot_SCW8916B_UART liquid(/*s =*/&mySerial, /*en =*/EN);
  #else
  DFRobot_SCW8916B_UART liquid(/*s =*/&Serial1, /*en =*/EN);
  #endif
#else
  #if defined(ESP32)
  #define OUT     5          /**<The IO pin of MCU which is connected to the OUT pin of Non-contact liquid level sensor>*/
  #else
  #define OUT     10          /**<The IO pin of MCU which is connected to the OUT pin of Non-contact liquid level sensor>*/
  #endif
  #define TEST    13          /**<The IO pin of MCU which is connected to the TEST pin of Non-contact liquid level sensor>*/
/**
 * @brief DFRobot_SCW8916B_IO abstract class constructor.Construction level one-to-one detection object.(eLevelDetecteMode)
 * @param out  :  The IO pin of MCU which is connected to OUT pin of Non-contact liquid level sensor.
 * @param en   :  The IO pin of MCU which is connected to EN pin of Non-contact liquid level sensor. 
 * @param test :  The IO pin of MCU which is connected to TEST pin of Non-contact liquid level sensor. 
 * @param s    :  The class pointer object of Abstract class， here you can fill in the pointer to the serial port object.
 * @n note: If you call selfCheck, you must use en, test and s, and if you call lowerWaterLevelCalibration or UpperWaterLevelCalibration,
 * @n you must use en and test pin.
 */
  #if defined(ARDUINO_AVR_UNO)||defined(ESP8266)
  SoftwareSerial mySerial(/*rx =*/4, /*tx =*/5);
  DFRobot_SCW8916B_IO liquid(/*out =*/OUT, /*en =*/EN, /*test =*/TEST, /*s =*/&mySerial);
  #else
  DFRobot_SCW8916B_IO liquid(/*out =*/OUT, /*en =*/EN, /*test =*/TEST, /*s =*/&Serial1);
  #endif
#endif

void setup() {
  Serial.begin(115200);
  while(!Serial){                                                     //Waiting for USB Serial COM port to open.
  }
  
#if defined(ARDUINO_AVR_UNO)||defined(ESP8266)
  mySerial.begin(9600);
#elif defined(ESP32)
  Serial1.begin(9600, SERIAL_8N1, /*rx =*/D3, /*tx =*/D2);
#else
  Serial1.begin(9600);
#endif
/**
 * @brief liquide level sensor initialization.
 * @return initialization state:
 * @n      0:  sucess
 * @n      0xAA(170)：代表传感器从来没有校准过，需要先对传感器进行校准（UART）
 * @n      -1:  fail
 */
  int errCode = liquid.begin();
  Serial.print("Error code: ");
  Serial.println(errCode);
/**
 * @brief Self check which can update to get the current sensitivity and calibration mode.
 * @n In UART detected mode: You must use TX, RX and EN pin of sensor.
 * @n In level one-to-one detection mode: You must use OUT, TEST, EN, and TX pin of sensor
 * @return selfCheck update state:
 * @n      true: update sucess, and you can call getSensitivity to get current sensitivity ,and to call getCalibrationMode to get current calibration mode.
 * @n      false: update fail.
 */
  bool selfFlag = liquid.selfCheck();
  if(selfFlag){
      Serial.println("Self check sucess.");
      Serial.print("Cailibration mode: ");
      /**
       * @brief Get calibration mode of sensor.
       * @n @n note:Before using this function,you need to selfCheck function to update sensitivity ache.
       * @return Calibration mode:
                 0 or CALIBRATION_MODE_LOWER_LEVEL: Only calibrate the lower water level.
                 1 or CALIBRATION_MODE_LOWER_AND_UPPER_LEVEL: Calibrate the upper and lower water level.
                 0xFF:   error mode.
       */
      uint8_t mode = liquid.getCalibrationMode();
      Serial.println(mode);
      Serial.print("Cailibration mode description: ");
      /**
       * @brief Get calibration mode of sensor's description.
       * @param mode  calibration mode.
       * @n     CALIBRATION_MODE_LOWER_LEVEL or 0           :  only calibrate the lower water level.
       * @n     CALIBRATION_MODE_LOWER_AND_UPPER_LEVEL or 1 :  Up and down water level calibration.
       * @n     2~255                                       :  Error calibration mode
       * @return description: the string of calibration mode of sensor's description.
       */
      Serial.println(liquid.getCalibModeDescription(mode));
      if (mode == CALIBRATION_MODE_LOWER_LEVEL){
          bool calibFlag = false;
          uint8_t calibStep = STEP1_START_CALIB;
          Serial.println("Keep the water inspection area free of water and do not touch the water inspection area in 2s");
          delay(2000);          /**<延时2秒，校准前准备，请保证在校准期间，保持检水区域无水，且不要触摸检水区域*/
          Serial.println("Starting calibration...");
          while(1){
              if(calibStep == STEP1_START_CALIB){
                 /**
                  * @brief Water level calibration, note: Before calibration, it is necessary to ensure that there is no water 
                  * @n in the water detection area, and do not touch the water detection area.
                  * @n In level one-to-one detection mode: You must use EN and TEST pin of the sensor can successfully calibrated.
                  * @n In UART detected mode: You must use EN pin of the sensor can successfully calibrated.
                  * @return Calibration state:
                  * @n      true: Calibration sucess.
                  * @n      false: Calibration fail.
                  */
                  calibFlag = liquid.calibration();
                  if(!calibFlag){
                      Serial.println("Calibration failed. \n");
                      continue;
                  }
                  calibStep = STEP2_END_CALIB;
              }
              if(calibStep == STEP2_END_CALIB){
                  /**
                   * @brief If you use calibration, you can call the function to detected that the calibration is completed.
                   * @n Before using this function, you need to call LowerWaterLevelCalibration or LowerWaterLevelCalibration and UpperWaterLevelCalibration.
                   * @return calibration state:
                   * @n      true: calibration completed.
                   * @n      false: calibration failed.
                   */
                  calibFlag = liquid.checkCalibrationState();
                  if(!calibFlag){
                      Serial.println("Calibration failed.");
                      Serial.println("Keep the water inspection area free of water and do not touch the water inspection area in 2s");
                      delay(2000);          /**<延时2秒，校准前准备，请保证在校准期间，保持检水区域无水，且不要触摸检水区域*/
                      Serial.println("Starting calibration...");
                      calibStep = STEP1_START_CALIB;
                      continue;
                  }
                  break;
              }
          }
          Serial.println("End calibration.");
      }else{
          Serial.println("Does not support this calibration mode");
      }
  }else{
      Serial.println("Self check failed");
  }
  
}

void loop() {
  bool flag = liquid.detectWater();      /**<Detect whether there is water at water level------>true: have water, false: no water.*/
  if(flag){
       Serial.print("Have water: ");
  }else{
       Serial.print("No water:   ");
  }
  Serial.println(flag);                 /**<Print water state at the water level, 0: No water, 1: Have water. */
}


