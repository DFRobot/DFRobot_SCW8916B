# DFRobot_SCW8916B
Non-contact liquid level sensor library for Arduino.<br>
Features:<br>
* Space detection thickness: <= 2mm.<br>
* Support two detection modes: Level one-to-one detection and UART detection.<br>
* Support sensitivity level setting: 0~7.
* Support water level cailibration.<br>
note:如果传感器从未校准过，上电8s后，你需要先校准一次才能正常使用，从未校准或校准失败的现象：<br>
1.如果你使用的是串口检测，将会一直读到0xAA；<br>
2.如果你使用的是IO口检测，上电8s后，读到的电平将每隔100ms后取反一次；<br>
这里需要显示拍照图片，可以一张图片，可以多张图片（不要用SVG图）

![正反面svg效果图](https://github.com/Arya11111/DFRobot_MCP23017/blob/master/resources/images/SEN0245svg1.png)


## Product Link（链接到英文商城）
    
   
## Table of Contents

* [Summary](#summary)
* [Connected](#connected)
* [Installation](#installation)
* [Calibration](#calibration)
* [Methods](#methods)
* [Compatibility](#compatibility)
* [History](#history)
* [Credits](#credits)

## Summary
This is a Non-contact liquid level sensor library. It can only support IO and UART communication.<br>
It can support up to 2mm space detection. the sensor can be used to test whether there is water at a certain water level<br>

## Connected
Hardware conneted table<br>
### hardware conneted table in Level one-to-one detection mode
 Sensor      |              MCU               |
------------ | :----------------------------: |
TSET         | connected to the IO pin of MCU |
OUT          | connected to the IO pin of MCU |
EN           | connected to the IO pin of MCU |
VCC          |                5V              |
GND          |                GND             |
RX           | not connected, floating        |
TX           | connected to the IO pin of MCU |
### hardware conneted table in UART detection mode
 Sensor      |               MCU                 |
------------ | :-------------------------------: |
TSET         | not connected, floating           |
OUT          | not connected, floating           |
EN           |  connected to the IO pin of MCU   |
VCC          |                5V                 |
GND          |                GND                |
RX           |connected to the UART TX pin of MCU|
TX           |connected to the UART RX pin of MCU|

## Installation

To use this library, first download the library file, paste it into the \Arduino\libraries directory, then open the examples folder and run the demo in the folder.

## Calibration
此传感器只支持下水位校准，校准方法：<br>
* step1: 硬件连接，如果采用IO检测校准，需要使用传感器的OUT，TEST，EN和TX，如果采用UART检测，需要连接TX，RX和EN引脚;<br>
* step2: 自检，检测此传感器的校准模式：仅支持上水位校准(CALIBRATION_MODE_LOWER_LEVEL)，或支持上水位和下水位校准(CALIBRATION_MODE_LOWER_AND_UPPER_LEVEL)<br>
* step3: 获取校准模式，如果传感器是仅支持上水位校准，则继续校准，否则则不支持校准;<br>
* step4: 开始校准，校准期间需保持检水区域无水，且不要用手触摸检水区域;<br>
* step5: 校准成功后，需要调用checkCalibrationState检测校准是否完成。<br>
注意： 校准成功后触摸检水区域或有水将打印1，否则将打印0<br>

## Methods

```C++
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
```

## Compatibility

MCU                | SoftwareSerial | HardwareSerial |  IO   |
------------------ | :----------: | :----------: | :---------: |
Arduino Uno        |      √       |      X       |      √       |
Mega2560           |      √       |      √       |      √       |
Leonardo           |      √       |      √       |      √       |
ESP32              |      X       |      √       |      √       |
ESP8266            |      √       |      X       |      √       |
micro:bit          |      X       |      X       |      √       |
FireBeetle M0      |      X       |      √       |      √       |
raspberry          |      X       |      √       |      √       |

## History

- Data 2021-05-14
- Version V1.0

## Credits

Written by(xue.peng@dfrobot.com), 2021. (Welcome to our [website](https://www.dfrobot.com/))





