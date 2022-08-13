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
   * @brief Initialize the device
   *
   */
  void initialize();

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
  void handle();

private:
  Logging logger_;
  /**
   * @brief Mounts all the parameters from the storage
   *
   */
  void loadParameters();

  /**
   * @brief Save all the parameters to storage
   *
   */
  void saveParameters();

  /**
   * @brief Reset mode for device
   *
   */
  void reset();
};

#endif
