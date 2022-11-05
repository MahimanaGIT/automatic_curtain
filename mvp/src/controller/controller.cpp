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

#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>

#include <memory>

#include "../alexa_interaction/alexa_interaction.h"
#include "../config/config.h"
#include "../connectivity/connectivity.h"
#include "../indicator/indicator.h"
#include "../logging/logging.h"
#include "../manual_interaction/manual_interaction.h"
#include "../motor_driver/motor_driver.h"
#include "../storage/storage.h"

Controller::Controller()
    : logger_(new Logging()),
      store_(new Storage(logger_)),
      indicator_(new Indicator(logger_)),
      manual_interaction_(new MI_Cls),
      connectivity_{nullptr},
      motor_driver_{nullptr},
      alexa_interaction_{nullptr} {
    using namespace CONFIG_SET;

    device_cred_ = DEVICE_CRED();
    device_cred_.SSID = "NETGEAR37";
    device_cred_.PASSWORD = "phobiclotus754";
    calib_params_ = CALIB_PARAMS();

    // for testing purposes only
    calib_params_.STALL_VALUE = 32;
    calib_params_.TOTAL_STEP_COUNT = 100000;

    logger_->setLoggingStatus(true);

    connectivity_.reset(new Connectivity(logger_, &device_cred_));
    motor_driver_.reset(new MotorDriver(logger_, calib_params_));
    alexa_interaction_.reset(new AlexaInteraction(logger_, device_cred_.DEVICE_ID));

    connectivity_->ensureConnectivity(&device_cred_);
    // connectivity_->startOTA();
    // connectivity_->startWebpage();

    // for testing purposes
    store_->saveDeviceCred(&device_cred_);
    store_->populateDeviceCred(&device_cred_);
    char buffer[20];
    bool success = store_->populateCalibParam(&calib_params_);
    itoa(success, buffer, 10);
    // logger_->log(INFO, CONTROLLER, buffer);
    itoa(calib_params_.TOTAL_STEP_COUNT, buffer, 10);
    // logger_->log(INFO, CONTROLLER, buffer);
    itoa(calib_params_.STALL_VALUE, buffer, 10);
    // logger_->log(INFO, CONTROLLER, buffer);
    calib_params_.TOTAL_STEP_COUNT = 10;
    calib_params_.STALL_VALUE = 200;
    bool sec = store_->saveCalibParam(&calib_params_);
    itoa(sec, buffer, 10);
    // logger_->log(INFO, CONTROLLER, buffer);
    itoa(calib_params_.TOTAL_STEP_COUNT, buffer, 10);
    // logger_->log(INFO, CONTROLLER, buffer);
    itoa(calib_params_.STALL_VALUE, buffer, 10);
    // logger_->log(INFO, CONTROLLER, buffer);
    CALIB_PARAMS dd = CALIB_PARAMS();
    store_->populateCalibParam(&dd);
    itoa(dd.TOTAL_STEP_COUNT, buffer, 10);
    // logger_->log(INFO, CONTROLLER, buffer);
    itoa(dd.STALL_VALUE, buffer, 10);
    // logger_->log(INFO, CONTROLLER, buffer);

    // logger_->log(INFO, CONTROLLER, "Initializing");
}

Controller::~Controller() {}

void Controller::handle() {
    using namespace CONFIG_SET;
    constexpr LOG_CLASS CONTROLLER = LOG_CLASS::CONTROLLER;
    constexpr LOG_TYPE INFO = LOG_TYPE::INFO;
    connectivity_->handleOTA();
    alexa_interaction_->handleFauxmo();
    motor_driver_->handle();
    indicator_->updateStatus(DEVICE_STATUS::NOT_CONNECTED);

    connectivity_->ensureConnectivity(&device_cred_);

    // for testing purposes only
    auto alexa_request_sub = alexa_interaction_->getAlexaRequest();
    if (std::get<0>(alexa_request_sub)) {
        MOTION_REQUEST submitted_alexa_request = std::get<1>(alexa_request_sub);
        logger_->log(INFO, CONTROLLER, "Got the Alexa Submission");
        motor_driver_->fulfillRequest(submitted_alexa_request);
    }

    // for testing purposes only
    auto webpage_submission = connectivity_->getWebpageSubmission();
    if (std::get<0>(webpage_submission)) {
        DEVICE_CRED sub_dev_cred = std::get<1>(webpage_submission);
        logger_->log(INFO, CONTROLLER, "Got the Webpage Submission");
        logger_->log(INFO, CONTROLLER, sub_dev_cred.DEVICE_ID);
        logger_->log(INFO, CONTROLLER, sub_dev_cred.SSID);
        logger_->log(INFO, CONTROLLER, sub_dev_cred.PASSWORD);
        // connectivity_->stopOTA();
        connectivity_->stopWebpage();
        connectivity_->stopWiFi();
    }

    // test code for manual_interaction
    C_S::MANUAL_PUSH manual_action_test;
    C_S::time_var manual_action_time_test;
    tie(manual_action_test, manual_action_time_test) = manual_interaction_->GetManualActionAndTime(true);

    switch (manual_action_test) {
        case C_S::MANUAL_PUSH::LONG_PRESS_UP:
            std::cout << "LONG_PRESS_UP" << std::endl;
            break;
        case C_S::MANUAL_PUSH::LONG_PRESS_DOWN:
            std::cout << "LONG_PRESS_DOWN" << std::endl;
            break;
        case C_S::MANUAL_PUSH::LONG_PRESS_BOTH:
            std::cout << "LONG_PRESS_BOTH" << std::endl;
            break;
        case C_S::MANUAL_PUSH::DOUBLE_TAP_UP:
            std::cout << "DOUBLE_TAP_UP" << std::endl;
            break;
        case C_S::MANUAL_PUSH::DOUBLE_TAP_DOWN:
            std::cout << "DOUBLE_TAP_DOWN" << std::endl;
            break;
        case C_S::MANUAL_PUSH::DOUBLE_TAP_BOTH:
            std::cout << "DOUBLE_TAP_BOTH" << std::endl;
            break;
    }
}
