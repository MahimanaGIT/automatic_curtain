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

#include <chrono>
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
      long_press_enabled_(false),
      last_blind_percentage_(0),
      last_motor_status_(CONFIG_SET::DRIVER_STATUS::AVAILABLE) {
    using namespace CONFIG_SET;
    logger_->SetLoggingStatus(true);
    calib_params_ = CONFIG_SET::CALIB_PARAMS();
    device_cred_ = CONFIG_SET::DEVICE_CRED();
    store_->PopulateOperationMode(&operation_mode_);
    switch (operation_mode_) {
        case OPERATION_MODE::RESET:
            indicator_status_ = DEVICE_STATUS::RESET_MODE;
            InitializeResetMode();
            break;
        case OPERATION_MODE::MAINTENANCE:
            indicator_status_ = DEVICE_STATUS::MAINTENANCE_MODE;
            InitializeMaintenanceMode();
            break;
        case OPERATION_MODE::USER:
            indicator_status_ = DEVICE_STATUS::OPERATION_MODE;
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
    logger_->Log(LOG_TYPE::INFO, LOG_CLASS::CONTROLLER, "Starting Reset Mode");
    OPERATION_MODE op = OPERATION_MODE::USER;
    store_->SaveOperationMode(&op);
    connectivity_.reset(new Connectivity(logger_, &device_cred_));
    connectivity_->StartWebpage();
    mode_start_time_ = current_time::now();
}

void Controller::HandleResetMode() {
    using namespace CONFIG_SET;
    constexpr LOG_CLASS CONTROLLER = LOG_CLASS::CONTROLLER;
    constexpr LOG_TYPE INFO = LOG_TYPE::INFO;
    auto webpage_submission = connectivity_->GetWebpageSubmission();
    if (std::get<0>(webpage_submission)) {
        device_cred_ = std::get<1>(webpage_submission);
        logger_->Log(INFO, CONTROLLER, "Got the webpage submission");
        connectivity_->StopWebpage();
        connectivity_->StopWiFi();
        Calibrate();
        operation_mode_ = OPERATION_MODE::USER;
        store_->Clear();
        SaveParameters();
        RestartDevice();
    }
    MANUAL_PUSH manual_action_test;
    time_var manual_action_time_test;
    std::tie(manual_action_test, manual_action_time_test) = manual_interaction_->GetManualActionAndTime();
    if (manual_action_test == MANUAL_PUSH::DOUBLE_TAP_BOTH) {
        operation_mode_ = OPERATION_MODE::MAINTENANCE;
        indicator_status_ = DEVICE_STATUS::MAINTENANCE_MODE;
        StopResetMode();
        InitializeMaintenanceMode();
    }
    int exec_time = std::chrono::duration_cast<std::chrono::seconds>(current_time::now() - mode_start_time_).count();
    if (exec_time > MODE_EXPIRE_TIME_LIMIT) {
        // restarting
        logger_->Log(LOG_TYPE::INFO, LOG_CLASS::CONTROLLER, "Reset Mode Expired");
        RestartDevice();
    }
}

void Controller::InitializeMaintenanceMode() {
    using namespace CONFIG_SET;
    logger_->Log(LOG_TYPE::INFO, LOG_CLASS::CONTROLLER, "Starting Maintenance Mode");
    connectivity_.reset(new Connectivity(logger_, &device_cred_));
    connectivity_->StartOTA();
    mode_start_time_ = current_time::now();
}

void Controller::HandleMaintenanceMode() {
    using namespace CONFIG_SET;
    connectivity_->HandleOTA();
    int exec_time = std::chrono::duration_cast<std::chrono::seconds>(current_time::now() - mode_start_time_).count();
    if (exec_time > MODE_EXPIRE_TIME_LIMIT) {
        // restarting
        logger_->Log(LOG_TYPE::INFO, LOG_CLASS::CONTROLLER, "Maintenance Mode Expired");
        RestartDevice();
    }
}

void Controller::InitializeOperationMode() {
    using namespace CONFIG_SET;
    logger_->Log(LOG_TYPE::INFO, LOG_CLASS::CONTROLLER, "Starting Operation Mode");
    if (!LoadParameters()) {
        logger_->Log(LOG_TYPE::INFO, LOG_CLASS::CONTROLLER, "Storage Reading Failed.");
        indicator_status_ = DEVICE_STATUS::RESET_MODE;
        operation_mode_ = OPERATION_MODE::RESET;
        InitializeResetMode();
        return;
    }
    connectivity_.reset(new Connectivity(logger_, &device_cred_));
    connectivity_->StartEnsureConnectivity(device_cred_);
    delay(100);
    motor_driver_.reset(new MotorDriver(logger_, calib_params_));
}

void Controller::HandleOperationMode() {
    using namespace CONFIG_SET;
    if (!alexa_interaction_ && connectivity_->IsConnected()) {
        alexa_interaction_.reset(new AlexaInteraction(logger_, device_cred_.DEVICE_ID));
    } else if (alexa_interaction_) {
        alexa_interaction_->HandleFauxmo();

        auto alexa_request_sub = alexa_interaction_->GetAlexaRequest();
        if (std::get<0>(alexa_request_sub)) {
            MOTION_REQUEST submitted_alexa_request = std::get<1>(alexa_request_sub);
            logger_->Log(LOG_TYPE::INFO, LOG_CLASS::CONTROLLER, "Got the Alexa Submission");
            motor_driver_->FulfillRequest(submitted_alexa_request);
        }

        DRIVER_STATUS current_status = motor_driver_->GetStatus();
        if (last_motor_status_ != current_status && current_status == DRIVER_STATUS::AVAILABLE) {
            int current_percentage = motor_driver_->GetPercentage();
            if (current_percentage != last_blind_percentage_) {
                MOTION_REQUEST motion_request;
                motion_request.PERCENTAGE = current_percentage;
                logger_->Log(LOG_TYPE::INFO, LOG_CLASS::CONTROLLER, "Updating Alexa Percentage");
                alexa_interaction_->SetState(motion_request);
                last_blind_percentage_ = current_percentage;
            }
        }
        last_motor_status_ = current_status;
    }

    if (connectivity_->GetSecLostConnection() > MAX_SECONDS_LOST_WIFI) {
        logger_->Log(LOG_TYPE::INFO, LOG_CLASS::CONTROLLER, "WiFi Lost");
        RestartDevice();
    }

    MANUAL_PUSH manual_action_test;
    time_var manual_action_time_test;
    std::tie(manual_action_test, manual_action_time_test) = manual_interaction_->GetManualActionAndTime();
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
            StopOperationMode();
            store_->SaveOperationMode(&operation_mode_);
            RestartDevice();
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
            StopOperationMode();
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
    return success_calib_param && success_creds;
}

bool Controller::SaveParameters() {
    return store_->SaveDeviceCred(&device_cred_) && store_->SaveCalibParam(&calib_params_) &&
           store_->SaveOperationMode(&operation_mode_);
}

bool Controller::Calibrate() {
    using namespace CONFIG_SET;
    logger_->Log(LOG_TYPE::INFO, LOG_CLASS::CONTROLLER, "Calibrating");

    delay(3000);
    CALIB_PARAMS calib_params;

    auto find_end = [&]() -> std::tuple<int, int> {
        MOTION_REQUEST motion_request_up;
        motor_driver_.reset(new MotorDriver(logger_, calib_params));
        motion_request_up.PERCENTAGE = 100;
        motor_driver_->FulfillRequest(motion_request_up);

        time_var start_time = current_time::now();
        int execution_time = 0;
        while (motor_driver_->GetStatus() != DRIVER_STATUS::BUSY) {
            execution_time = std::chrono::duration_cast<std::chrono::seconds>(current_time::now() - start_time).count();
            if (execution_time > MOTOR_STOP_TIME_SEC) {
                logger_->Log(LOG_TYPE::INFO, LOG_CLASS::CONTROLLER,
                             "Stopping Motor, reached time limit for calibration");
                break;
            }
        }

        while (motor_driver_->GetStatus() != DRIVER_STATUS::AVAILABLE) {
            execution_time = std::chrono::duration_cast<std::chrono::seconds>(current_time::now() - start_time).count();
            if (execution_time > MOTOR_STOP_TIME_SEC) {
                logger_->Log(LOG_TYPE::INFO, LOG_CLASS::CONTROLLER,
                             "Stopping Motor, reached time limit for calibration");
                break;
            }
        }
        logger_->Log(LOG_TYPE::INFO, LOG_CLASS::CONTROLLER, "Loop Ended");

        if (motor_driver_->GetStatus() != DRIVER_STATUS::AVAILABLE) {
            motor_driver_->CancelCurrentRequest();
        }
        return std::make_tuple(execution_time, motor_driver_->GetSteps());
    };

    calib_params.DIRECTION = true;
    int first_dir_exec_time, first_dir_stps;
    std::tie(first_dir_exec_time, first_dir_stps) = find_end();
    if (first_dir_exec_time >= MOTOR_STOP_TIME_SEC) {
        logger_->Log(LOG_TYPE::INFO, LOG_CLASS::CONTROLLER, "Not found an end, returning from first dir");
        return false;
    }
    delay(3000);

    calib_params.DIRECTION = false;
    int sec_dir_exec_time, sec_dir_stps;
    std::tie(sec_dir_exec_time, sec_dir_stps) = find_end();
    if (sec_dir_exec_time >= MOTOR_STOP_TIME_SEC) {
        logger_->Log(LOG_TYPE::INFO, LOG_CLASS::CONTROLLER, "Not found an end, returning from second dir");
        return false;
    }

    calib_params.DIRECTION = (first_dir_exec_time < (sec_dir_exec_time * 0.3));
    calib_params.TOTAL_STEP_COUNT = std::max(first_dir_stps, sec_dir_stps);
    calib_params_ = calib_params;
    logger_->Log(LOG_TYPE::INFO, LOG_CLASS::CONTROLLER, "Calibration Successful");
    return true;
}

void Controller::StopOperationMode() {
    using namespace CONFIG_SET;
    logger_->Log(LOG_TYPE::INFO, LOG_CLASS::CONTROLLER, "Stopping Operation Mode");
    motor_driver_.reset();
    alexa_interaction_.reset();
    connectivity_.reset();
}

void Controller::StopResetMode() {
    using namespace CONFIG_SET;
    logger_->Log(LOG_TYPE::INFO, LOG_CLASS::CONTROLLER, "Stopping Reset Mode");
    connectivity_.reset();
}

void Controller::RestartDevice() {
    using namespace CONFIG_SET;
    logger_->Log(LOG_TYPE::INFO, LOG_CLASS::CONTROLLER, "Restarting Device");
    ESP.restart();
}
