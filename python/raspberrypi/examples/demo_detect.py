from __future__ import print_function
# -*- coding:utf-8 -*-

'''
  # demo_detect.py
  #
  # This demo is that detect whether there is water in a certain location by Non-contact liquid level sensor.
  # Experimental phenomena: If there is water at this level that will print digital 1, or print digital 0.
  # There are two methods to detect:
  # (1)Level one-to-one detection
  # (2) UART detection.
  #
  # @n connected table in eLEVEL_DETECT_MODE
  # -----------------------------------------------------------------------------
  # sensor pin |             MCU                |         raspberry pi          |
  #     TEST   | Connected to the IO pin of MCU |  Not connected, floating(-1)  |
  #     OUT    | Connected to the IO pin of MCU |           22(BCM)             |
  #     EN     |    Not connected, floating(-1) |  Not connected, floating(-1)  |
  #     VCC    |            3.3V/5V             |            5V/3V3             |
  #     GND    |              GND               |             GND               |
  #     RX     |    Not connected, floating     |  Not connected, floating(-1)  |
  #     TX     |    Not connected, floating     |  Not connected, floating(-1)  |
  # -----------------------------------------------------------------------------
  #
  # @n connected table in eUART_DETECT_MODE
  # -----------------------------------------------------------------------------
  # sensor pin |             MCU                |         raspberry pi          |
  #     TEST   |    Not connected, floating     |  Not connected, floating(-1)  |
  #     OUT    |    Not connected, floating     |  Not connected, floating(-1)  |
  #     EN     |    Not connected, floating(-1) |  Not connected, floating(-1)  |
  #     VCC    |            3.3V/5V             |            5V/3V3             |
  #     GND    |              GND               |             GND               |
  #     RX     |              TX                |          (BCM)14 TX           |
  #     TX     |              RX                |          (BCM)15 RX           |
  # -----------------------------------------------------------------------------
  #
  # Copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
  # licence     The MIT License (MIT)
  # author [Arya](xue.peng@dfrobot.com)
  # version  V1.0
  # date  2021-05-16
  # get from https://www.dfrobot.com
  # url from https://github.com/DFRobot/DFRobot_SCW8916B
'''

import sys
import os
import time

sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))
from DFRobot_SCW8916B import *

liquid = DFRobot_SCW8916B_UART()
#liquid = DFRobot_SCW8916B_IO(out = 22)       # 22 is the io pin of raspberry pi in BCM mode which is connected to the OUT pin of Non-contact liquid level sensor.

if __name__ == "__main__":
  print("Initialization Non-contact liquid level sensor....", end = " ")
  err = liquid.begin()
  while err != 0:
    print("failed. \nError code: %d"%err)
    if(err == ERR_CALIBRATION_CODE):
      print("You need to use calibration.py to calibration sensor.")
    else:
      print("Please check whether the hardware connection or configuration parameter is wrong.")
    time.sleep(1)
    print("Initialization Non-contact liquid level sensor....", end = " ")
    err = liquid.begin()
  print("done.")
  
  while True:
    flag = liquid.detect_water()        #Detect whether there is water at water level------>true: have water, false: no water.
    if flag:
      print("Have water: %d"%flag)
    else:
      print("No water: %d"%flag)

