/**
 * @file motor_driver.cpp
 * @author Dhiraj Deshmukh (deshmukhdhiraj15@gmail.com)
 * @brief Contains algorithms for controlling stepper motor
 * @version 0.1
 * @date 2022-07-19
 *
 * @copyright Copyright (c) 2022
 *
 */

/*
TODO: Tune parameters to reduce stepper motor noise
TODO: Implement calibration mode
*/

#include "motor_driver.h"

#include <Arduino.h>
#include <HardwareSerial.h>
#include <TMCStepper.h>

#include <ctime>

#include "../config/config.h"
#include "../logging/logging.h"

bool MotorDriver::direction_ = false;
int MotorDriver::current_step_ = 0;
int MotorDriver::full_rot_step_count_ = (4 * CONFIG_SET::MOTOR_DRIVER_MICROSTEP);

MotorDriver::MotorDriver(std::shared_ptr<Logging> logging, CONFIG_SET::CALIB_PARAMS calib_param)
    : logger_(logging), TMC2209Stepper(&Serial2, CONFIG_SET::MOTOR_DRIVER_R_SENSE, CONFIG_SET::MOTOR_DRIVER_ADDRESS) {
    using namespace CONFIG_SET;
    pinMode(PIN_MD_DIAG, INPUT);
    pinMode(PIN_MD_ENABLE, OUTPUT);
    pinMode(PIN_MD_STEP, OUTPUT);
    pinMode(PIN_MD_DIR, OUTPUT);
    pinMode(PIN_MD_MS1, OUTPUT);
    pinMode(PIN_MD_MS2, OUTPUT);
    pinMode(PIN_MD_INDEX, INPUT_PULLUP);
    digitalWrite(PIN_MD_MS1, LOW);
    digitalWrite(PIN_MD_MS2, LOW);

    Serial2.begin(MOTOR_DRIVER_BAUD_RATE, SERIAL_8N1, PIN_MD_RX, PIN_MD_TX);

    UpdateCalibParams(calib_param);
    attachInterrupt(PIN_MD_INDEX, MotorDriver::InterruptForIndex, RISING);
    logger_->Log(LOG_TYPE::INFO, LOG_CLASS::MOTOR_DRIVER, "Motor Driver Setup Completed");
}

MotorDriver::~MotorDriver() {
    EnableDriver(false);
}

CONFIG_SET::CALIB_PARAMS MotorDriver::Calibrate() {
    return CONFIG_SET::CALIB_PARAMS();
}

bool MotorDriver::FulfillRequest(CONFIG_SET::MOTION_REQUEST request) {
    using namespace CONFIG_SET;
    if (is_motor_running_) {
        return false;
    }

    expected_step_ = (float(request.PERCENTAGE) / 100) * calib_params_.TOTAL_STEP_COUNT;

    // Handle 100, 0 for blinds traversals
    blind_traversal_requested_ = request.PERCENTAGE == 100 || request.PERCENTAGE == 0;

    direction_ = expected_step_ > current_step_;
    return true;
}

CONFIG_SET::DRIVER_STATUS MotorDriver::GetStatus() {
    return (is_motor_running_) ? CONFIG_SET::DRIVER_STATUS::BUSY : CONFIG_SET::DRIVER_STATUS::AVAILABLE;
}

bool MotorDriver::EnableDriver(bool enable) {
    digitalWrite(CONFIG_SET::PIN_MD_ENABLE, !enable);
}

void MotorDriver::ResetDriver() {
    using namespace CONFIG_SET;
    logger_->Log(LOG_TYPE::INFO, LOG_CLASS::MOTOR_DRIVER, "Resetting the driver");

    EnableDriver(false);
    delay(25);
    EnableDriver(true);
    delay(25);

    InitializeDriver();
}

void MotorDriver::InitializeDriver() {
    using namespace CONFIG_SET;
    this->begin();
    this->toff(MOTOR_DRIVER_TOFF);
    this->blank_time(MOTOR_DRIVER_BLANK_TIME);
    this->rms_current(MOTOR_DRIVER_RMS_CURRENT);
    this->microsteps(MOTOR_DRIVER_MICROSTEP);
    this->TCOOLTHRS(MOTOR_DRIVER_TCOOL_THRS);  // 20bit max
    this->semin(MOTOR_DRIVER_SE_MIN);
    this->semax(MOTOR_DRIVER_SE_MAX);
    this->shaft(false);
    this->sedn(MOTOR_DRIVER_SEDN);
    this->SGTHRS(calib_params_.STALL_VALUE);
}

bool MotorDriver::CheckFault() {}

void MotorDriver::UpdateCalibParams(CONFIG_SET::CALIB_PARAMS calib_param) {
    calib_params_ = calib_param;
    EnableDriver(true);
    InitializeDriver();
    EnableDriver(false);
}

void MotorDriver::StopMotor(bool soft_traversal) {
    using namespace CONFIG_SET;
    logger_->Log(LOG_TYPE::INFO, LOG_CLASS::MOTOR_DRIVER, "Stopping Motor");
    if (soft_traversal) {
        for (uint32_t i = MOTOR_DRIVER_SPEED; i > 0; i = i - MOTOR_DRIVER_ACCEL) {
            this->VACTUAL(i);
            delay(10);
        }
    } else {
        this->VACTUAL(0);
    }
    EnableDriver(false);
    is_motor_running_ = false;
}

void MotorDriver::StartMotor(bool soft_traversal) {
    using namespace CONFIG_SET;
    logger_->Log(LOG_TYPE::INFO, LOG_CLASS::MOTOR_DRIVER, "Starting Motor");
    EnableDriver(true);
    shaft(calib_params_.DIRECTION ^ direction_);
    if (soft_traversal) {
        for (uint32_t i = 0; i <= MOTOR_DRIVER_SPEED; i = i + MOTOR_DRIVER_ACCEL) {
            this->VACTUAL(i);
            delay(10);
        }
    } else {
        this->VACTUAL(MOTOR_DRIVER_SPEED);
    }
    is_motor_running_ = true;
    last_motor_start_time_sec_ = std::time(nullptr);
}

void MotorDriver::Handle() {
    using namespace CONFIG_SET;
    if (is_motor_running_) {
        bool current_step_out_of_bound = (current_step_ < 0) || (current_step_ > calib_params_.TOTAL_STEP_COUNT);
        if (current_step_out_of_bound) {
            current_step_ = (current_step_ < 0) ? 0 : calib_params_.TOTAL_STEP_COUNT;
        }

        bool reached_destination = false;
        bool step_in_limit = (current_step_ < (1 + STEP_FRACTION_ALLOWANCE) * expected_step_ &&
                              (current_step_ > (1 - STEP_FRACTION_ALLOWANCE) * expected_step_));
        bool step_exceeded_bounds = (direction_ && current_step_ > (1 + STEP_FRACTION_ALLOWANCE) * expected_step_) ||
                                    (!direction_ && current_step_ < (1 - STEP_FRACTION_ALLOWANCE) * expected_step_);

        if (!blind_traversal_requested_ && (step_in_limit || step_exceeded_bounds)) {
            logger_->Log(LOG_TYPE::INFO, LOG_CLASS::MOTOR_DRIVER, "Reached Destination");
            reached_destination = true;
        }

        bool stall_detected = digitalRead(PIN_MD_DIAG);
        bool end_timer_reached = (std::time(nullptr) - last_motor_start_time_sec_) > MOTOR_STOP_TIME_SEC;
        if (stall_detected || reached_destination || end_timer_reached || stop_requested_) {
            StopMotor(false);
            // StopMotor(!stall_detected);
            expected_step_ = current_step_;
            stop_requested_ = false;
        }
    }

    if (!is_motor_running_ && expected_step_ != current_step_) {
        StartMotor(true);
    }
}

void IRAM_ATTR MotorDriver::InterruptForIndex() {
    current_step_ += (direction_) ? full_rot_step_count_ : -full_rot_step_count_;
}

bool MotorDriver::CancelCurrentRequest() {
    using namespace CONFIG_SET;
    logger_->Log(LOG_TYPE::INFO, LOG_CLASS::MOTOR_DRIVER, "Cancelling Request");
    stop_requested_ = is_motor_running_;
}
