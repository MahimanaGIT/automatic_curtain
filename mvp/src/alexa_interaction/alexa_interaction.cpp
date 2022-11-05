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

#include <mutex>
#include <string>
#include <tuple>

#include "../config/config.h"
#include "../logging/logging.h"
#include "fauxmoESP.h"

std::mutex alexa_request_mutex_;
// declaring static variables to be used by callback
bool AlexaInteraction::is_new_request_available_ = false;
CONFIG_SET::MOTION_REQUEST AlexaInteraction::latest_alexa_request_ = CONFIG_SET::MOTION_REQUEST();
String AlexaInteraction::device_id_ = CONFIG_SET::DEFAULT_DEVICE_ID;

AlexaInteraction::AlexaInteraction(std::shared_ptr<Logging> logging, String device_id) : logger_(logging), fauxmoESP() {
    device_id_ = device_id;
    this->setPort(80);
    this->enable(true);
    this->addDevice(device_id_.c_str());
    this->onSetState(Callback);
}

void AlexaInteraction::Callback(unsigned char id, const char* device_name, bool state, unsigned char percentage) {
    if (strcmp(device_name, AlexaInteraction::device_id_.c_str()) == 0) {
        std::unique_lock<std::mutex> lock(alexa_request_mutex_);
        AlexaInteraction::latest_alexa_request_.PERCENTAGE = (float(percentage) / 255) * 100;
        if (percentage == 255 || percentage == 254) {
            latest_alexa_request_.PERCENTAGE = 100;
        }
        if (!state) {
            latest_alexa_request_.PERCENTAGE = 0;
        }
        AlexaInteraction::is_new_request_available_ = true;
    }
}

AlexaInteraction::~AlexaInteraction() {}

std::tuple<bool, CONFIG_SET::MOTION_REQUEST> AlexaInteraction::GetAlexaRequest() {
    std::unique_lock<std::mutex> lock(alexa_request_mutex_);
    std::tuple<bool, CONFIG_SET::MOTION_REQUEST> return_value(is_new_request_available_, latest_alexa_request_);
    is_new_request_available_ = false;
    return return_value;
}

void AlexaInteraction::HandleFauxmo() {
    this->handle();
}

void AlexaInteraction::SetState(CONFIG_SET::MOTION_REQUEST request) {
    fauxmoESP::setState(device_id_.c_str(), request.PERCENTAGE != 0, request.PERCENTAGE * 2.55);
}