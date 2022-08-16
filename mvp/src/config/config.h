/**
 * @file config.h
 * @author Mahimana Bhatt (mahimanabhatt@gmail.com)
 * @brief This header file contains all the constants and enum that will be used
 * throughout the repository like specific pins, mapping from state to led color
 * etc.
 * @version 0.1
 * @date 2022-07-19
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef _CONFIG_INCLUDE_GUARD
#define _CONFIG_INCLUDE_GUARD

#include <Arduino.h>
#include <iostream>

/**
 * @brief Namespace to be used for constants/enums
 *
 */
namespace CONFIG_SET {
const int LOGGING_BAUD_RATE = 115200;
const std::string STORAGE_NAMESPACE = "madac";

// Vars For Indicator Class
const int NUMBER_OF_LEDS = 1;
const int LED_BRIGHTNESS = 25;

/*
****** STORAGE KEYS ******
These keys are used by Storage class to retrieve the data from FLASHs
*/
const std::string KEY_SSID = "ssid";
const std::string KEY_PASSWORD = "password";
const std::string KEY_DEVICE_ID = "deviceID";
const std::string KEY_TOTAL_STEP_COUNT = "totalStepCount";
const std::string KEY_STALL_VALUE = "stallValue";
const String DEFAULT_DEVICE_ID = "madac_blinds";

enum class OPERATION_MODE {
  START,
  RESET,
  MAINTENANCE,
};

/*
****** ESP32 PINS ******
These are hardware ESP32 pin number where each external pin/component is
connected
PIN_RGB_LED -> RGB LED WS2812
PIN_BUTTON_UP -> Up button
PIN_BUTTON_DOWN -> Down button

Motor Driver (TMC2209) Pins
PIN_MD_ENABLE -> Enable
PIN_MD_RX -> Serial Receiver
PIN_MD_TX -> Serial Transmitter
PIN_MD_MS1 -> MS1
PIN_MD_MS2 -> MS2
PIN_MD_STEP -> Step
PIN_MD_DIR -> Dir
PIN_MD_INDEX -> Index
PIN_MD_DIAG -> Diag (Stall)
*/
enum class PINS {
  PIN_RGB_LED = 32,
  PIN_BUTTON_UP = 33,
  PIN_BUTTON_DOWN = 25,
  PIN_MD_ENABLE = 4,
  PIN_MD_RX = 16,
  PIN_MD_TX = 17,
  PIN_MD_MS1 = 5,
  PIN_MD_MS2 = 18,
  PIN_MD_STEP = 22,
  PIN_MD_DIR = 23,
  PIN_MD_INDEX = 21,
  PIN_MD_DIAG = 19,
};

enum class LOG_TYPE {
  INFO,
  ERROR,
  WARN,
};

enum class LOG_CLASS {
  CONTROLLER,
  CONNECTIVITY,
  INDICATOR,
  LOGGING,
  MANUAL_INTERACTION,
  MOTOR_DRIVER,
  STORAGE,
  ALEXA_INTERACTION,
};

enum class MANUAL_PUSH {
  NO_PUSH,
  LONG_PRESS_UP,
  LONG_PRESS_DOWN,
  LONG_PRESS_BOTH,
  DOUBLE_TAP_UP,
  DOUBLE_TAP_DOWN,
  DOUBLE_TAP_BOTH,
};

enum class DEVICE_STATUS {
  FAULT,
  NOT_CONNECTED,
  MANUAL_OPERATION,
  RESET_MODE,
  WEBPAGE,
  OTA_ENABLED,
};

enum class DRIVER_STATUS {
  AVAILABLE,
  BUSY,
  IN_ACTIVE,
};

enum class MOTION_DIRECTION { FORWARD, BACKWARD, STOP };

struct MOTION_REQUEST {
  MOTION_DIRECTION direction;
  int percentage; // -1 for blind traversal
};

struct DEVICE_CRED {
  String DEVICE_ID = "madac_blinds";
  String SSID = "NETGEAR37";
  String PASSWORD = "phobiclotus754";
};

struct CALIB_PARAMS {
  int TOTAL_STEP_COUNT = -1;
  int STALL_VALUE = -1;
};

struct ALEXA_REQUEST {
  bool STATE = false;
  int PERCENTAGE = -1;
};
} // namespace CONFIG_SET

#endif
