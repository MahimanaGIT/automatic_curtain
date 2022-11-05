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
    //! ManualInteraction class constructor
    /*!
    @brief Setup pinmode and interrupt
  */
    ManualInteraction();

    //! ManualInteraction class constructor
    /*!
    @brief  Destroy the ManualInteraction object
  */
    ~ManualInteraction();

    //! Hardware Interrupt for up(red colored) button
    /*!
    @brief check for valid interrupt by software debouncing,switch state cheking
    and adds it to the button deque down
  */
    static void IntrAddToButtonDequeUp();

    //! Hardware Interrupt for down(black colored) button
    /*!
    @brief check for valid interrupt by software debouncing,switch state cheking
    and adds it to the button deque down
  */
    static void IntrAddToButtonDequeDown();

    //! Stop button deque analyser function
    /*!
    @brief set_button_deque_analyser varible to ture so it stops the threaded
    function
  */
    void StopButtonDequeAnalyserFn();

    //! start button deque analyser function thread
    /*!
    @brief start button deque analyser function thread
  */
    void StartButtonDequeAnalyserFn();

    //! Return manual_action
    /*!
    @brief Return value for manual_action and time with option of clearing the
    variable
  */
    std::tuple<C_S::MANUAL_PUSH, C_S::time_var> GetManualActionAndTime();

    //@{
    /**
   * @brief class static varibles
   */
    static C_S::button_press_deque button_state_deque_up_;
    static C_S::button_press_deque button_state_deque_down_;
    static const int pin_button_up_;
    static const int pin_button_down_;
    static bool class_setup_flag_;
    //@}

   private:
    //! thread function to check the deque status and identify the manual
    //! interaction mode
    /*!
    @brief Function will check state and time of occurance for objects in deque
    for determining mode
  */
    void ButtonstateDequeAnalyser();

    //! Function to get the current buttonstate
    /*!
    @brief Function to sets the current buttonstate [0 ->NO_PUSH, 1->LONGPRESS,
    2->DOUBLETAP]
  */
    void SETCurrentButtonState(C_S::button_press_deque& button_press_deque_, int& button_state, C_S::time_var& time);

    //! set manual button action based on individual button state
    /*!
    @brief set manual button action based on individual button state
  */
    void SetManualActionAndTime(int& button_1_state, int& button_2_state, C_S::time_var& time_button_1,
                                C_S::time_var& time_button_2);

    //@{
    /**
   * @brief private class varibles to communicate button action
   */
    C_S::MANUAL_PUSH manual_action_ = C_S::MANUAL_PUSH::NO_PUSH;
    C_S::time_var manual_action_time_;
    bool stop_button_deque_analyser_ = true;
    std::unique_ptr<std::thread> m_deque_analyser_{nullptr};
    //@}
};

using MI_Cls = ManualInteraction;

#endif
