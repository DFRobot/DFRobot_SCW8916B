/*!
 * @file detect.ino
 * @brief This demo is that detect whether there is water in a certain location by Non-contact liquid level sensor.
 * @n Experimental phenomena: If there is water at this level that will print digital 1 and let LED_BUILTIN of MCU on, 
 * @n or print digital 0 and let LED_BUILTIN of MCU off.
 * @n There are two methods to detect:
 * @n (1)Level one-to-one detection
 * @n (2) UART detection.
 *
 * @n connected table in eLevelDetecteMode
 * ---------------------------------------------------------------------------------------------------------------
 * sensor pin |             MCU                | Leonardo/Mega2560/M0 |    UNO    | ESP8266 | ESP32 |  microbit  |
 *     TEST   | Connected to the IO pin of MCU |               Not connected, floating(-1)                       |
 *     OUT    | Connected to the IO pin of MCU |          10          |     10    |  10/D3  | 5/D8 |     8/P8   |
 *     EN     |    Not connected, floating(-1) |               Not connected, floating(-1)                       |
 *     VCC    |            3.3V/5V             |        VCC           |    VCC    |   VCC   |  VCC  |    GND     |
 *     GND    |              GND               |        GND           |    GND    |   GND   |  GND  |    GND     |
 *     RX     |    Not connected, floating     |               Not connected, floating(-1)                       |
 *     TX     |    Not connected, floating     |               Not connected, floating(-1)                       |
 * ---------------------------------------------------------------------------------------------------------------
 *
 * @n connected table in eUARTDetecteMode(not support microbit)
 * ---------------------------------------------------------------------------------------------------------------
 * sensor pin |             MCU                | Leonardo/Mega2560/M0 |    UNO    | ESP8266 | ESP32 |  microbit  |
 *     TEST   |    Not connected, floating     |               Not connected, floating(-1)          |     X      |
 *     OUT    |    Not connected, floating     |               Not connected, floating(-1)          |     X      |
 *     EN     |    Not connected, floating(-1) |               Not connected, floating(-1)          |     X      |
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

#ifdef UART_COMMUNICATION
/**
 * @brief DFRobot_SCW8916B_UART abstract class constructor. Construct serial port detection object.(eUARTDetecteMode)
 * @param s:  The class pointer object of Abstract class， here you can fill in the pointer to the serial port object
 * @param en: The IO pin of MCU which is connected to the EN pin of Non-contact liquid level sensor.when you call selfCheck,
 * @n setSensitivityLevel, LowerWaterLevelCalibration,or UpperWaterLevelCalibration function, you must use EN pin.
 */
  #if defined(ARDUINO_AVR_UNO)||defined(ESP8266)
  SoftwareSerial mySerial(/*rx =*/4, /*tx =*/5);
  DFRobot_SCW8916B_UART liquid(/*s =*/&mySerial);
  #else
  DFRobot_SCW8916B_UART liquid(/*s =*/&Serial1);
  #endif
#else
  #if defined(ARDUINO_BBC_MICROBIT)
    #define OUT     8          /**<The IO pin of MCU which is connected to the OUT pin of Non-contact liquid level sensor>*/
  #elif defined(ESP32)
    #define OUT     5          /**<The IO pin of MCU which is connected to the OUT pin of Non-contact liquid level sensor>*/
  #else
    #define OUT     10          /**<The IO pin of MCU which is connected to the OUT pin of Non-contact liquid level sensor>*/
  #endif
/**
 * @brief DFRobot_SCW8916B_IO abstract class constructor.Construction level one-to-one detection object.(eLevelDetecteMode)
 * @param out  :  The IO pin of MCU which is connected to OUT pin of Non-contact liquid level sensor.
 * @param en   :  The IO pin of MCU which is connected to EN pin of Non-contact liquid level sensor. 
 * @param test :  The IO pin of MCU which is connected to TEST pin of Non-contact liquid level sensor. 
 * @param s    :  The class pointer object of Abstract class， here you can fill in the pointer to the serial port object.
 * @n note: If you call selfCheck, you must use en, test and s, and if you call lowerWaterLevelCalibration or UpperWaterLevelCalibration,
 * @n you must use en and test pin.
 */
 DFRobot_SCW8916B_IO liquid(/*out =*/OUT);
#endif

void setup() {
  Serial.begin(115200);
  while(!Serial){
  }

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
#ifdef UART_COMMUNICATION
  #if defined(ARDUINO_AVR_UNO)||defined(ESP8266)
    mySerial.begin(9600);
  #elif defined(ESP32)
    Serial1.begin(9600, SERIAL_8N1, /*rx =*/D3, /*tx =*/D2);
  #else
    Serial1.begin(9600);
  #endif
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
}

void loop() {
  bool flag = liquid.detectWater();                      /**<Detect whether there is water at water level------>true: have water, false: no water.*/
  if(flag){
      digitalWrite(LED_BUILTIN, HIGH);                   /**<If have water turn the LED on (HIGH is the voltage level)*/
      Serial.print("Have water: ");
  }else{
       digitalWrite(LED_BUILTIN, LOW);                   /**<If no water turn the LED off by making the voltage LOW*/
       Serial.print("No water:   ");
  }
  Serial.println(flag);                                  /**<Print water state at the water level, 0: No water, 1: Have water. */
}


