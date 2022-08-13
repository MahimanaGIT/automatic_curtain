/**
 * @file controller.cpp
 * @author Dhiraj Deshmukh (deshmukhdhiraj15@gmail.com)
 * @brief Contains algorithms which controls the whole device from
 * initialization to giving commands to every subsystem
 * @version 0.1
 * @date 2022-07-19
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "controller.h"
#include "../alexa_interaction/alexa_interaction.h"
#include "../config/config.h"
#include "../connectivity/connectivity.h"
#include "../indicator/indicator.h"
#include "../logging/logging.h"
#include "../manual_interaction/manual_interaction.h"
#include "../motor_driver/motor_driver.h"
#include "../storage/storage.h"

Controller::Controller() {
  logger_.log(CONFIG_SET::LOG_TYPE::INFO, CONFIG_SET::LOG_CLASS::CONTROLLER,
              "Initializing");
}

Controller::~Controller() {}

void Controller::initialize() {}

void Controller::handle() {
  logger_.log(CONFIG_SET::LOG_TYPE::INFO, CONFIG_SET::LOG_CLASS::CONTROLLER,
              "Handle");
  logger_.setLoggingStatus(!logger_.getLoggingStatus());
}
