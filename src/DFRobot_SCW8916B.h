/*!
 * @file DFRobot_SCW8916B.h
 * @brief 隔空水位检测传感器，可隔空2mm准确检测到水和其他液体的位置变化。
 * @n 检测模式：
 * @n 1. 电平检测模式（电平一对一检测，通过检测电平来判断有水没水， 1（高电平）：有水， 0（低电平）： 无水）
 * @n 2. UART检测模式（通过发送串口命令检测有水没水）；
 * @n compatibility table
 * --------------------------------------------------------------------------
 * | LIB          \ MCU | UNO | Leonardo | Mega2560 | ESP32 | M0 | microbit |
 * -------------------------------------------------------------------------
 * |HardwareSerial      |  X  |     √    |     √    |   √   | √  |    X     |
 * --------------------------------------------------------------------------
 * |SoftwareSerial      |  √  |     √    |     √    |   X   | X  |    X     |
 * --------------------------------------------------------------------------
 * √:support, X: not support
 *
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [Arya](xue.peng@dfrobot.com)
 * @version  V1.0
 * @date  2021-04-22
 * @https://github.com/DFRobot/DFRobot_SCW8916B
 */
#ifndef __DFRobot_SCW8916B_H
#define __DFRobot_SCW8916B_H

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include<Stream.h>

//Define DBG, change 0 to 1 open the DBG, 1 to 0 to close.  
#if 0
#define DBG(...) {Serial.print("["); Serial.print(__FUNCTION__); Serial.print("(): "); Serial.print(__LINE__); Serial.print(" ] "); Serial.println(__VA_ARGS__);}
#else
#define DBG(...)
#endif


#define CALIBRATION_MODE_LOWER_LEVEL   0/**<Only calibrate the lower water level*/
#define CALIBRATION_MODE_LOWER_AND_UPPER_LEVEL   1 /**<Calibrate the upper and lower water levels*/

class DFRobot_Nilometer{
private:
#define SELF_CHECK_CMD          0x34
#define CALIB_UART_CMD_LWL      0x25
#define CALIB_UART_CMD_UWL      0x8A
#define CALIB_IO_TIME_LWL       100   /**<unit: ms*/
#define CALIB_IO_TIME_UWL       200   /**<unit: ms*/
public:
#define ERR_CALIBRATION_CODE    0xAA
typedef enum{
  eLevelDetecteMode = 0,/**<level one-to-one detection mode*/
  eUARTDetecteMode/**<UART detecte mode.*/
}eDetecteMode_t;

typedef enum{
  eSensitivityLevel0 = 0, /**<sensitivity level 0 of sensor, the highest forest sensitivity level*/
  eSensitivityLevel1, /**<sensitivity level 1 of sensor*/
  eSensitivityLevel2, /**<sensitivity level 2 of sensor*/
  eSensitivityLevel3, /**<sensitivity level 3 of sensor*/
  eSensitivityLevel4, /**<sensitivity level 4 of sensor*/
  eSensitivityLevel5, /**<sensitivity level 5 of sensor*/
  eSensitivityLevel6, /**<sensitivity level 6 of sensor*/
  eSensitivityLevel7  /**<sensitivity level 7 of sensor, the lowest forest sensitivity level*/
}eSensitivityLevel_t;

typedef struct{
  union{
    struct{
      uint8_t outs:1; /**<detected mode: 0~eUARTDetecteMode,1~eLevelDetecteMode*/
      uint8_t topt:1; /**<calibration flag: 0~Only calibrate the lower water level, 1~Calibrate the upper and lower water levels*/
      uint8_t chan:3; /**<Channel numbers:1~4*/
      uint8_t sen: 3; /**<Channel 1's sensitivity*/
   };
    uint8_t value;
  };
  uint8_t pad; /**<The complement of the previous byte*/
}sSelfCheckRslt_t;

typedef union{
  struct{
     uint8_t ch1:1; /**<Water level status of channel 1, 1:have water, 0: no water*/
     uint8_t ch2:1; /**<Water level status of channel 2, 1:have water, 0: no water*/
     uint8_t ch3:1; /**<Water level status of channel 3, 1:have water, 0: no water*/
     uint8_t ch4:1; /**<Water level status of channel 4, 1:have water, 0: no water*/
     uint8_t pad:4;
  };
  uint8_t value:4; /**<One's complement of the first 4 bits*/
}uCheckRslt_t;
/**
 * @brief DFRobot_Nilometer abstract class constructor. Construct serial port detection object.(eUARTDetecteMode)
 * @param s:  The class pointer object of Abstract class， here you can fill in the pointer to the serial port object
 * @param en: The IO pin of MCU which is connected to the EN pin of Non-contact liquid level sensor.
 */
  DFRobot_Nilometer(Stream *s, int en);//eUARTDetecteMode
/**
 * @brief DFRobot_Nilometer abstract class constructor.Construction level one-to-one detection object.(eLevelDetecteMode)
 * @param out:  The IO pin of MCU which is connected to OUT pin of Non-contact liquid level sensor.
 * @param en: The IO pin of MCU which is connected to the EN pin of Non-contact liquid level sensor.
 * @param test :  The IO pin of MCU which is connected to TEST pin of Non-contact liquid level sensor. 
 * @param s:  The class pointer object of Abstract class， here you can fill in the pointer to the serial port object
 */
  DFRobot_Nilometer(int out, int en, int test, Stream *s);//eLevelDetecteMode
  ~DFRobot_Nilometer();
/**
 * @brief liquide level sensor initialization.
 * @return initialization state:
 * @n      0:  sucess
 * @n      0xAA(170)：代表传感器从来没有校准过，需要先对传感器进行校准（UART）
 * @n      -1:  fail
 */
  int begin();
/**
 * @brief Detect the presence or absence of water.
 * @return water state:
           true: There is water at this location.
           false: There is no water at this location.
 */
  bool detectWater();
/**
 * @brief Self check which can update to get the current sensitivity and calibration mode.
 * @n In UART detected mode: You must use TX, RX and EN pin of sensor.
 * @n In level one-to-one detection mode: You must use OUT, TEST, EN, and TX pin of sensor
 * @return selfCheck update state:
 * @n      true: update sucess, and you can call getSensitivity to get current sensitivity ,and to call getCalibrationMode to get current calibration mode.
 * @n      false: update fail.
 */
  bool selfCheck();
/**
 * @brief Water level calibration, note: Before calibration, it is necessary to ensure that there is no water 
 * @n in the water detection area, and do not touch the water detection area.
 * @n In level one-to-one detection mode: You must use EN and TEST pin of the sensor can successfully calibrated.
 * @n In UART detected mode: You must use EN pin of the sensor can successfully calibrated.
 * @return Calibration state:
 * @n      true: Calibration sucess.
 * @n      false: Calibration fail.
 */
  bool calibration();
  
/**
 * @brief Get calibration mode of sensor.
 * @n @n note:Before using this function,you need to selfCheck function to update sensitivity ache.
 * @return Calibration mode:
           0 or CALIBRATION_MODE_LOWER_LEVEL: Only calibrate the lower water level.
           1 or CALIBRATION_MODE_LOWER_AND_UPPER_LEVEL: Calibrate the upper and lower water level.
           0xFF:   error mode.
 */
  uint8_t getCalibrationMode();
/**
 * @brief Get calibration mode of sensor's description.
 * @param mode  calibration mode.
 * @n     CALIBRATION_MODE_LOWER_LEVEL or 0           :  only calibrate the lower water level.
 * @n     CALIBRATION_MODE_LOWER_AND_UPPER_LEVEL or 1 :  Up and down water level calibration.
 * @n     2~255                                       :  Error calibration mode
 * @return description: the string of calibration mode of sensor's description.
 */
  String getCalibModeDescription(uint8_t mode);
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
  uint8_t getSensitivity();
/**
 * @brief Clear recive buffer of UART, only use in UART deteceted mode.
 */
  void flush();
/**
 * @brief If you use calibration, you can call the function to detected that the calibration is completed.
 * @n Before using this function, you need to call LowerWaterLevelCalibration or LowerWaterLevelCalibration and UpperWaterLevelCalibration.
 * @return calibration state:
 * @n      true: calibration completed.
 * @n      false: calibration failed.
 */
  bool checkCalibrationState();
protected:
/**
 * @brief Water level calibration in UART Mode.
 * @param en  The IO pin of MCU which is connected to the EN pin of Non-contact liquid level sensor.
 * @param cmd  calibration command in UART Mode which is Used to distinguish between upper water level calibration and lower water level calibration.
 * @n     CALIB_UART_CMD_LWL: Lower water Level calibration command.
 * @n     CALIB_UART_CMD_UWL: Upper water Level calibration command.
 * @return Calibration state:
 * @n      true:  Calibration sucess.
 * @n      false:  Calibration failed.
 */
  bool uartWaterLevelCalibration(int en, uint8_t cmd);
/**
 * @brief Water level calibration in one-to-one detection mode.
 * @param en    The IO pin of MCU which is connected to the EN pin of Non-contact liquid level sensor.
 * @param test  The IO pin of MCU which is connected to the TEST pin of Non-contact liquid level sensor.
 * @param t     The Test pin's low signal delay time, unit: ms.
 * @n     CALIB_IO_TIME_LWL:  delay 100ms
 * @n     CALIB_IO_TIME_UWL:  delay 200ms
 * @return Calibration state:
 * @n      true:  Calibration sucess.
 * @n      false:  Calibration failed.
 */
  bool ioWaterLevelCalibration(int en, int test, uint8_t t);
  bool uartSelfCheck(int en);
  bool ioSelfCheck(int en, int test, Stream *s);
  uint8_t getCs(void *data, uint8_t len);
  void enableSensor(int en);
  void writeData(void *data, uint8_t len);
  uint8_t readData(void *data, uint8_t len);

  Stream *_s;
  int _out;
  int _test;
  int _en;
  eDetecteMode_t _mode;
  sSelfCheckRslt_t _rslt;
  
};

class DFRobot_SCW8916B_IO: public DFRobot_Nilometer{
public:
/**
 * @brief DFRobot_SCW8916B_IO abstract class constructor.Construction level one-to-one detection object.(eLevelDetecteMode)
 * @param out  :  The IO pin of MCU which is connected to OUT pin of Non-contact liquid level sensor.
 * @param en   :  The IO pin of MCU which is connected to EN pin of Non-contact liquid level sensor. 
 * @param test :  The IO pin of MCU which is connected to TEST pin of Non-contact liquid level sensor. 
 * @param s    :  The class pointer object of Abstract class， here you can fill in the pointer to the serial port object.
 * @n note: If you call selfCheck, you must use en, test and s, and if you call lowerWaterLevelCalibration or UpperWaterLevelCalibration,
 * @n you must use en and test pin.
 */
  DFRobot_SCW8916B_IO(int out, int en = -1, int test = -1, Stream *s = NULL);
};

class DFRobot_SCW8916B_UART: public DFRobot_Nilometer{
public:
/**
 * @brief DFRobot_SCW8916B_UART abstract class constructor. Construct serial port detection object.(eUARTDetecteMode)
 * @param s:  The class pointer object of Abstract class， here you can fill in the pointer to the serial port object
 * @param en: The IO pin of MCU which is connected to the EN pin of Non-contact liquid level sensor.when you call selfCheck,
 * @n setSensitivityLevel, LowerWaterLevelCalibration,or UpperWaterLevelCalibration function, you must use EN pin.
 */
  DFRobot_SCW8916B_UART(Stream *s, int en = -1);
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
  bool setSensitivityLevel(eSensitivityLevel_t level);
  bool setSensitivityLevel(uint8_t level);
};
#endif
