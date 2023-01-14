/**
 * @file manual_interaction.cpp
 * @author Dhiraj Deshmukh (deshmukhdhiraj15@gmail.com)
 * @brief Contains algorithms to detect a users manual interaction with the
 * device, specifically push buttons
 * @version 0.1
 * @date 2022-07-19
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "manual_interaction.h"

#include <Arduino.h>

#include <chrono>
#include <deque>
#include <memory>
#include <mutex>
#include <new>
#include <tuple>
#include <utility>

#include "../config/config.h"
#include "../logging/logging.h"

bool ManualInteraction::s_class_setup_flag_ = false;
std::deque<std::pair<bool, CONFIG_SET::time_var>> ManualInteraction::s_button_state_deque_up_;
std::deque<std::pair<bool, CONFIG_SET::time_var>> ManualInteraction::s_button_state_deque_down_;
std::mutex ManualInteraction::s_deque_mutex_;

ManualInteraction::ManualInteraction(int freq_to_run_deque_analyser, std::shared_ptr<Logging>& logging)
    : freq_to_run_deque_analyser_(freq_to_run_deque_analyser), logger_(logging) {

    // Importing namespace for config
    using namespace CONFIG_SET;

    logger_->Log(LOG_TYPE::INFO, LOG_CLASS::MANUAL_INTERACTION, "Manual Interaction intilization started");
    if (s_class_setup_flag_ == false) {
        pinMode(PIN_BUTTON_UP, INPUT);
        pinMode(PIN_BUTTON_DOWN, INPUT);
        attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_UP), s_IntrAddToButtonDequeUp, CHANGE);
        attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_DOWN), s_IntrAddToButtonDequeDown, CHANGE);
        StartButtonDequeAnalyserFn();
        s_class_setup_flag_ = true;
        delay_to_run_deque_analyser_ = 1000 / 2;
        logger_->Log(LOG_TYPE::INFO, LOG_CLASS::MANUAL_INTERACTION, "Manual Interaction intilization completed");
    } else {
        logger_->Log(LOG_TYPE::INFO, LOG_CLASS::MANUAL_INTERACTION,
                     "s_class_setup_flag_ = true did not initialize Manual Interaction");
    }
}

ManualInteraction::~ManualInteraction() {
    // Importing namespace for config
    using namespace CONFIG_SET;

    logger_->Log(LOG_TYPE::INFO, LOG_CLASS::MANUAL_INTERACTION, "Manual Interaction object destroyed");
    StopButtonDequeAnalyserFn();
}

void ManualInteraction::s_IntrAddToButtonDequeUp() {
    // Importing namespace for config
    using namespace CONFIG_SET;

    // setting a mutex lock and reading current button state
    std::lock_guard<std::mutex> lock(s_deque_mutex_);
    int state = digitalRead(PIN_BUTTON_UP);

    if (s_button_state_deque_up_.empty()) {
        // checking for condition of empty deque
        s_button_state_deque_up_.push_back(std::make_pair(state, current_time::now()));
    } else if ((std::chrono::duration_cast<std::chrono::milliseconds>(current_time::now() -
                                                                      s_button_state_deque_up_.back().second)
                    .count() > 50) &&
               (s_button_state_deque_up_.back().first != state)) {
        // current state is not same as previous state and for
        // debouncing time
        s_button_state_deque_up_.push_back(std::make_pair(state, current_time::now()));
    }
}

void ManualInteraction::s_IntrAddToButtonDequeDown() {
    // Importing namespace for config
    using namespace CONFIG_SET;

    // setting a mutex lock and reading current button state
    std::lock_guard<std::mutex> lock(s_deque_mutex_);
    int state = digitalRead(PIN_BUTTON_DOWN);

    if (s_button_state_deque_down_.empty()) {
        s_button_state_deque_down_.push_back(
            // checking for condition of empty deque
            std::make_pair(state, current_time::now()));
    } else if ((std::chrono::duration_cast<std::chrono::milliseconds>(current_time::now() -
                                                                      s_button_state_deque_down_.back().second)
                    .count() > 50) &&
               (s_button_state_deque_down_.back().first != state)) {
        s_button_state_deque_down_.push_back(
            // current state is not same as previous state and for debouncing time
            std::make_pair(state, current_time::now()));
    }
}

void ManualInteraction::StopButtonDequeAnalyserFn() {
    // set the flag to stop the ButtonstateDequeAnalyser function
    stop_button_deque_analyser_ = false;
}

void ManualInteraction::StartButtonDequeAnalyserFn() {
    // Importing namespace for config
    using namespace CONFIG_SET;

    // starts the ButtonstateDequeAnalyser function in a thread
    deque_analyser_.reset(new std::thread(&ManualInteraction::ButtonstateDequeAnalyser, this));
    logger_->Log(LOG_TYPE::INFO, LOG_CLASS::MANUAL_INTERACTION,
                 "Manual interaction class function to analyse button press started");
}

std::tuple<CONFIG_SET::MANUAL_PUSH, CONFIG_SET::time_var> ManualInteraction::GetManualActionAndTime() {
    // Importing namespace for config
    using namespace CONFIG_SET;

    // return the manual action and time of action
    std::tuple<MANUAL_PUSH, time_var> return_manual_action_and_time =
        std::make_tuple(manual_action_, manual_action_time_);

    // clear the acrion based on flag
    if (manual_action_ == MANUAL_PUSH::DOUBLE_TAP_BOTH || manual_action_ == MANUAL_PUSH::DOUBLE_TAP_UP ||
        manual_action_ == MANUAL_PUSH::DOUBLE_TAP_DOWN) {
        manual_action_ = MANUAL_PUSH::NO_PUSH;
        manual_action_time_ = current_time::now();
    }
    return return_manual_action_and_time;
}

void ManualInteraction::ButtonstateDequeAnalyser() {
    // Importing namespace for config
    using namespace CONFIG_SET;

    // Declaring variables to hold button states
    // [0->NO_PUSH,1->LONG_PRESS,2->DOUBLE_TAP] and time the state was
    // trigerred
    BUTTON_PRESS button_1_state = BUTTON_PRESS::NO_PUSH;
    BUTTON_PRESS button_2_state = BUTTON_PRESS::NO_PUSH;
    time_var time_button_1 = current_time::now();
    time_var time_button_2 = current_time::now();

    // Continous loop until the deuqe analyser flag is true
    while (stop_button_deque_analyser_) {
        time_var function_start_time = current_time::now();

        // calling  function to queue for up button
        if (!s_button_state_deque_up_.empty()) {
            SetCurrentButtonState(s_button_state_deque_up_, button_1_state, time_button_1);
        }
        // calling  function to queue for down button
        if (!s_button_state_deque_down_.empty()) {
            SetCurrentButtonState(s_button_state_deque_down_, button_2_state, time_button_2);
        }

        // calling function to set manual action based on button states
        SetManualActionAndTime(button_1_state, button_2_state, time_button_1, time_button_2);

        time_var function_end_time = current_time::now();

        int execution_time =
            std::chrono::duration_cast<std::chrono::milliseconds>(function_end_time - function_start_time).count();

        // non-blocking delay for running loop 2HZ
        if (execution_time < delay_to_run_deque_analyser_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(delay_to_run_deque_analyser_ - execution_time));
        }
    }
}

void ManualInteraction::SetCurrentButtonState(std::deque<std::pair<bool, CONFIG_SET::time_var>>& button_press_deque_,
                                              CONFIG_SET::BUTTON_PRESS& button_state, CONFIG_SET::time_var& time) {
    // Importing namespace for config
    using namespace CONFIG_SET;

    std::lock_guard<std::mutex> lock(s_deque_mutex_);
    // check if button_1 long is pressed:
    if (button_press_deque_.back().first == 1 &&
        std::chrono::duration_cast<std::chrono::milliseconds>(current_time::now() - button_press_deque_.back().second)
                .count() > 1100) {
        button_state = BUTTON_PRESS::LONG_PRESS;
        time = button_press_deque_.back().second;
        button_press_deque_.clear();
    }

    // check if long press was cleared
    if (button_state == BUTTON_PRESS::LONG_PRESS && button_press_deque_.size() > 0) {
        time = button_press_deque_.front().second;
        button_press_deque_.pop_front();
        button_state = BUTTON_PRESS::NO_PUSH;
    }

    // Check if double press was in the queue
    while (button_press_deque_.size() >= 4) {
        if (std::chrono::duration_cast<std::chrono::milliseconds>(button_press_deque_[3].second -
                                                                  button_press_deque_.front().second)
                .count() < 1000) {
            time = button_press_deque_.front().second;
            button_press_deque_.erase(button_press_deque_.begin(), button_press_deque_.begin() + 4);
            button_state = BUTTON_PRESS::DOUBLE_TAP;
        } else {
            button_press_deque_.erase(button_press_deque_.begin(), button_press_deque_.begin() + 2);
        }
    }
}

void ManualInteraction::SetManualActionAndTime(CONFIG_SET::BUTTON_PRESS& button_1_state,
                                               CONFIG_SET::BUTTON_PRESS& button_2_state,
                                               CONFIG_SET::time_var& time_button_1,
                                               CONFIG_SET::time_var& time_button_2) {
    // Importing namespace for config
    using namespace CONFIG_SET;

    if (button_1_state == BUTTON_PRESS::LONG_PRESS && button_2_state == BUTTON_PRESS::LONG_PRESS) {
        manual_action_ = MANUAL_PUSH::LONG_PRESS_BOTH;
        manual_action_time_ = time_button_1 > time_button_2 ? time_button_1 : time_button_2;
    } else if (button_1_state == BUTTON_PRESS::LONG_PRESS && button_2_state == BUTTON_PRESS::NO_PUSH) {
        manual_action_ = MANUAL_PUSH::LONG_PRESS_UP;
        manual_action_time_ = time_button_1;
    } else if (button_1_state == BUTTON_PRESS::NO_PUSH && button_2_state == BUTTON_PRESS::LONG_PRESS) {
        manual_action_ = MANUAL_PUSH::LONG_PRESS_DOWN;
        manual_action_time_ = time_button_2;
    } else if (button_1_state == BUTTON_PRESS::DOUBLE_TAP && button_2_state == BUTTON_PRESS::NO_PUSH) {
        manual_action_ = MANUAL_PUSH::DOUBLE_TAP_UP;
        manual_action_time_ = time_button_1;
        button_1_state = BUTTON_PRESS::NO_PUSH;
    } else if (button_1_state == BUTTON_PRESS::NO_PUSH && button_2_state == BUTTON_PRESS::DOUBLE_TAP) {
        manual_action_ = MANUAL_PUSH::DOUBLE_TAP_DOWN;
        manual_action_time_ = time_button_2;
        button_2_state = BUTTON_PRESS::NO_PUSH;
    } else if (button_1_state == BUTTON_PRESS::DOUBLE_TAP && button_2_state == BUTTON_PRESS::DOUBLE_TAP) {
        manual_action_ = MANUAL_PUSH::DOUBLE_TAP_BOTH;
        manual_action_time_ = time_button_1 > time_button_2 ? time_button_1 : time_button_2;
        button_1_state = BUTTON_PRESS::NO_PUSH;
        button_2_state = BUTTON_PRESS::NO_PUSH;
    } else if (button_1_state == BUTTON_PRESS::NO_PUSH && button_2_state == BUTTON_PRESS::NO_PUSH &&
               manual_action_ != MANUAL_PUSH::DOUBLE_TAP_BOTH && manual_action_ != MANUAL_PUSH::DOUBLE_TAP_UP &&
               manual_action_ != MANUAL_PUSH::DOUBLE_TAP_DOWN) {
        manual_action_ = MANUAL_PUSH::NO_PUSH;
        manual_action_time_ = time_button_1 > time_button_2 ? time_button_1 : time_button_2;
        button_1_state = BUTTON_PRESS::NO_PUSH;
        button_2_state = BUTTON_PRESS::NO_PUSH;
    }
}
