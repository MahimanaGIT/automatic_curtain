/**
 * @file manual_interaction.h
 * @author Mahimana Bhatt (mahimanabhatt@gmail.com)
 * @brief Defines structure for manual interaction with device
 * @version 0.1
 * @date 2022-07-19
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef _MAN_INT_INCLUDE_GUARD
#define _MAN_INT_INCLUDE_GUARD

#include <Arduino.h>

#include <chrono>
#include <deque>
#include <memory>
#include <mutex>
#include <new>
#include <thread>
#include <tuple>
#include <utility>

#include "../config/config.h"
#include "../logging/logging.h"

class ManualInteraction {
   public:
    /**
   * @brief  ManualInteraction class constructor
   * Setup pinmode and interrupt
   */
    ManualInteraction(int freq_to_run_deque_analyser, std::shared_ptr<Logging>& logging);

    /**
   * @brief  ManualInteraction class destructor
   *  Destroy the ManualInteraction object
   */
    ~ManualInteraction();

    /**
   * @brief  Hardware Interrrupt for up [red colored] button
   * check for valid interrupt by software debouncing,switch state cheking
   * and adds it to the button deque up
   */
    static void s_IntrAddToButtonDequeUp();

    /**
   * @brief  Hardware Interrrupt for down [black colored] button
   * check for valid interrupt by software debouncing,switch state cheking
   * and adds it to the button deque down
   */
    static void s_IntrAddToButtonDequeDown();

    /**
   * @brief Stop button deque analyser function
   * set_button_deque_analyser varible to ture so it stops the threaded
   * function
   */
    void StopButtonDequeAnalyserFn();

    /**
   * @brief start button deque analyser function thread
   * start button deque analyser function thread
   */
    void StartButtonDequeAnalyserFn();

    /**
   * @brief Get the Manual Action And Time object
   *
   * @return std::tuple<CONFIG_SET::MANUAL_PUSH, CONFIG_SET::time_var>
   */
    std::tuple<CONFIG_SET::MANUAL_PUSH, CONFIG_SET::time_var> GetManualActionAndTime();

   private:
    /**
   * @brief thread function to check the deque status and identify the manual
   * interaction mode
   * Function will check state and time of occurance for objects in deque for
   * determining mode
   */
    void ButtonstateDequeAnalyser();

    /**
   * @brief Set the CurrentButtonState object
   * Function to sets the current buttonstate [0 ->NO_PUSH, 1->LONGPRESS,
   * 2->DOUBLETAP]
   *
   * @param[in] button_press_deque_ deque to analyse and set button_state
   * @param[out] button_state refrence to button_state passed
   * @param[out] time refrence to time variable for change of button_state
   */
    void SetCurrentButtonState(std::deque<std::pair<bool, CONFIG_SET::time_var>>& button_press_deque_,
                               CONFIG_SET::BUTTON_PRESS& button_state, CONFIG_SET::time_var& time);

    /**
   * @brief Set the Manual Action And Time object
   * sets manual_action_ and manual_action_time_
   *
   * @param[in] button_1_state current button state 1 [red]
   * @param[in] button_2_state current button state 2 [black]
   * @param[in] time_button_1 time the current button state 1 was set
   * @param[in] time_button_2 time the current button state 2 was set
   */
    void SetManualActionAndTime(CONFIG_SET::BUTTON_PRESS& button_1_state, CONFIG_SET::BUTTON_PRESS& button_2_state,
                                CONFIG_SET::time_var& time_button_1, CONFIG_SET::time_var& time_button_2);

    CONFIG_SET::MANUAL_PUSH manual_action_ = CONFIG_SET::MANUAL_PUSH::NO_PUSH;
    CONFIG_SET::time_var manual_action_time_;
    bool stop_button_deque_analyser_ = true;
    std::unique_ptr<std::thread> deque_analyser_{nullptr};
    std::shared_ptr<Logging> logger_{nullptr};
    const int freq_to_run_deque_analyser_;
    int delay_to_run_deque_analyser_;

    static std::deque<std::pair<bool, CONFIG_SET::time_var>> s_button_state_deque_up_;
    static std::deque<std::pair<bool, CONFIG_SET::time_var>> s_button_state_deque_down_;
    static bool s_class_setup_flag_;
    static std::mutex s_deque_mutex_;
};

#endif
