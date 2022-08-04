/**
 * @file logging.cpp
 * @author Dhiraj Deshmukh (deshmukhdhiraj15@gmail.com)
 * @brief Contains all algorithms for maintaing device logs
 * @version 0.1
 * @date 2022-07-19
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "logging.h"
#include "../config/config.h"
#include <Arduino.h>
#include <mutex>

// mutex for locking access to status, to prevent updation and use of the
// variable at the same time
std::mutex status_mutex;

Logging::Logging() {
  Serial.begin(CONFIG_SET::LOGGING_BAUD_RATE);

  const std::lock_guard<std::mutex> lock(status_mutex);
  this->logging_status_ = false;
}

Logging::~Logging() { Serial.end(); }

bool Logging::log(CONFIG_SET::LOG_TYPE log_type,
                  CONFIG_SET::LOG_CLASS log_class, const char *message) const {
  const std::lock_guard<std::mutex> lock(status_mutex);
  if (this->logging_status_) {
    switch (log_type) {
    case CONFIG_SET::LOG_TYPE::INFO:
      Serial.print("[INFO] ");
      break;
    case CONFIG_SET::LOG_TYPE::WARN:
      Serial.print("[WARN] ");
      break;
    case CONFIG_SET::LOG_TYPE::ERROR:
      Serial.print("[ERROR] ");
      break;
    default:
      Serial.print("[INFO] ");
    }
    switch (log_class) {
    case CONFIG_SET::LOG_CLASS::CONTROLLER:
      Serial.print("[CONTROLLER] ");
      break;
    case CONFIG_SET::LOG_CLASS::CONNECTIVITY:
      Serial.print("[CONNECTIVITY] ");
      break;
    case CONFIG_SET::LOG_CLASS::INDICATOR:
      Serial.print("[INDICATOR] ");
      break;
    case CONFIG_SET::LOG_CLASS::LOGGING:
      Serial.print("[LOGGING] ");
      break;
    case CONFIG_SET::LOG_CLASS::MANUAL_INTERACTION:
      Serial.print("[MANUAL_INTERACTION] ");
      break;
    case CONFIG_SET::LOG_CLASS::MOTOR_DRIVER:
      Serial.print("[MOTOR_DRIVER] ");
      break;
    case CONFIG_SET::LOG_CLASS::STORAGE:
      Serial.print("[STORAGE] ");
      break;
    case CONFIG_SET::LOG_CLASS::ALEXA_INTERACTION:
      Serial.print("[ALEXA_INTERACTION] ");
      break;
    default:
      Serial.print("[LOGGING] ");
    }
    Serial.println(message);
  }
}

void Logging::setLoggingStatus(bool status) {
  const std::lock_guard<std::mutex> lock(status_mutex);
  this->logging_status_ = status;
}

bool Logging::getLoggingStatus() const { return this->logging_status_; }
