/**
 * @file motor_driver.h
 * @author Mahimana Bhatt (mahimanabhatt@gmail.com)
 * @brief Defines structure for controller of stepper motor, uses library
 * TMC2209 for this usecase
 *
 * [FUTURE] Do all motor driver control using serial comm, replacing the step
 * and dir pins
 *
 * @version 0.1
 * @date 2022-07-19
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef _MOTOR_DRV_INCLUDE_GUARD
#define _MOTOR_DRV_INCLUDE_GUARD

#include "../config/config.h"

class MotorDriver {
public:
  /**
   * @brief Initializes motor drive TMC2209, and required pins
   *
   */
  MotorDriver();

  /**
   * @brief Cleans and disables motor driver
   *
   */
  ~MotorDriver();

  /**
   * @brief Returns the status of the motor driver, by checking if the motor is
   * running or the driver is in fault, or the motor is available for next
   * request
   *
   * @return CONFIG_SET::DRIVER_STATUS
   */
  CONFIG_SET::DRIVER_STATUS getStatus();

  /**
   * @brief This is the primary contact function for external requests, it will
   * handle all requests from the controller or anywhere else, run the thread
   * for supplying steps
   *
   * @return true : request accepted
   * @return false : rejected
   */
  bool fulfillRequest(CONFIG_SET::MOTION_REQUEST);

  /**
   * @brief Calibrate the motor driver, determine the stall value and total step
   * count
   *
   * @return CONFIG_SET::CALIB_PARAMS: calibration parameters
   */
  CONFIG_SET::CALIB_PARAMS calibrate();

private:
  /**
   * @brief Set the motor driver to sleep
   *
   * @return true : if successful
   * @return false : otherwise
   */
  bool sleep();

  /**
   * @brief Reset the motor driver by supplying appropriate pulse
   *
   * @return true : if successful
   * @return false : otherwise
   */
  void reset();

  /**
   * @brief Check if the nfault is enable from the motor driver
   *
   * @return true : if enabled
   * @return false : otherwise
   */
  bool checkFault();

  /**
   * @brief Runs on a timer, supplies step and direction signal to motor driver
   *
   */
  void pulseHandler();
};

#endif
