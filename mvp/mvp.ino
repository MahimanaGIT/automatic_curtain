/**
 * @file mvp.ino
 * @author Dhiraj Deshmukh (deshmukhdhiraj15@gmail.com)
 * @brief Arduino file for functioning automatic curtain
 * @version 0.1
 * @date 2022-07-19
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "src/alexa_interaction/alexa_interaction.h"
#include "src/config/config.h"
#include "src/connectivity/connectivity.h"
#include "src/controller/controller.h"
#include "src/indicator/indicator.h"
#include "src/logging/logging.h"
#include "src/manual_interaction/manual_interaction.h"
#include "src/motor_driver/motor_driver.h"
#include "src/storage/storage.h"

/**
 * @brief Runs at the power on / device reset stage
 *
 */
void setup() {}

/**
 * @brief Runs recurrently until power on after setup
 *
 */
void loop() {}