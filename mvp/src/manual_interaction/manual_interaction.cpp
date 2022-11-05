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
#include <memory>
#include "../config/config.h"
#include "../logging/logging.h"

#include <Arduino.h>

#include <chrono>
#include <deque>
#include <memory>
#include <mutex>
#include <new>
#include <tuple>
#include <utility>

#include "../config/config.h"

std::mutex MI_mutex;

//@{
/**
 * @brief Initialize static variables of class
 */
bool MI_Cls::class_setup_flag_ = false;
C_S::button_press_deque MI_Cls::button_state_deque_up_;
C_S::button_press_deque MI_Cls::button_state_deque_down_;
const int MI_Cls::pin_button_up_ = C_S::PIN_BUTTON_UP;
const int MI_Cls::pin_button_down_ = C_S::PIN_BUTTON_DOWN;
//@}

//// public funtions of class : -------------- start
///----------------------------------------- >

MI_Cls::ManualInteraction() {
    //@{
    /**
   * @brief Define hardware interrupt for rising and falling edge of manual
   * buttons and starting the thread for button deque analyser
   */
    if (MI_Cls::class_setup_flag_ == false) {
        pinMode(MI_Cls::pin_button_up_, INPUT);
        pinMode(MI_Cls::pin_button_down_, INPUT);
        attachInterrupt(digitalPinToInterrupt(MI_Cls::pin_button_up_), MI_Cls::IntrAddToButtonDequeUp, CHANGE);
        attachInterrupt(digitalPinToInterrupt(MI_Cls::pin_button_down_), MI_Cls::IntrAddToButtonDequeDown, CHANGE);
        this->StartButtonDequeAnalyserFn();
        MI_Cls::class_setup_flag_ == true;
    }
    //@}
}

MI_Cls::~ManualInteraction() {
    this->StopButtonDequeAnalyserFn();
}

void MI_Cls::IntrAddToButtonDequeUp() {
    //@{
    /**
   * @brief checking the state of button and adding the valid state to up
   * button deque
   */

    // setting a mutex lock and reading current button state
    std::lock_guard<std::mutex> lock(MI_mutex);
    int state = digitalRead(MI_Cls::pin_button_up_);

    // checking for condition of empty deque
    if (MI_Cls::button_state_deque_up_.empty()) {
        MI_Cls::button_state_deque_up_.push_back(std::make_pair(state, C_S::current_time::now()));
    } else if ((std::chrono::duration_cast<std::chrono::milliseconds>(
                    // current state is not same as previous state and for
                    // debouncing time
                    C_S::current_time::now() - MI_Cls::button_state_deque_up_.back().second)
                    .count() > 50) &&
               (MI_Cls::button_state_deque_up_.back().first != state)) {
        MI_Cls::button_state_deque_up_.push_back(std::make_pair(state, C_S::current_time::now()));
    }
    //@}
}

void MI_Cls::IntrAddToButtonDequeDown() {
    //@{
    /**
   * @brief checking the state of button and adding the valid state to down
   * button deque
   */

    // setting a mutex lock and reading current button state
    std::lock_guard<std::mutex> lock(MI_mutex);
    int state = digitalRead(MI_Cls::pin_button_down_);

    // checking for condition of empty deque
    if (MI_Cls::button_state_deque_down_.empty()) {
        MI_Cls::button_state_deque_down_.push_back(std::make_pair(state, C_S::current_time::now()));
    }
    // current state is not same as previous state and for debouncing time
    else if ((std::chrono::duration_cast<std::chrono::milliseconds>(C_S::current_time::now() -
                                                                    MI_Cls::button_state_deque_down_.back().second)
                  .count() > 50) &&
             (MI_Cls::button_state_deque_down_.back().first != state)) {
        MI_Cls::button_state_deque_down_.push_back(std::make_pair(state, C_S::current_time::now()));
    }
    //@}
}

void MI_Cls::StopButtonDequeAnalyserFn() {
    //@{
    /**
   * @brief set the flag to stop the ButtonstateDequeAnalyser function
   */

    this->stop_button_deque_analyser_ = false;
    //@}
}

void MI_Cls::StartButtonDequeAnalyserFn() {
    //@{
    /**
   * @brief starts the ButtonstateDequeAnalyser function in a thread
   */
    this->m_deque_analyser_.reset(new std::thread(&MI_Cls::ButtonstateDequeAnalyser, this));
    //@}
}

std::tuple<C_S::MANUAL_PUSH, C_S::time_var> MI_Cls::GetManualActionAndTime() {
    //@{
    /**
   * @brief returns manual action and time the action was set
   */

    // return the manual action and time of action
    std::tuple<C_S::MANUAL_PUSH, C_S::time_var> return_manual_action_and_time =
        std::make_tuple(this->manual_action_, this->manual_action_time_);

    // clear the acrion based on flag
    if (this->manual_action_ == C_S::MANUAL_PUSH::DOUBLE_TAP_BOTH ||
        this->manual_action_ == C_S::MANUAL_PUSH::DOUBLE_TAP_UP ||
        this->manual_action_ == C_S::MANUAL_PUSH::DOUBLE_TAP_DOWN) {
        this->manual_action_ = C_S::MANUAL_PUSH::NO_PUSH;
        this->manual_action_time_ = C_S::current_time::now();
    }
    return return_manual_action_and_time;

    //@}
}

//// public funtions of class : -------------- end
///----------------------------------------- <

//// Private funtions of class : -------------- start
///----------------------------------------- >

void MI_Cls::ButtonstateDequeAnalyser() {
    //@{
    /**
   * @brief its a structure function which defines initial button state's and
   * periodically calls helper functions to get current button states and set
   * manual action and trigger time
   */

    // Declaring variables to hold button states
    // [0->NO_PUSH,1->LONG_PRESS,2->DOUBLE_TAP] and time the state was trigerred
    int button_1_state = 0;
    int button_2_state = 0;
    C_S::time_var time_button_1 = C_S::current_time::now();
    C_S::time_var time_button_2 = C_S::current_time::now();

    // Continous loop until the deuqe analyser flag is true
    while (this->stop_button_deque_analyser_) {
        C_S::time_var function_start_time = C_S::current_time::now();

        // calling  function to queue for up button
        if (!MI_Cls::button_state_deque_up_.empty()) {
            this->SETCurrentButtonState(button_state_deque_up_, button_1_state, time_button_1);
        }
        // calling  function to queue for down button
        if (!MI_Cls::button_state_deque_down_.empty()) {
            this->SETCurrentButtonState(button_state_deque_down_, button_2_state, time_button_2);
        }

        // calling function to set manual action based on button states
        this->SetManualActionAndTime(button_1_state, button_2_state, time_button_1, time_button_2);

        C_S::time_var function_end_time = C_S::current_time::now();

        int execution_time =
            std::chrono::duration_cast<std::chrono::milliseconds>(function_end_time - function_start_time).count();

        // non-blocking delay for running loop 1HZ
        if (execution_time < 500) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500 - execution_time));
        }
    }

    //@}
}

void MI_Cls::SETCurrentButtonState(C_S::button_press_deque& button_press_deque_, int& button_state,
                                   C_S::time_var& time) {
    //@{
    /**
   * @brief analyses the current state of button press based on deque
   */

    // check if button_1 long is pressed:
    if (button_press_deque_.back().first == 1 && std::chrono::duration_cast<std::chrono::milliseconds>(
                                                     C_S::current_time::now() - button_press_deque_.back().second)
                                                         .count() > 1100) {
        button_state = 1;
        time = button_press_deque_.back().second;
        button_press_deque_.clear();
    }

    // check if long press was cleared
    if (button_state == 1 && button_press_deque_.size() > 0) {
        time = button_press_deque_.front().second;
        button_press_deque_.pop_front();
        button_state = 0;
    }

    // Check if double press was in the queue
    while (button_press_deque_.size() >= 4) {
        if (std::chrono::duration_cast<std::chrono::milliseconds>(button_press_deque_[3].second -
                                                                  button_press_deque_.front().second)
                .count() < 1000) {
            time = button_press_deque_.front().second;
            button_press_deque_.erase(button_press_deque_.begin(), button_press_deque_.begin() + 4);
            button_state = 2;
        } else {
            button_press_deque_.erase(button_press_deque_.begin(), button_press_deque_.begin() + 2);
        }
    }

    //@}
}

void MI_Cls::SetManualActionAndTime(int& button_1_state, int& button_2_state, C_S::time_var& time_button_1,
                                    C_S::time_var& time_button_2) {
    //@{
    /**
   * @brief based on button states [0->NO_PUSH,1->LONG_PRESS,2->DOUBLE_TAP]
   * setting manual action and time of action
   */
    if (button_1_state == 1 && button_2_state == 1) {
        this->manual_action_ = C_S::MANUAL_PUSH::LONG_PRESS_BOTH;
        this->manual_action_time_ = time_button_1 > time_button_2 ? time_button_1 : time_button_2;
    } else if (button_1_state == 1 && button_2_state == 0) {
        this->manual_action_ = C_S::MANUAL_PUSH::LONG_PRESS_UP;
        this->manual_action_time_ = time_button_1;
    } else if (button_1_state == 0 && button_2_state == 1) {
        this->manual_action_ = C_S::MANUAL_PUSH::LONG_PRESS_DOWN;
        this->manual_action_time_ = time_button_2;
    } else if (button_1_state == 2 && button_2_state == 0) {
        this->manual_action_ = C_S::MANUAL_PUSH::DOUBLE_TAP_UP;
        this->manual_action_time_ = time_button_1;
        button_1_state = 0;
    } else if (button_1_state == 0 && button_2_state == 2) {
        this->manual_action_ = C_S::MANUAL_PUSH::DOUBLE_TAP_DOWN;
        this->manual_action_time_ = time_button_2;
        button_2_state = 0;
    } else if (button_1_state == 2 && button_2_state == 2) {
        this->manual_action_ = C_S::MANUAL_PUSH::DOUBLE_TAP_BOTH;
        this->manual_action_time_ = time_button_1 > time_button_2 ? time_button_1 : time_button_2;
        button_1_state = 0;
        button_2_state = 0;
    } else if (button_1_state == 0 && button_2_state == 0 &&
               this->manual_action_ != C_S::MANUAL_PUSH::DOUBLE_TAP_BOTH &&
               this->manual_action_ != C_S::MANUAL_PUSH::DOUBLE_TAP_UP &&
               this->manual_action_ != C_S::MANUAL_PUSH::DOUBLE_TAP_DOWN) {
        this->manual_action_ = C_S::MANUAL_PUSH::NO_PUSH;
        this->manual_action_time_ = time_button_1 > time_button_2 ? time_button_1 : time_button_2;
        button_1_state = 0;
        button_2_state = 0;
    }
    //@}
}

//// Private funtions of class : -------------- end
///----------------------------------------- <
