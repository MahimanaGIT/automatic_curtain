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
  this->logger_.setLoggingStatus(true);
  this->store_ = Storage(&this->logger_);
  this->indicator_ = Indicator(&this->logger_);
  this->device_cred_ = CONFIG_SET::DEVICE_CRED();
  this->calib_params_ = CONFIG_SET::CALIB_PARAMS();
  this->connectivity_ = Connectivity(&this->logger_, &this->device_cred_);
  this->connectivity_.ensureConnectivity(&this->device_cred_);
  this->connectivity_.startOTA();
  this->connectivity_.startWebpage();

  // for testing purposes
  this->store_.saveDeviceCred(&this->device_cred_);
  this->store_.populateDeviceCred(&this->device_cred_);
  char buffer[20];
  bool success = this->store_.populateCalibParam(&this->calib_params_);
  itoa(success, buffer, 10);
  this->logger_.log(CONFIG_SET::LOG_TYPE::INFO,
                    CONFIG_SET::LOG_CLASS::CONTROLLER, buffer);
  itoa(this->calib_params_.TOTAL_STEP_COUNT, buffer, 10);
  this->logger_.log(CONFIG_SET::LOG_TYPE::INFO,
                    CONFIG_SET::LOG_CLASS::CONTROLLER, buffer);
  itoa(this->calib_params_.STALL_VALUE, buffer, 10);
  this->logger_.log(CONFIG_SET::LOG_TYPE::INFO,
                    CONFIG_SET::LOG_CLASS::CONTROLLER, buffer);
  this->calib_params_.TOTAL_STEP_COUNT = 10;
  this->calib_params_.STALL_VALUE = 200;
  bool sec = this->store_.saveCalibParam(&this->calib_params_);
  itoa(sec, buffer, 10);
  this->logger_.log(CONFIG_SET::LOG_TYPE::INFO,
                    CONFIG_SET::LOG_CLASS::CONTROLLER, buffer);
  itoa(this->calib_params_.TOTAL_STEP_COUNT, buffer, 10);
  this->logger_.log(CONFIG_SET::LOG_TYPE::INFO,
                    CONFIG_SET::LOG_CLASS::CONTROLLER, buffer);
  itoa(this->calib_params_.STALL_VALUE, buffer, 10);
  this->logger_.log(CONFIG_SET::LOG_TYPE::INFO,
                    CONFIG_SET::LOG_CLASS::CONTROLLER, buffer);
  CONFIG_SET::CALIB_PARAMS dd = CONFIG_SET::CALIB_PARAMS();
  this->store_.populateCalibParam(&dd);
  itoa(dd.TOTAL_STEP_COUNT, buffer, 10);
  this->logger_.log(CONFIG_SET::LOG_TYPE::INFO,
                    CONFIG_SET::LOG_CLASS::CONTROLLER, buffer);
  itoa(dd.STALL_VALUE, buffer, 10);
  this->logger_.log(CONFIG_SET::LOG_TYPE::INFO,
                    CONFIG_SET::LOG_CLASS::CONTROLLER, buffer);

  this->logger_.log(CONFIG_SET::LOG_TYPE::INFO,
                    CONFIG_SET::LOG_CLASS::CONTROLLER, "Initializing");
}

void Controller::handle() {
  this->indicator_.updateStatus(CONFIG_SET::DEVICE_STATUS::NOT_CONNECTED);
  this->connectivity_.handleOTA();
  this->connectivity_.ensureConnectivity(&this->device_cred_);

  // for testing purposes only
  auto submission_return = this->connectivity_.getWebpageSubmission();
  if (std::get<0>(submission_return)) {
    CONFIG_SET::DEVICE_CRED submitted_device_cred =
        std::get<1>(submission_return);
    this->logger_.log(CONFIG_SET::LOG_TYPE::INFO,
                      CONFIG_SET::LOG_CLASS::CONTROLLER, "Got the Submission");
    Serial.println(submitted_device_cred.DEVICE_ID.c_str());
    Serial.println(submitted_device_cred.SSID.c_str());
    Serial.println(submitted_device_cred.PASSWORD.c_str());
    this->connectivity_.stopOTA();
    this->connectivity_.stopWebpage();
    this->connectivity_.stopWiFi();
  }
}
