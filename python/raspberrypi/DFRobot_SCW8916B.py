# -*- coding:utf-8 -*-

'''
  @file DFRobot_SCW8916B.py
  @brief Define the basic structure of the DFRobot_SCW8916B class and the implementation of the basic methods.
  @n Non-contact liquid level sensor python driver library for Raspberry Pi.
  @n Features:
  @n 1. Space detection thickness: <= 2mm.
  @n 2. Support two detection modes: Level one-to-one detection and UART detection.
  @n 3. Support set and get sensitivity level: 0~7.
  @n 4. Support water level calibration.
  @
  @n Hardware conneted table in Level one-to-one detection mode
  @n -----------------------------------------------
  @n  Sensor      |         raspberry pi           |
  @n -----------------------------------------------
  @n TSET         | connected to the IO pin of MCU |
  @n OUT          | connected to the IO pin of MCU |
  @n EN           | connected to the IO pin of MCU |
  @n VCC          |                5V              |
  @n GND          |                GND             |
  @n RX           | not connected, floating        |
  @n TX           | connected to the IO pin of MCU |
  @n -----------------------------------------------
  @n Hardware conneted table in UART detection mode
  @n --------------------------------------------------
  @n  Sensor      |           raspberry pi            |
  @n --------------------------------------------------
  @n TSET         | not connected, floating           |
  @n OUT          | not connected, floating           |
  @n EN           | connected to the IO pin of MCU    |
  @n VCC          |                5V                 |
  @n GND          |                GND                |
  @n RX           |connected to the UART TX pin of MCU|
  @n TX           |connected to the UART RX pin of MCU|
  @n --------------------------------------------------
  @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
  @licence     The MIT License (MIT)
  @author      [Arya](xue.peng@dfrobot.com)
  @version  V1.0
  @date  2021-04-16
  @get from https://www.dfrobot.com
  @url https://github.com/DFRobot/DFRobot_SCT80S16B
'''
import sys
import serial
import time
import RPi.GPIO as GPIO

ERR_CALIBRATION_CODE = 0xAA

CALIBRATION_MODE_LOWER_LEVEL = 0
CALIBRATION_MODE_LOWER_AND_UPPER_LEVEL = 1
class DFRobot_Nilometer:
  '''Enum constant'''
  eSENSITIVITY_LEVEL0 = 0
  eSENSITIVITY_LEVEL1 = 1
  eSENSITIVITY_LEVEL2 = 2
  eSENSITIVITY_LEVEL3 = 3
  eSENSITIVITY_LEVEL4 = 4
  eSENSITIVITY_LEVEL5 = 5
  eSENSITIVITY_LEVEL6 = 6
  eSENSITIVITY_LEVEL7 = 7

  eLEVEL_DETECT_MODE = 0
  eUART_DETECT_MODE = 1
  
  SELF_CHECK_CMD     =   0x34
  CALIB_UART_CMD_LWL =   0x25
  CALIB_UART_CMD_UWL =   0x8A
  CALIB_IO_TIME_LWL  =   0.1
  CALIB_IO_TIME_UWL  =   0.2
  
  _out = -1
  _mode = 0
  _en  = -1
  _test = -1
  _rslt_h = 0
  _rslt_l = 0
  _ser = -1

  def __init__(self):
    GPIO.setmode(GPIO.BCM)
    GPIO.setwarnings(False)

  def begin(self):
    '''
      @brief liquide level sensor initialization.
      @return initialization state:
      @n      0:  sucess
      @n      0xAA(170)：It means that the sensor has never been calibrated, and the sensor needs to be calibrated first.
      @n      -1:  fail
    '''
    val = 0
    val1 = 0
    wait_for_timeout_s = 8
    wait_for_timeout_inc_s = 0.1
    count = 0
    t = 0
    self._enable_sensor(self._en)
    #time.sleep(1)
    if self._mode == self.eUART_DETECT_MODE:
      if self._ser.isOpen == False:
        print("serial is not open")
        return -1
      val1 = self._read_byte()
      while val1 != ERR_CALIBRATION_CODE:
        time.sleep(wait_for_timeout_inc_s)
        t += wait_for_timeout_inc_s
        #print("val1=%#x"%val1)
        #print(type(val1))
        val_h = (val1 >> 4) & 0x0F
        val_l = val1 & 0x0F
        if(val_h + val_l) == 0x0F:
          #print("ok1")
          return 0
        if(t > wait_for_timeout_s):
          #print("ok")
          return 0
        val1 = self._read_byte()
    else:
      if self._out == -1:
        print("Error: _out = -1.")
        return -1
      GPIO.setup(self._out, GPIO.IN)
      val = GPIO.input(self._out)
      val1 = GPIO.input(self._out)
      while val1 == val:
        time.sleep(wait_for_timeout_inc_s)
        t += wait_for_timeout_inc_s
        if(t > wait_for_timeout_s):
          
          return 0
        val1 = GPIO.input(self._out)
      val = val1
      for i in range(5):
        time.sleep(wait_for_timeout_inc_s)
        val1 = GPIO.input(self._out)
        if (val + val1) == 1:
          count = count + 1
        val = val1
        if count < 2:
          return 0
    return ERR_CALIBRATION_CODE
    
  def detect_water(self):
    '''
      @brief Detect the presence or absence of water.
      @return water state:
      @n      1: There is water at this location.
      @n      0: There is no water at this location.
    '''
    flag = 0
    val_h = 0
    val_l = 0
    if self._mode == self.eUART_DETECT_MODE:
      val = self._read_byte()
      #print("val=%#X"%val)
      val_h = (val >> 4) & 0x0f
      val_l = val & 0x0f
      if (val_h + val_l) == 0x0f:
        flag = val & 0x01
    else:
      flag = GPIO.input(self._out)
    time.sleep(0.1)
    return flag

  def get_sensitivity_level(self):
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
    if self._rslt_h +  self._rslt_l == 0xFF:
      return ((self._rslt_l >> 5) & 0x07)
    print("%#X = %#X"%(self._rslt_h, self._rslt_l))
    return 0xff
  
  def get_calibration_mode(self):
    '''
      @brief Get calibration mode of sensor.
      @n @n note:Before using this function,you need to self_heck function to update sensitivity ache.
      @return Calibration mode:
      @n      0 or CALIBRATION_MODE_LOWER_LEVEL: Only calibrate the lower water level.
      @n      1 or CALIBRATION_MODE_LOWER_AND_UPPER_LEVEL: Calibrate the upper and lower water level.
      @n      0xFF:   error mode.
    '''
    if self._rslt_h +  self._rslt_l == 0xFF:
      return ((self._rslt_l >> 1) & 0x01)
    return 0xff
  
  def get_calib_mode_description(self,mode):
    '''
      @brief Get calibration mode of sensor's description.
      @param mode  calibration mode.
      @n     CALIBRATION_MODE_LOWER_LEVEL or 0           :  only calibrate the lower water level.
      @n     CALIBRATION_MODE_LOWER_AND_UPPER_LEVEL or 1 :  Up and down water level calibration.
      @n     2~255                                       :  Error calibration mode
      @return description: the string of calibration mode of sensor's description.
    '''
    str = ""
    if mode == CALIBRATION_MODE_LOWER_LEVEL:
      str = "Only calibrate the lower water level mode"
    elif mode == CALIBRATION_MODE_LOWER_AND_UPPER_LEVEL:
      str = "Up and down water level calibration mode"
    else:
      str = "Error calibration mode"
    return str

  def self_check(self):
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
    if self._mode == self.eUART_DETECT_MODE:
      return self._uart_self_check(self._en)
    else:
      return self._io_self_check(self._en,self._test)
  
  def flush(self):
    '''
      @brief Clear recive buffer of UART, only use in UART deteceted mode.
    '''
    if self._mode == self.eUART_DETECT_MODE:
      timenow = time.time()
      count = self._ser.inWaiting()
      if count != 0:
        self._ser.read(count)
  
  def calibration(self):
    '''
      @brief Water level calibration, note: Before calibration, it is necessary to ensure that there is no water 
      @n in the water detection area, and do not touch the water detection area.
      @n In level one-to-one detection mode: You must use EN and TEST pin of the sensor can successfully calibrated.
      @n In UART detected mode: You must use EN pin of the sensor can successfully calibrated.
      @return Calibration state:
      @n      true: Calibration sucess.
      @n      false: Calibration fail.
    '''
    if self._mode == self.eUART_DETECT_MODE:
      return self._uart_water_level_calibration(self._en, self.CALIB_UART_CMD_LWL)
    else:
      return self._io_water_level_cailibration(self._en,self._test, self.CALIB_IO_TIME_LWL)
      
  def check_calibration_state(self):
    '''
      @brief If you use calibration, you can call the function to detected that the calibration is completed.
      @n Before using this function, you need to call LowerWaterLevelCalibration or LowerWaterLevelCalibration and UpperWaterLevelCalibration.
      @return calibration state:
      @n      true: calibration completed.
      @n      false: calibration failed.
    '''
    val_h = 0
    val_l = 0
    wait_for_timeout_s = 1
    wait_for_timeout_inc_s = 0.1
    
    t = 0
    if self._mode == self.eUART_DETECT_MODE:
      remain = self._ser.inWaiting()
      #print("remian=%d"%remain)
      if remain:
        val1 = ord(self._ser.read(1))
        while val1 != ERR_CALIBRATION_CODE:
          time.sleep(wait_for_timeout_inc_s)
          t = t + wait_for_timeout_inc_s
          val_h = (val1 >> 4) & 0x0F
          val_l = val1 & 0x0F
          #print("val1=%#x, %#X, %#x"%(val1,val_h, val_l))
          if val_h + val_l == 0x0f:
            self.flush()
            return True
          if t > wait_for_timeout_s:
            self.flush()
            return True
          remain = self._ser.inWaiting()
          #print("remian=%d"%remain)
          if remain:
            val1 = ord(self._ser.read(1))
    else:
      val = GPIO.input(self._out)
      val1 = GPIO.input(self._out)
      while val == val1:
        time.sleep(wait_for_timeout_inc_s)
        t = t + wait_for_timeout_inc_s
        if t > wait_for_timeout_s:
          return True
        val1 = GPIO.input(self._out)
      val = val1
      count = 5
      while count:
        time.sleep(wait_for_timeout_inc_s)
        val1 = GPIO.input(self._out)
        if val + val1 != 1:
          return True
        val = val1
        count = count - 1
      return False

  def _uart_self_check(self, en):
    if en < 0 or self._ser.isOpen == False:
      return False
    remain = 0
    buf = [0,0]
    self._enable_sensor(en)
    self._write_data([self.SELF_CHECK_CMD])
    time.sleep(1)
    remain = self._ser.inWaiting()
    #print("remain=%d"%remain)
    #return True
    
    if remain >= 2:
      buf[0] = ord(self._ser.read(1))
      remain -= 1
      while remain > 0:
        buf[1] = ord(self._ser.read(1))
        #print("buf[1] = %#X, buf[0]=%d"%buf[1])
        remain = remain -1
        if buf[0] + buf[1] == 0xFF:
          self._rslt_h = buf[1]
          self._rslt_l = buf[0]
          return True
        buf[0] = buf[1]
    return False
    
  def _uart_water_level_calibration(self, en, cmd):
    remain = 0
    val = 0
    if(en < 0):
      return False
    self._enable_sensor(en)
    self._write_data([cmd])
    cmd = ((cmd >> 4) | (cmd << 4)) & 0xFF
    #print("cmd=%#x"%cmd)
    time.sleep(1)
    remain = self._ser.inWaiting()
    #print("remain=%d"%remain)
    while remain:
      val = ord(self._ser.read(1))
      #print("val=%#x"%val)
      if val == cmd:
        return True
      remain = remain - 1
    return False
    
  def _io_water_level_cailibration(self, en, test, t):
    if en < 0 or test < 0 or self._out < 0:
      return False
    t1 = 0
    curT = 0
    interT1 = 0
    interT2 = 0
    GPIO.setup(test, GPIO.OUT)
    GPIO.output(test, GPIO.HIGH)
    self._enable_sensor(en)
    GPIO.output(test, GPIO.LOW)
    time.sleep(t)
    GPIO.output(test, GPIO.HIGH)
    t1 = time.time()
    val1 = GPIO.input(self._out)
    val = 0
    curT = t1
    while True:
      val = GPIO.input(self._out)
      if val != val1:
        if val1:
          if interT1 >= t - 0.01:
            interT2 = time.time() - t1
            if interT2 > 0.5:
              return True
            else:
              interT1 = 0
              interT2 = 0
        else:
          interT1 = time.time() - t1
        t1= time.time()
        val1 = val
        time.sleep(0.005)
        if time.time() - curT > 1000:
          break
    return False
  
  def _io_self_check(self, en, test):
    ser = serial.Serial("/dev/ttyAMA0",9600)
    if en < 0 or test < 0 or self._out < 0 or ser.isOpen == False:
      return False
    buf = [0,0]
    GPIO.setup(test, GPIO.OUT)
    GPIO.output(test, GPIO.HIGH)
    self._enable_sensor(en)
    GPIO.output(test, GPIO.LOW)
    time.sleep(0.5)
    GPIO.output(test, GPIO.HIGH)
    time.sleep(0.55)
    time.sleep(1)
    remain = ser.inWaiting()
    #print("remain=%d"%remain)
    if remain >= 2:
      buf[0] = ord(ser.read(1))
      remain = remain - 1
      while remain > 0:
        buf[1] = ord(ser.read(1))
        remain = remain -1
        if buf[0] + buf[1] == 0xff:
          self._rslt_h = buf[1]
          self._rslt_l = buf[0]
          return True
        buf[0] =buf[1]
    return False

  def _write_data(self, data):
    self._ser.write(data)

  def _read_byte(self):
    data = 0
    timenow = time.time()
    while (time.time() - timenow) <= 1:
      count = self._ser.inWaiting()
      #print("count = %d"%count)
      if count != 0:
        l = self._ser.read(count)
        
        try: 
          data = ord(l[count - 1])
        except:
          data = l[count - 1]
        break
    return data

  def _get_cs(self, data):
    cs = 0
    for i in data:
      cs += i
      #print(i)
    cs = cs & 0xff
    return cs
    
  def _enable_sensor(self, en):
    if(en > -1):
      GPIO.setup(en, GPIO.OUT)
      GPIO.output(en, GPIO.LOW)
      self.flush()
      time.sleep(0.2)
      GPIO.output(en, GPIO.HIGH)
      time.sleep(1)

class DFRobot_SCW8916B_UART(DFRobot_Nilometer):
  def __init__(self,en = -1):
    '''
      @brief DFRobot_SCW8916B_UART abstract class constructor. Construct serial port detection object.(UART detection mode.)
      @param en:  The IO pin of raspberry pi which is connected to the EN pin of Non-contact liquid level sensor. If you use self check and calibration fuction that you 
      @n must using EN pin of sensor to connected to raspberry pi.
    '''
    self._mode = self.eUART_DETECT_MODE
    self._ser = serial.Serial("/dev/ttyAMA0",9600)
    self._en = en
    DFRobot_Nilometer.__init__(self)

  def set_sensitivity_level(self, level):
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
    buf = [0x43, level & 0x07, 7, 7, 7,0]
    state = 0x53
    wait_for_timeout_s = 2
    wait_for_timeout_inc_s = 0.1
    t = 0
    count = 0
    
    buf[5] = self._get_cs(buf[1:5])
    self._enable_sensor(self._en)
    self._write_data(buf)
    #print(buf)

    while True:
      remain = self._ser.inWaiting()
      #print("remain = %d"%remain)
      if remain:
        val = ord(self._ser.read(1))
        #print("val=%#X"%val)
        if state == val:
          self.flush()
          return True
      time.sleep(wait_for_timeout_inc_s)
      t = t + wait_for_timeout_inc_s
      if t > wait_for_timeout_s:
        return False
    


class DFRobot_SCW8916B_IO(DFRobot_Nilometer):
  def __init__(self,out, en = -1, test = -1):
    '''
      @brief DFRobot_SCW8916B_IO abstract class constructor.Construction level one-to-one detection object.(Level one-to-one detection mode)
      @param out:  The IO pin of raspberry pi which is connected to OUT pin of Non-contact liquid level sensor.
      @param en:  The IO pin of raspberry pi which is connected to the EN pin of Non-contact liquid level sensor. If you use self check and calibration fuction that you 
      @n must using EN pin of sensor to connected to raspberry pi.
      @param test:  The IO pin of raspberry pi which is connected to TEST pin of Non-contact liquid level sensor.If you use self check and calibration fuction that you 
      @n must using TEST pin of sensor to connected to raspberry pi.
    '''
    self._out = out
    self._en = en
    self._test = test
    self._mode = self.eLEVEL_DETECT_MODE
    DFRobot_Nilometer.__init__(self)


