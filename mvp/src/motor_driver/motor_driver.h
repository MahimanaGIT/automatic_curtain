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

#include <Arduino.h>
#include <HardwareSerial.h>
#include <TMCStepper.h>

#include <ctime>
#include <memory>
#include <thread>

#include "../config/config.h"
#include "../logging/logging.h"

class MotorDriver : private TMC2209Stepper {
   public:
    /**
   * @brief Initializes motor drive TMC2209, and required pins
   *
   */
    MotorDriver(std::shared_ptr<Logging>& logging);

    /**
   * @brief Initializes motor drive TMC2209, and required pins
   *
   */
    MotorDriver(std::shared_ptr<Logging>& logging, CONFIG_SET::CALIB_PARAMS calib_param);

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
    CONFIG_SET::DRIVER_STATUS GetStatus();

    /**
   * @brief Returns the current steps of motor
   *
   * @return current_steps
   */
    int GetSteps();

    /**
   * @brief Returns the current percentage of motor
   *
   * @return current percentage
   */
    int GetPercentage();

    /**
   * @brief This is the primary contact function for external requests, it will
   * handle all requests from the controller or anywhere else, run the thread
   * for supplying steps
   *
   * @return true : request accepted
   * @return false : rejected
   */
    bool FulfillRequest(CONFIG_SET::MOTION_REQUEST request);

    /**
   * @brief Cancels current request
   *
   * @return true: Cancellation successful
   * @return false: otherwise
   */
    bool CancelCurrentRequest();

    /**
   * @brief Updates calibration parameters to be used by motor driver
   *
   */
    void UpdateCalibParams(CONFIG_SET::CALIB_PARAMS calib_param);

    /**
   * @brief Runs on a timer, supplies step and direction signal to motor driver
   *
   */
    static void InterruptForIndex();

    /**
   * @brief Stop handler thread
   *
   */
    void StopHandler();

   private:
    CONFIG_SET::DRIVER_STATUS driver_status_;
    CONFIG_SET::CALIB_PARAMS calib_params_;

    std::shared_ptr<Logging> logger_;

    bool is_motor_running_ = false;
    hw_timer_t* step_timer_ = NULL;
    int expected_step_ = 0;
    bool blind_traversal_requested_ = false;
    bool stop_requested_ = false;
    bool keep_handler_running_ = false;
    static int full_rot_step_count_;
    static int current_step_;
    static bool direction_;
    std::time_t last_motor_start_time_sec_ = std::time(nullptr);
    std::unique_ptr<std::thread> handler_thread_{nullptr};

    /**
   * @brief Motor Driver class handler, communicates with driver and is
   * responsible for reading latest motion request, cancellationof request, and
   * acting upon it i.e. moving the motor and stopping it when stall is
   * detected, or motor reached destination or stop is requested or timer limit
   * is reached
   *
   */
    void Handler();

    /**
   * @brief Enable Motor Driver
   * Give argument as true for enabling, false for disabling the driver
   * @return true : if successful
   * @return false : otherwise
   */
    bool EnableDriver(bool enable);

    /**
   * @brief Sets needed parameters for the driver
   *
   */
    void InitializeDriver();

    /**
   * @brief Responsible for starting the motor, i.e. setups the driver and sets
   * required velocity of motor
   *
   */
    void StartMotor();

    /**
   * @brief Responsible for stopping the motor, i.e. disables the driver and
   * sets velocity of motor to zero
   *
   */
    void StopMotor();

    /**
   * @brief Starts handler thread
   *
   */
    void StartHandler();

    /**
   * @brief Reset the current steps of motor
   *
   */
    void ResetSteps();
};

#endif
