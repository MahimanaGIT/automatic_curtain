/**
 * @file controller.h
 * @author Mahimana Bhatt (mahimanabhatt@gmail.com)
 * @brief Defines the structure of the main controller of the device
 * @version 0.1
 * @date 2022-07-19
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef _CONTROLLER_INCLUDE_GUARD
#define _CONTROLLER_INCLUDE_GUARD

#include <memory>

#include "../alexa_interaction/alexa_interaction.h"
#include "../config/config.h"
#include "../connectivity/connectivity.h"
#include "../indicator/indicator.h"
#include "../logging/logging.h"
#include "../manual_interaction/manual_interaction.h"
#include "../motor_driver/motor_driver.h"
#include "../storage/storage.h"

class Controller {
   public:
    /**
     * @brief Construct a new Controller object, initializes the device
     *
     */
    Controller();

    /**
     * @brief Destroy the Controller object
     *
     */
    ~Controller();

    /**
     * @brief Continous loop after initialization, handles the controlling of the
     * device
     * Performs following functions:
     * 1. Updates the device status on indicator
     * 2. Fetches commands from alexa
     * 3. Fetches commands from manual control
     * 4. Gives instructions to motor driver
     * 5. Ensure connectivity to internet
     * 6. Controls OTA enable
     * 7. Handles any fault conditions and acts accordingly
     */
    void Handle();

   private:
    // Device parameter initialization
    CONFIG_SET::DEVICE_CRED device_cred_;
    CONFIG_SET::CALIB_PARAMS calib_params_;
    CONFIG_SET::OPERATION_MODE operation_mode_;
    CONFIG_SET::DEVICE_STATUS indicator_status_;
    CONFIG_SET::time_var mode_start_time_;
    bool long_press_enabled_;

    // Device class objects initialization
    std::shared_ptr<Logging> logger_{nullptr};
    std::unique_ptr<Storage> store_{nullptr};
    std::unique_ptr<Indicator> indicator_{nullptr};
    std::unique_ptr<Connectivity> connectivity_{nullptr};
    std::unique_ptr<AlexaInteraction> alexa_interaction_{nullptr};
    std::unique_ptr<MotorDriver> motor_driver_{nullptr};
    std::unique_ptr<ManualInteraction> manual_interaction_{nullptr};

    /**
     * @brief Mounts all the parameters from the storage
     *
     */
    bool LoadParameters();

    /**
     * @brief Save all the parameters to storage
     *
     */
    bool SaveParameters();

    /**
      * @brief Initialize reset mode
      *
      */
    void InitializeResetMode();

    /**
     * @brief Handle reset mode
     *
     */
    void HandleResetMode();

    /**
     * @brief Initialize maintenance mode
     *
     */
    void InitializeMaintenanceMode();

    /**
     * @brief Handle reset mode
     *
     */
    void HandleMaintenanceMode();

    /**
     * @brief Initialize operation mode
     *
     */
    void InitializeOperationMode();

    /**
     * @brief Handle operation mode
     *
     */
    void HandleOperationMode();

    /**
     * @brief Restarts device
     *
     */
    void RestartDevice();

    /**
     * @brief Call when exiting operation mode
     * 
     */
    void StopOperationMode();

    /**
     * @brief Call when exiting reset mode
     * 
     */
    void StopResetMode();

    /**
     * @brief Calibrate the motor driver, determine the stall value and total step
     * count, updates internal calib_params_ with new params if successful
     *
     * @return successful (true) or not (false)
     */
    bool Calibrate();
};

#endif
