# DFRobot_SCW8916B.py
Non-contact liquid level sensor python driver library for Raspberry Pi.<br>
Support Python2 and python3
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
 Sensor      |         raspberry pi           |
------------ | :----------------------------: |
TSET         | connected to the IO pin of MCU |
OUT          | connected to the IO pin of MCU |
EN           | connected to the IO pin of MCU |
VCC          |                5V              |
GND          |                GND             |
RX           | not connected, floating        |
TX           | connected to the IO pin of MCU |
### hardware conneted table in UART detection mode
 Sensor      |           raspberry pi            |
------------ | :-------------------------------: |
TSET         | not connected, floating           |
OUT          | not connected, floating           |
EN           | connected to the IO pin of MCU    |
VCC          |                5V                 |
GND          |                GND                |
RX           |connected to the UART TX pin of MCU|
TX           |connected to the UART RX pin of MCU|

## Installation
To use this library, first download the library file, then open the examples folder and run the demo in the folder Proceed as follows:
* sudo git clone https://github.com/DFRobot/DFRobot_SCW8916B
* cd python
* cd raspberrypi
* cd examples
* python demo_*
* python3 demo_*

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
'''
  @brief DFRobot_SCW8916B_UART abstract class constructor. Construct serial port detection object.(UART detection mode.)
  @param en:  The IO pin of raspberry pi which is connected to the EN pin of Non-contact liquid level sensor. If you use self check and calibration fuction that you 
  @n must using EN pin of sensor to connected to raspberry pi.
'''
def __init__(self,en = -1)

'''
  @brief DFRobot_SCW8916B_IO abstract class constructor.Construction level one-to-one detection object.(Level one-to-one detection mode)
  @param out:  The IO pin of raspberry pi which is connected to OUT pin of Non-contact liquid level sensor.
  @param en:  The IO pin of raspberry pi which is connected to the EN pin of Non-contact liquid level sensor. If you use self check and calibration fuction that you 
  @n must using EN pin of sensor to connected to raspberry pi.
  @param test:  The IO pin of raspberry pi which is connected to TEST pin of Non-contact liquid level sensor.If you use self check and calibration fuction that you 
  @n must using TEST pin of sensor to connected to raspberry pi.
'''
def __init__(self,out, en = -1, test = -1)

'''
   @brief Set sensitivity level of the channel of sensor.The higher the sensitivity level, the lower the sensitivity.Ranging 0~7.
   @n note: This function only support eUARTDetecteMode, not using eLevelDetecteMode.
   @param level:   the enum varible of eSensitivityLevel_t or 0~7
   @n     eSensitivityLevel0 or 0: Sensitivity level 0, the most Sensitivity level.
   @n     eSensitivityLevel1 or 1: Sensitivity level 1
   @n     eSensitivityLevel2 or 2: Sensitivity level 2
   @n     eSensitivityLevel3 or 3: Sensitivity level 3
   @n     eSensitivityLevel4 or 4: Sensitivity level 4
   @n     eSensitivityLevel5 or 5: Sensitivity level 5
   @n     eSensitivityLevel6 or 6: Sensitivity level 6
   @n     eSensitivityLevel7 or 7: Sensitivity level 7,the least Sensitivity Level.
  
   @return status: return config state.
   @n      true:  Set sensitivity sucess.
   @n      false:  Set sensitivity fail.
'''
def set_sensitivity_level(self, level):

'''
  @brief liquide level sensor initialization.
  @return initialization state:
  @n      0:  sucess
  @n      0xAA(170)：It means that the sensor has never been calibrated, and the sensor needs to be calibrated first.
  @n      -1:  fail
'''
def begin(self):

'''
  @brief Detect the presence or absence of water.
  @return water state:
  @n      1: There is water at this location.
  @n      0: There is no water at this location.
'''
def detect_water(self):

'''
  @brief Get sensitivity level of the channel of sensor.
  @n note:Before using this function,you need to self_check function to update sensitivity ache.
  @return sensitivity level: 
  @n      0:  Sensitivity level 0
  @n     eSensitivityLevel0 or 0: Sensitivity level 1, the most Sensitivity level.
  @n     eSensitivityLevel1 or 1: Sensitivity level 2
  @n     eSensitivityLevel2 or 2: Sensitivity level 3
  @n     eSensitivityLevel3 or 3: Sensitivity level 4
  @n     eSensitivityLevel4 or 4: Sensitivity level 5
  @n     eSensitivityLevel5 or 5: Sensitivity level 6
  @n     eSensitivityLevel6 or 6: Sensitivity level 6
  @n     eSensitivityLevel7 or 7: Sensitivity level 7,the least Sensitivity Level.
  @n     0xFF                   : Error sensitivity level value
'''
def get_sensitivity_level(self)

'''
  @brief Get calibration mode of sensor.
  @n @n note:Before using this function,you need to self_heck function to update sensitivity ache.
  @return Calibration mode:
  @n      0 or CALIBRATION_MODE_LOWER_LEVEL: Only calibrate the lower water level.
  @n      1 or CALIBRATION_MODE_LOWER_AND_UPPER_LEVEL: Calibrate the upper and lower water level.
  @n      0xFF:   error mode.
'''
def get_calibration_mode(self):

'''
  @brief Get calibration mode of sensor's description.
  @param mode  calibration mode.
  @n     CALIBRATION_MODE_LOWER_LEVEL or 0           :  only calibrate the lower water level.
  @n     CALIBRATION_MODE_LOWER_AND_UPPER_LEVEL or 1 :  Up and down water level calibration.
  @n     2~255                                       :  Error calibration mode
  @return description: the string of calibration mode of sensor's description.
'''
def get_calib_mode_description(self,mode):
    
'''
  @brief Self check to update  current sensitivity and calibration mode ache.
  @note: In Level one-to-one detection mode: You must be connet EN/TSET/OUT and TX pin of sensor to raspberry pi's io pin and TX pin,for example:
  @n                                         EN:    The IO pin of raspberry pi which is connected to the EN pin of Non-contact liquid level sensor.
  @n                                         TEST:  The IO pin of raspberry pi which is connected to the TEST pin of Non-contact liquid level sensor.
  @n                                         OUT:   The IO pin of raspberry pi which is connected to the OUT pin of Non-contact liquid level sensor.
  @n                                         TX:    The UART RX pin of raspberry pi which is connected to the TX pin of Non-contact liquid level sensor.
  @n In UART detection mode: You must be connect EN/TX/RX pin of sensor to raspberry pi's io/RX/TX pin, for example:
  @n                         EN:    The IO pin of raspberry pi which is connected to the EN pin of Non-contact liquid level sensor.
  @n                         TX:    The UART RX pin of raspberry pi which is connected to the TX pin of Non-contact liquid level sensor.
  @n                         RX:    The UART TX pin of raspberry pi which is connected to the RX pin of Non-contact liquid level sensor.
  @param en     The IO pin of MCU which is connected to the EN pin of Non-contact liquid level sensor.
  @return selfCheck update state:
  @n      True: update sucess, and you can call getSensitivity to get current sensitivity ,and to call getCalibrationMode to get current calibration mode.
  @n      False: update fail.
'''
  def self_check(self):

'''
  @brief Clear recive buffer of UART, only use in UART deteceted mode.
'''
 def flush(self):

'''
  @brief Water level calibration, note: Before calibration, it is necessary to ensure that there is no water 
  @n in the water detection area, and do not touch the water detection area.
  @n In level one-to-one detection mode: You must use EN and TEST pin of the sensor can successfully calibrated.
  @n In UART detected mode: You must use EN pin of the sensor can successfully calibrated.
  @return Calibration state:
  @n      true: Calibration sucess.
  @n      false: Calibration fail.
'''
def calibration(self):

'''
  @brief If you use calibration, you can call the function to detected that the calibration is completed.
  @n Before using this function, you need to call LowerWaterLevelCalibration or LowerWaterLevelCalibration and UpperWaterLevelCalibration.
  @return calibration state:
  @n      true: calibration completed.
  @n      false: calibration failed.
'''
def check_calibration_state(self):

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

- Data 2021-05-16s
- Version V1.0

## Credits

Written by(xue.peng@dfrobot.com), 2021. (Welcome to our [website](https://www.dfrobot.com/))





