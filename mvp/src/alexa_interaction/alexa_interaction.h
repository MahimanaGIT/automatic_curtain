/**
 * @file alexa_interaction.h
 * @author Mahimana Bhatt (mahimanabhatt@gmail.com)
 * @brief Defines structure for class which will be interacting with alexa API
 * @version 0.1
 * @date 2022-07-20
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef _ALEXA_INT_INCLUDE_GUARD
#define _ALEXA_INT_INCLUDE_GUARD

#include "../config/config.h"
#include "../logging/logging.h"
#include "fauxmoESP.h"
#include <mutex>
#include <sstream>
#include <string>
#include <tuple>

class AlexaInteraction {
public:
  /**
   * @brief Construct a new Alexa Interaction object, default constructor
   *
   */
  AlexaInteraction();

  /**
   * @brief Construct a new AlexaInteraction object, initializes fauxmoesp
   *
   */
  AlexaInteraction(Logging *, String);

  /**
   * @brief Destroy the AlexaInteraction object
   *
   */
  ~AlexaInteraction();

  /**
   * @brief Get the latest Alexa Request object
   *
   * @return std::tuple<bool, CONFIG_SET::ALEXA_REQUEST>
   */
  std::tuple<bool, CONFIG_SET::ALEXA_REQUEST> getAlexaRequest();

  void handleFauxmo();

private:
  Logging *logger_;
  static String device_id_;
  static bool is_new_request_available_;
  static CONFIG_SET::ALEXA_REQUEST latest_alexa_request_;

  /**
   * @brief Gets called by fauxmo esp when a alexa calls the device
   *
   * @param id: device id number
   * @param device_name: device name
   * @param state: on/off bool
   * @param percentage: 0-100 percentage
   */
  static void callback(unsigned char id, const char *device_name, bool state,
                       unsigned char percentage);
};

#endif
