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

#include <mutex>

#include "src/controller/controller.h"

std::unique_ptr<Controller> ctrl{nullptr};

/**
 * @brief Runs at the power on / device reset stage
 *
 */
void setup() { ctrl.reset(new Controller()); }

/**
 * @brief Runs recurrently until power on after setup, handles the device
 * functioning
 *
 */
void loop() { ctrl->handle(); }
