from __future__ import print_function
# -*- coding:utf-8 -*-

'''
  # demo_self_check.py
  #
  # The Non-contact liquid level sensor can self check to update sensitivity and cailibration mode.
  # There are two methods to detect:
  # (1)Level one-to-one detection
  # (2) UART detection.
  # Sensitivity level: 0~7(The higher the number, the lower the sensitivity)
  # Cailibration Mode:
  #  CALIBRATION_MODE_LOWER_LEVEL or 0            :   Only calibrate the lower water level
  #  CALIBRATION_MODE_LOWER_AND_UPPER_LEVEL or 1  :   Calibrate the upper and lower water levels.
  #  2~255                                        :   Error mode.
  #
  # @n connected table in eLEVEL_DETECT_MODE
  # -----------------------------------------------------------------------------
  # sensor pin |             MCU                |         raspberry pi          |
  #     TEST   | Connected to the IO pin of MCU |           17(BCM)             |
  #     OUT    | Connected to the IO pin of MCU |           22(BCM)             |
  #     EN     | Connected to the IO pin of MCU |           27(BCM)             |
  #     VCC    |            3.3V/5V             |            5V/3V3             |
  #     GND    |              GND               |             GND               |
  #     RX     |    Not connected, floating     |  Not connected, floating(-1)  |
  #     TX     | Connected to the RX pin of MCU |           15 RX (BCM)         |
  # -----------------------------------------------------------------------------
  #
  # @n connected table in eUART_DETECT_MODE
  # -----------------------------------------------------------------------------
  # sensor pin |             MCU                |         raspberry pi          |
  #     TEST   |    Not connected, floating     |  Not connected, floating(-1)  |
  #     OUT    |    Not connected, floating     |  Not connected, floating(-1)  |
  #     EN     | Connected to the IO pin of MCU |             27(BCM)           |
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

liquid = DFRobot_SCW8916B_UART(en = 27)
#liquid = DFRobot_SCW8916B_IO(out = 22, en = 27, test = 17)       # 22/27/17 is the io pin of raspberry pi in BCM mode which is connected to the OUT/EN/TEST pin of Non-contact liquid level sensor.

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
  
  print("Self check to update sensitivity...", end = " ")
  flag = liquid.self_check()
  if flag:
    print("done.")
    print("Current Sensitivity level(0~7): %d"%liquid.get_sensitivity_level())
    mode = liquid.get_calibration_mode()
    print("Cailibration mode: %d"%mode)
    print("Cailibration mode description:  %s"%liquid.get_calib_mode_description(mode))
  else:
    print("failed.")
  

