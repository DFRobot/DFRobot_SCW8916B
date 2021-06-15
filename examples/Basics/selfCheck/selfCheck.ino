/*!
 * @file selfCheck.ino
 * @brief The Non-contact liquid level sensor can self check to update sensitivity and cailibration mode.
 * @n There are two methods to detect:
 * @n (1)Level one-to-one detection
 * @n (2) UART detection.
 * @n Sensitivity level: 0~7(The higher the number, the lower the sensitivity)
 * @n Cailibration Mode:
 * @n  CALIBRATION_MODE_LOWER_LEVEL or 0            :   Only calibrate the lower water level
 * @n  CALIBRATION_MODE_LOWER_AND_UPPER_LEVEL or 1  :   Calibrate the upper and lower water levels.
 * @n  2~255                                        :   Error mode.
 * 
 * @n connected table in eLevelDetecteMode(not support microbit)
 * --------------------------------------------------------------------------------------------------
 * sensor pin |             MCU                | Leonardo/Mega2560/M0 |    UNO    | ESP8266 | ESP32 |
 *     TEST   | Connected to the IO pin of MCU |          13          |     13    |  13/D2  |  13/D7|
 *     OUT    | Connected to the IO pin of MCU |          10          |     10    |  15/D4  |  5/D8 |
 *     EN     | Connected to the IO pin of MCU |          2           |     2     |  2/D5   |  2/D9 |
 *     VCC    |            3.3V/5V             |        VCC           |    VCC    |   VCC   |  VCC  |
 *     GND    |              GND               |        GND           |    GND    |   GND   |  GND  |
 *     RX     |    Not connected, floating     |               Not connected, floating(-1)          |
 *     TX     |              RX                |     Serial1 RX1      |     4     |4/D7(RX) |  D3   |
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
 *     RX     |              TX                |     Serial1 RX1      |     5     |5/D6(TX) |  D2   |     X      |
 *     TX     |              RX                |     Serial1 TX1      |     4     |4/D7(RX) |  D3   |     X      |
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
  Serial.begin(115200);                                                                               //Serial USB Initialization
  while(!Serial){                                                                                     //Wait for USB serial port to connect. Needed for native USB port only
  }

#if defined(ARDUINO_AVR_UNO)||defined(ESP8266)
  mySerial.begin(9600);
#elif defined(ESP32)
  Serial1.begin(9600, SERIAL_8N1, /*rx =*/D3, /*tx =*/D2);
#else
  Serial1.begin(9600);
#endif

  Serial.print("Initialization sensor...");
  int error = 0;
/**
 * @brief liquide level sensor initialization.
 * @return initialization state:
 * @n      0:  sucess
 * @n      0xAA(170)：代表传感器从来没有校准过，需要先对传感器进行校准（UART）
 * @n      -1:  fail
 */
  while((error = liquid.begin()) != 0){
      Serial.print("failed. \nError code: ");
      Serial.println(error);
      if(error == ERR_CALIBRATION_CODE){
          Serial.println("You need to use calibration.ino to calibration sensor.");
      }else{
          Serial.println("Please check whether the hardware connection or configuration parameter is wrong.");
      }
      delay(1000);
      Serial.print("Initialization sensor...");
  }
  Serial.println("done.");

/**
 * @brief Self check which can update to get the current sensitivity and calibration mode.
 * @n In UART detected mode: You must use TX, RX and EN pin of sensor.
 * @n In level one-to-one detection mode: You must use OUT, TEST, EN, and TX pin of sensor
 * @return selfCheck update state:
 * @n      true: update sucess, and you can call getSensitivity to get current sensitivity ,and to call getCalibrationMode to get current calibration mode.
 * @n      false: update fail.
 */
  bool flag = liquid.selfCheck();
  if(flag){
      Serial.println("Self check sucess.");
      Serial.print("Current Sensitivity level(0~7): ");
      Serial.println(liquid.getSensitivity());

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
  }else{
      Serial.println("Self check failed.");
  }
  
}

void loop() {
  
}