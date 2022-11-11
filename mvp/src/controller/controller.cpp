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
      alexa_interaction_{nullptr},
      long_press_enabled_(false) {
    using namespace CONFIG_SET;
    logger_->SetLoggingStatus(true);
    calib_params_ = CONFIG_SET::CALIB_PARAMS();
    device_cred_ = CONFIG_SET::DEVICE_CRED();
    operation_mode_ = OPERATION_MODE::USER;
    indicator_status_ = DEVICE_STATUS::OPERATION_MODE;
    switch (operation_mode_) {
        case OPERATION_MODE::RESET:
            InitializeResetMode();
            break;
        case OPERATION_MODE::MAINTENANCE:
            InitializeMaintenanceMode();
            break;
        case OPERATION_MODE::USER:
            InitializeOperationMode();
            break;
        default:
            break;
    }
    logger_->Log(LOG_TYPE::INFO, LOG_CLASS::CONTROLLER, "Initialization Finished");
}

Controller::~Controller() {}

void Controller::Handle() {
    using namespace CONFIG_SET;
    indicator_->UpdateStatus(indicator_status_);
    switch (operation_mode_) {
        case OPERATION_MODE::RESET:
            HandleResetMode();
            break;
        case OPERATION_MODE::MAINTENANCE:
            HandleMaintenanceMode();
            break;
        case OPERATION_MODE::USER:
            HandleOperationMode();
            break;
        default:
            break;
    }
}

void Controller::InitializeResetMode() {
    using namespace CONFIG_SET;
    store_->Clear();
    logger_->Log(LOG_TYPE::INFO, LOG_CLASS::CONTROLLER, "Starting Reset Mode");
    connectivity_.reset(new Connectivity(logger_, &device_cred_));
    connectivity_->StartWebpage();
}

void Controller::HandleResetMode() {
    using namespace CONFIG_SET;
    constexpr LOG_CLASS CONTROLLER = LOG_CLASS::CONTROLLER;
    constexpr LOG_TYPE INFO = LOG_TYPE::INFO;
    logger_->Log(INFO, CONTROLLER, "Handling reset mode");
    auto webpage_submission = connectivity_->GetWebpageSubmission();
    if (std::get<0>(webpage_submission)) {
        device_cred_ = std::get<1>(webpage_submission);
        logger_->Log(INFO, CONTROLLER, "Got the webpage submission");
        logger_->Log(INFO, CONTROLLER, device_cred_.DEVICE_ID);
        logger_->Log(INFO, CONTROLLER, device_cred_.SSID);
        logger_->Log(INFO, CONTROLLER, device_cred_.PASSWORD);
        SaveParameters();
        // connectivity_->StopOTA();
        connectivity_->StopWebpage();
        connectivity_->StopWiFi();
        RestartDevice();
    }
}

void Controller::InitializeMaintenanceMode() {
    connectivity_->StartOTA();
}

void Controller::HandleMaintenanceMode() {
    connectivity_->HandleOTA();
}

void Controller::InitializeOperationMode() {
    using namespace CONFIG_SET;
    if (!LoadParameters()) {
        logger_->Log(LOG_TYPE::INFO, LOG_CLASS::CONTROLLER, "Storage Reading Failed.");
        indicator_status_ = DEVICE_STATUS::RESET_MODE;
        operation_mode_ = OPERATION_MODE::RESET;
        InitializeResetMode();
        return;
    }
    connectivity_.reset(new Connectivity(logger_, &device_cred_));
    connectivity_->EnsureConnectivity(&device_cred_);
    motor_driver_.reset(new MotorDriver(logger_, calib_params_));
    alexa_interaction_.reset(new AlexaInteraction(logger_, device_cred_.DEVICE_ID));
}

void Controller::HandleOperationMode() {
    using namespace CONFIG_SET;
    alexa_interaction_->HandleFauxmo();
    connectivity_->EnsureConnectivity(&device_cred_);

    auto alexa_request_sub = alexa_interaction_->GetAlexaRequest();
    if (std::get<0>(alexa_request_sub)) {
        MOTION_REQUEST submitted_alexa_request = std::get<1>(alexa_request_sub);
        logger_->Log(LOG_TYPE::INFO, LOG_CLASS::CONTROLLER, "Got the Alexa Submission");
        motor_driver_->FulfillRequest(submitted_alexa_request);
    }

    // test code for manual_interaction
    MANUAL_PUSH manual_action_test;
    time_var manual_action_time_test;
    tie(manual_action_test, manual_action_time_test) = manual_interaction_->GetManualActionAndTime();
    String out = "";
    switch (manual_action_test) {
        case MANUAL_PUSH::LONG_PRESS_UP:
            if (!long_press_enabled_) {
                MOTION_REQUEST motion_request_down;
                motion_request_down.PERCENTAGE = 100;
                motor_driver_->FulfillRequest(motion_request_down);
            }
            long_press_enabled_ = true;
            out = "LONG_PRESS_UP";
            break;
        case MANUAL_PUSH::LONG_PRESS_DOWN:
            if (!long_press_enabled_) {
                MOTION_REQUEST motion_request_down;
                motion_request_down.PERCENTAGE = 0;
                motor_driver_->FulfillRequest(motion_request_down);
            }
            long_press_enabled_ = true;
            out = "LONG_PRESS_DOWN";
            break;
        case MANUAL_PUSH::LONG_PRESS_BOTH:
            operation_mode_ = OPERATION_MODE::RESET;
            indicator_status_ = DEVICE_STATUS::RESET_MODE;
            InitializeResetMode();
            break;
            out = "LONG_PRESS_BOTH";
            break;
        case MANUAL_PUSH::DOUBLE_TAP_UP:
            MOTION_REQUEST motion_request_up_1;
            motion_request_up_1.PERCENTAGE = 100;
            motor_driver_->FulfillRequest(motion_request_up_1);
            out = "DOUBLE_TAP_UP";
            break;
        case MANUAL_PUSH::DOUBLE_TAP_DOWN:
            MOTION_REQUEST motion_request_down_1;
            motion_request_down_1.PERCENTAGE = 0;
            motor_driver_->FulfillRequest(motion_request_down_1);
            out = "DOUBLE_TAP_DOWN";
            break;
        case MANUAL_PUSH::DOUBLE_TAP_BOTH:
            operation_mode_ = OPERATION_MODE::MAINTENANCE;
            indicator_status_ = DEVICE_STATUS::MAINTENANCE_MODE;
            InitializeMaintenanceMode();
            out = "DOUBLE_TAP_BOTH";
            break;
        case MANUAL_PUSH::NO_PUSH:
            // out = "NO_PUSH";
            break;
        default:
            break;
    }
    if (out != "") {
        logger_->Log(LOG_TYPE::INFO, LOG_CLASS::CONTROLLER, out);
    }
    if ((manual_action_test != MANUAL_PUSH::LONG_PRESS_DOWN && manual_action_test != MANUAL_PUSH::LONG_PRESS_UP) &&
        long_press_enabled_) {
        long_press_enabled_ = false;
        motor_driver_->CancelCurrentRequest();
    }
}

bool Controller::LoadParameters() {
    using namespace CONFIG_SET;
    bool success_calib_param = store_->PopulateCalibParam(&calib_params_);
    bool success_creds = store_->PopulateDeviceCred(&device_cred_);
    calib_params_.TOTAL_STEP_COUNT = 100000;
    calib_params_.DIRECTION = true;
    return success_calib_param && success_creds;
}

bool Controller::SaveParameters() {
    return store_->SaveDeviceCred(&device_cred_) && store_->SaveCalibParam(&calib_params_);
}

void Controller::RestartDevice() {
    ESP.restart();
}

CONFIG_SET::CALIB_PARAMS Controller::Calibrate() {
    return CONFIG_SET::CALIB_PARAMS();
}
