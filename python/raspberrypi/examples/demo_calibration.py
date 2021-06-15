from __future__ import print_function
# -*- coding:utf-8 -*-

'''
  # demo_calibration.py
  #
  # This demo provides the water level sensor calibration function. If the water level sensor is inaccurate 
  # or has never been calibrated, you can run this demo to calibrate the sensor to get the correct.
  # Experimental phenomena: If there is water at this level that will print digital 1  or print digital 0
  # There are two methods to detect:
  # (1)Level one-to-one detection
  # (2) UART detection.
  # There two calibration mode:
  # (1) CALIBRATION_MODE_LOWER_LEVEL or 0            :   Only calibrate the lower water level(support)
  # (2) CALIBRATION_MODE_LOWER_AND_UPPER_LEVEL or 1  :   Calibrate the upper and lower water levels.(not support)
  # (3) 2~255                                        :   Error mode.
  # Calibration way in CALIBRATION_MODE_LOWER_LEVEL: 
  # (a) lower water level calibration:  Keep no water in Water detection area, and send lower water level calibration command.
  # (b) check calibration state.
  # Calibration way in CALIBRATION_MODE_LOWER_AND_UPPER_LEVEL: 
  # (a) lower water level calibration: Keep no water in Water detection area, and send lower water level calibration command.
  # (b) upper water level calibration: Keep have water in Water detection area, and send upper water level calibration command.
  # (c) check calibration state.
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

STEP1_START_CALIB  =  1
STEP2_END_CALIB    =  2

if __name__ == "__main__":
  print("Initialization Non-contact liquid level sensor...")
  err = liquid.begin()
  print("Error code: %d"%err)

  self_flag = liquid.self_check()
  if self_flag:
    print("Self check sucess.")
    mode = liquid.get_calibration_mode();
    print("Cailibration mode: %d"%mode)
    print("Cailibration mode description: %s"%liquid.get_calib_mode_description(mode))
    if mode == CALIBRATION_MODE_LOWER_LEVEL:
      calib_flag = False
      calib_step = STEP1_START_CALIB
      print("Keep the water inspection area free of water and do not touch the water inspection area in 2s")
      time.sleep(2)
      print("Starting calibration...")
      while True:
        if calib_step == STEP1_START_CALIB:
          calib_flag = liquid.calibration()
          if calib_flag == False:
            print("The lower water level calibration failed. \n")
            continue
          calib_step = STEP2_END_CALIB
          
        if calib_step == STEP2_END_CALIB:
          calib_flag = liquid.check_calibration_state();
          if calib_flag == False:
            print("Calibration failed.")
            calib_step = STEP1_START_CALIB
            print("Keep the water inspection area free of water and do not touch the water inspection area in 2s")
            time.sleep(2)
            print("Starting calibration...")
            continue
          break
      print("End calibration.")
    else:
      print("Does not support this calibration mode")
  else:
    print("Self check failed")
    
  while True:
    flag = liquid.detect_water()        #Detect whether there is water at water level------>true: have water, false: no water.s
    if flag:
      print("Have water: %d"%flag)
    else:
      print("No water: %d"%flag)        #Print water state at the water level, 0: No water, 1: Have water.

