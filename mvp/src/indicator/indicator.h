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

#ifndef _INDICATOR_INCLUDE_GUARD
#define _INDICATOR_INCLUDE_GUARD

#include "../config/config.h"

class Indicator {
public:
  /**
   * @brief Fetches the LED pin from config, setups pin mode, setup WS2812
   * RGBLED
   *
   */
  Indicator();

  /**
   * @brief Destroy the Indicator object
   *
   */
  ~Indicator();

  /**
   * @brief Updates the color of LED by using mapping from status to color of
   * LED
   *
   * @param status: Device Status to be used for updation
   * @return true: if the updation was successful
   * @return false: otherwise
   */
  bool updateStatus(CONFIG_SET::DEVICE_STATUS status);
};

#endif
