/**
 * @file motor_driver.h
 * @author Mahimana Bhatt (mahimanabhatt@gmail.com)
 * @brief Defines structure for controller of stepper motor, uses library
 * TMC2209 for this usecase
 * @version 0.1
 * @date 2022-07-19
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

class MotorDriver {
public:
  /**
   * @brief Construct a new MotorDriver object
   *
   */
  MotorDriver();

  /**
   * @brief Destroy the MotorDriver object
   *
   */
  ~MotorDriver();

  /**
   * @brief Hello
   *
   */
  void test_function();
};