/**
 * @file indicator.h
 * @author Mahimana Bhatt (mahimanabhatt@gmail.com)
 * @brief Contains structure for RGB LED (COM 11821), uses FASTLed library for
 * controlling the LED
 * @version 0.1
 * @date 2022-07-19
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

class Indicator {
public:
  /**
   * @brief Construct a new Indicator object
   *
   */
  Indicator();

  /**
   * @brief Destroy the Indicator object
   *
   */
  ~Indicator();

  /**
   * @brief Hello
   *
   */
  void test_function();
};