from __future__ import print_function
# -*- coding:utf-8 -*-

'''
  # demo_set_and_get_sensitivity.py(only support UART detection mode)
  #
  #This demo tells how to set and get sensitivity of Non-contact liquid level sensor.
  #Features:
  #1. Support sensitivity level setting,ranging 0~7 or eSENSITIVITY_LEVEL0~eSENSITIVITY_LEVEL7.(only support UART detection mode)
  #2. Get sensitivity level, in IO detecte mode, the sensitivity level shoudle be change by replace resistance. 
  #In UART detecte mode, can set sensitivity level by call software function setSensitivityLevel.
  #
  #note: This demo only support eUARTDetecteMode, not using eLevelDetecteMode.
  #
  # @n connected table in eUARTDetecteMode
  # -----------------------------------------------------------------------------
  # sensor pin |             MCU                |         raspberry pi          |
  #     TEST   |    Not connected, floating     |  Not connected, floating(-1)  |
  #     OUT    |    Not connected, floating     |  Not connected, floating(-1)  |
  #     EN     | connected to the io pin of MCU |             27(BCM)           |
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

liquid = DFRobot_SCW8916B_UART(en = 27) # 27 is the io pin of raspberry pi which is connected to the EN pin of Non-contact liquid level sensors.

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
  
  print("Set sensitivity level...", end = " ")
  while liquid.set_sensitivity_level(level = liquid.eSENSITIVITY_LEVEL1) == False:  #27 is the io pin of raspberry pi in BCM which is connected to the EN pin of sensor, Set 8 level sensitivity(0~7) to the Non-contact liquid level sensor.
    print(".",end=" ")
    time.sleep(0.02)
  print("sucess.")
  
  print("Self check to update sensitivity...", end = " ")
  while liquid.self_check() == False:  # 27 is the io pin of raspberry pi in BCM which is connected to the EN pin of  Non-contact liquid level sensor.
    print(".",end=" ")
    time.sleep(0.02)
  print("done.")

  print("Current sensitivity level(0~7): %d"%liquid.get_sensitivity_level())

