/*!
 * @file setAndGetSensitivity.ino
 * @brief This demo tells how to set and get sensitivity of Non-contact liquid level sensor.
 * @n Features:
 * @n 1. Support sensitivity level setting,ranging 0~7 or eSensitivityLevel0~eSensitivityLevel7.(only support UART detection mode)
 * @n 2. Get sensitivity level, in IO detecte mode, the sensitivity level shoudle be change by replace resistance. 
 * @n In UART detecte mode, can set sensitivity level by call software function setSensitivityLevel.
 *
 * @n note: This demo only support eUARTDetecteMode, not using eLevelDetecteMode. Not support microbit.
 *
 * @n connected table
 * @n --------------------------------------------------------------------------------------------------------------
 * @n sensor pin |             MCU                | Leonardo/Mega2560/M0 |    UNO    | ESP8266 | ESP32 |  microbit  |
 * @n     TEST   |   Not connected, floating(-1)  |               Not connected, floating(-1)          |     X      |
 * @n     OUT    |   Not connected, floating(-1)  |               Not connected, floating(-1)          |     X      |
 * @n     EN     | Connected to the IO pin of MCU |         2            |     2     |   D5    |  D9   |     X      |
 * @n     VCC    |            3.3V/5V             |        VCC           |    VCC    |   VCC   |  VCC  |     X      |
 * @n     GND    |              GND               |        GND           |    GND    |   GND   |  GND  |     X      |
 * @n     RX     | Connected to the TX pin of MCU |     Serial1 RX1      |     5     |   D6    |  D2   |     X      |
 * @n     TX     | Connected to the RX pin of MCU |     Serial1 TX1      |     4     |   D7    |  D3   |     X      |
 * @n ---------------------------------------------------------------------------------------------------------------
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

#define EN              2    /**<The IO pin of MCU which is connected to the EN pin of Non-contact liquid level sensor>*/

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
void setup() {
  Serial.begin(115200);
  while(!Serial){
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
  
  Serial.print("Set sensitivity level...");
/**
 * @brief Set sensitivity level of the channel of sensor.The higher the sensitivity level, the lower the sensitivity.Ranging 0~7.
 * @n note: This function only support eUARTDetecteMode, not using eLevelDetecteMode.
 * @param level:   the enum varible of eSensitivityLevel_t or 0~7
 * @n     eSensitivityLevel0 or 0: Sensitivity level 0, the most Sensitivity level.
 * @n     eSensitivityLevel1 or 1: Sensitivity level 1
 * @n     eSensitivityLevel2 or 2: Sensitivity level 2
 * @n     eSensitivityLevel3 or 3: Sensitivity level 3
 * @n     eSensitivityLevel4 or 4: Sensitivity level 4
 * @n     eSensitivityLevel5 or 5: Sensitivity level 5
 * @n     eSensitivityLevel6 or 6: Sensitivity level 6
 * @n     eSensitivityLevel7 or 7: Sensitivity level 7,the least Sensitivity Level.
 *
 * @return status: return config state.
 * @n      true:  Set sensitivity sucess.
 * @n      false:  Set sensitivity fail.
 */
  while(liquid.setSensitivityLevel(liquid.eSensitivityLevel2) != true){
      Serial.print(".");
      delay(2);
  }
  Serial.println("sucess.");
  
  Serial.print("Self check to update sensitivity...");
/**
 * @brief Self check which can update to get the current sensitivity and calibration mode.
 * @n In UART detected mode: You must use TX, RX and EN pin of sensor.
 * @n In level one-to-one detection mode: You must use OUT, TEST, EN, and TX pin of sensor
 * @return selfCheck update state:
 * @n      true: update sucess, and you can call getSensitivity to get current sensitivity ,and to call getCalibrationMode to get current calibration mode.
 * @n      false: update fail.
 */
  while(liquid.selfCheck() != true){
      Serial.print(".");
      delay(2);
  }
  Serial.println("done");
  Serial.print("Current sensitivity level(0~7): ");
/**
 * @brief Get sensitivity level of the channel of sensor.
 * @n @n note:Before using this function,you need to selfCheck function to update sensitivity ache.
 * @return sensitivity level: 
 * @n     eSensitivityLevel0 or 0: Sensitivity level 1, the most Sensitivity level.
 * @n     eSensitivityLevel1 or 1: Sensitivity level 2
 * @n     eSensitivityLevel2 or 2: Sensitivity level 3
 * @n     eSensitivityLevel3 or 3: Sensitivity level 4
 * @n     eSensitivityLevel4 or 4: Sensitivity level 5
 * @n     eSensitivityLevel5 or 5: Sensitivity level 6
 * @n     eSensitivityLevel6 or 6: Sensitivity level 6
 * @n     eSensitivityLevel7 or 7: Sensitivity level 7,the least Sensitivity Level.
 * @n     0xFF                   : Error sensitivity level value
 */
  Serial.println(liquid.getSensitivity());
}

void loop() {
}