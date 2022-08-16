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
  connectivity_ = Connectivity(&logger_, &device_cred_);
  connectivity_.ensureConnectivity(&device_cred_);
  connectivity_.startOTA();
  // connectivity_.startWebpage();
  alexa_interaction_ = AlexaInteraction(&logger_, device_cred_.DEVICE_ID);

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
  indicator_.updateStatus(CONFIG_SET::DEVICE_STATUS::NOT_CONNECTED);
  connectivity_.handleOTA();
  connectivity_.ensureConnectivity(&device_cred_);
  alexa_interaction_.handleFauxmo();

  // for testing purposes only
  auto alexa_request_sub = alexa_interaction_.getAlexaRequest();
  if (std::get<0>(alexa_request_sub)) {
    CONFIG_SET::ALEXA_REQUEST submitted_alexa_request =
        std::get<1>(alexa_request_sub);
    logger_.log(CONFIG_SET::LOG_TYPE::INFO, CONFIG_SET::LOG_CLASS::CONTROLLER,
                "Got the Alexa Submission");
    Serial.println(submitted_alexa_request.STATE);
    Serial.println(submitted_alexa_request.PERCENTAGE);
  }

  // for testing purposes only
  auto webpage_submission = connectivity_.getWebpageSubmission();
  if (std::get<0>(webpage_submission)) {
    CONFIG_SET::DEVICE_CRED submitted_device_cred =
        std::get<1>(webpage_submission);
    logger_.log(CONFIG_SET::LOG_TYPE::INFO, CONFIG_SET::LOG_CLASS::CONTROLLER,
                "Got the Submission");
    Serial.println(submitted_device_cred.DEVICE_ID.c_str());
    Serial.println(submitted_device_cred.SSID.c_str());
    Serial.println(submitted_device_cred.PASSWORD.c_str());
    connectivity_.stopOTA();
    connectivity_.stopWebpage();
    connectivity_.stopWiFi();
  }
}
