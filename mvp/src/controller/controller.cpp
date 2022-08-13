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
#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>

Controller::Controller() {}

Controller::~Controller() {}

void Controller::initialize() {
  logger_.setLoggingStatus(true);
  store_ = Storage(&logger_);
  indicator_ = Indicator(&logger_);
  device_cred_ = CONFIG_SET::DEVICE_CRED();
  calib_params_ = CONFIG_SET::CALIB_PARAMS();

  logger_.log(CONFIG_SET::LOG_TYPE::INFO, CONFIG_SET::LOG_CLASS::CONTROLLER,
              "Initializing");

  // for testing purposes
  store_.saveDeviceCred(&device_cred_);
  store_.populateDeviceCred(&device_cred_);
  char buffer[20];
  bool success = store_.populateCalibParam(&calib_params_);
  itoa(success, buffer, 10);
  logger_.log(CONFIG_SET::LOG_TYPE::INFO, CONFIG_SET::LOG_CLASS::CONTROLLER,
              buffer);
  itoa(calib_params_.TOTAL_STEP_COUNT, buffer, 10);
  logger_.log(CONFIG_SET::LOG_TYPE::INFO, CONFIG_SET::LOG_CLASS::CONTROLLER,
              buffer);
  itoa(calib_params_.STALL_VALUE, buffer, 10);
  logger_.log(CONFIG_SET::LOG_TYPE::INFO, CONFIG_SET::LOG_CLASS::CONTROLLER,
              buffer);
  calib_params_.TOTAL_STEP_COUNT = 10;
  calib_params_.STALL_VALUE = 200;
  bool sec = store_.saveCalibParam(&calib_params_);
  itoa(sec, buffer, 10);
  logger_.log(CONFIG_SET::LOG_TYPE::INFO, CONFIG_SET::LOG_CLASS::CONTROLLER,
              buffer);
  itoa(calib_params_.TOTAL_STEP_COUNT, buffer, 10);
  logger_.log(CONFIG_SET::LOG_TYPE::INFO, CONFIG_SET::LOG_CLASS::CONTROLLER,
              buffer);
  itoa(calib_params_.STALL_VALUE, buffer, 10);
  logger_.log(CONFIG_SET::LOG_TYPE::INFO, CONFIG_SET::LOG_CLASS::CONTROLLER,
              buffer);
  CONFIG_SET::CALIB_PARAMS dd = CONFIG_SET::CALIB_PARAMS();
  store_.populateCalibParam(&dd);
  itoa(dd.TOTAL_STEP_COUNT, buffer, 10);
  logger_.log(CONFIG_SET::LOG_TYPE::INFO, CONFIG_SET::LOG_CLASS::CONTROLLER,
              buffer);
  itoa(dd.STALL_VALUE, buffer, 10);
  logger_.log(CONFIG_SET::LOG_TYPE::INFO, CONFIG_SET::LOG_CLASS::CONTROLLER,
              buffer);

  logger_.log(CONFIG_SET::LOG_TYPE::INFO, CONFIG_SET::LOG_CLASS::CONTROLLER,
              "Initializing");
}

void Controller::handle() {
  logger_.log(CONFIG_SET::LOG_TYPE::INFO, CONFIG_SET::LOG_CLASS::CONTROLLER,
              "Handle");

  // for testing purposes
  indicator_.updateStatus(CONFIG_SET::DEVICE_STATUS::NOT_CONNECTED);
}
