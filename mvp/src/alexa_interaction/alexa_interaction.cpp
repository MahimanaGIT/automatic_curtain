/**
 * @file alexa_interaction.cpp
 * @author Dhiraj Deshmukh (deshmukhdhiraj15@gmail.com)
 * @brief Contains algorithms for handling alexa requests and make appropriate
 * call at every request
 * @version 0.1
 * @date 2022-07-20
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "alexa_interaction.h"
#include "../config/config.h"
#include "../logging/logging.h"
#include "fauxmoESP.h"
#include <mutex>
#include <string>
#include <tuple>

std::mutex alexa_request_mutex_;
fauxmoESP fauxmo_;

bool AlexaInteraction::is_new_request_available_ = true;
CONFIG_SET::ALEXA_REQUEST AlexaInteraction::latest_alexa_request_ =
    CONFIG_SET::ALEXA_REQUEST();
String AlexaInteraction::device_id_ = CONFIG_SET::DEFAULT_DEVICE_ID;

AlexaInteraction::AlexaInteraction() {}

AlexaInteraction::AlexaInteraction(Logging *logging, String device_id)
    : logger_(logging) {
  device_id_ = device_id;
  fauxmo_.setPort(80);
  fauxmo_.enable(true);
  fauxmo_.addDevice(device_id_.c_str());
  fauxmo_.onSetState(callback);
}

void AlexaInteraction::callback(unsigned char id, const char *device_name,
                                bool state, unsigned char percentage) {
  if (strcmp(device_name, AlexaInteraction::device_id_.c_str()) == 0) {
    std::unique_lock<std::mutex> lock(alexa_request_mutex_);
    AlexaInteraction::latest_alexa_request_.STATE = state;
    AlexaInteraction::latest_alexa_request_.PERCENTAGE =
        (float(percentage) / 255) * 100;
    AlexaInteraction::is_new_request_available_ = true;
  }
}

AlexaInteraction::~AlexaInteraction() {}

std::tuple<bool, CONFIG_SET::ALEXA_REQUEST>
AlexaInteraction::getAlexaRequest() {
  std::unique_lock<std::mutex> lock(alexa_request_mutex_);
  std::tuple<bool, CONFIG_SET::ALEXA_REQUEST> return_value(
      is_new_request_available_, latest_alexa_request_);
  is_new_request_available_ = false;
  return return_value;
}

void AlexaInteraction::handleFauxmo() { fauxmo_.handle(); }