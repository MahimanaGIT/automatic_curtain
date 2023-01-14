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

#include <chrono>
#include <cstdint>
#include <deque>
#include <iostream>

/**
 * @brief Namespace to be used for constants/enums
 *
 */
namespace CONFIG_SET {

const int LOGGING_BAUD_RATE = 115200;
const int MOTOR_DRIVER_BAUD_RATE = 115200;
const int TRY_RECONNECT = 3;            // 3 seconds
const int MAX_SECONDS_LOST_WIFI = 600;  // 10 mins
const std::string STORAGE_NAMESPACE = "madac";

// Vars For Indicator Class
const int NUMBER_OF_LEDS = 1;
const int LED_BRIGHTNESS = 25;

// Freq var for Manual Interaction Class
const int DEQUE_ANALYZER_FREQ = 2;

/*
****** STORAGE KEYS ******
These keys are used by Storage class to retrieve the data from FLASHs
*/
const std::string KEY_SSID = "ssid";
const std::string KEY_PASSWORD = "password";
const std::string KEY_DEVICE_ID = "deviceID";
const std::string KEY_TOTAL_STEP_COUNT = "totalStepCount";
const std::string KEY_DIRECTION = "direction";
const std::string KEY_MODE = "mode";
const String DEFAULT_DEVICE_ID = "madac_blinds";

/*
****** MOTOR DRIVER CALIBRATION PARAMETERS ******
*/
const float MOTOR_DRIVER_R_SENSE = 0.11f;
const uint8_t MOTOR_DRIVER_ADDRESS = 0b00;
const uint8_t MOTOR_DRIVER_TOFF = 4;
const uint8_t MOTOR_DRIVER_BLANK_TIME = 24;
const uint16_t MOTOR_DRIVER_RMS_CURRENT = 2000;
const uint16_t MOTOR_DRIVER_MICROSTEP = 16;
const uint32_t MOTOR_DRIVER_TCOOL_THRS = 0xFFFFF;
const uint8_t MOTOR_DRIVER_SE_MIN = 0;
const uint8_t MOTOR_DRIVER_SE_MAX = 2;
const uint8_t MOTOR_DRIVER_SEDN = 0b01;
const uint32_t MOTOR_DRIVER_MAX_SPEED = 5000;
const int MOTOR_DRIVER_SG_THRESH = 40;
const int MOTOR_STOP_TIME_SEC = 120;                // 2 mins
const float STEP_FRACTION_ALLOWANCE = 0.05;         // 5% Allowance allowed for motor reaching destination
const int MODE_EXPIRE_TIME_LIMIT = 300;             // 5 mins
const int WIFI_DISCONNECT_RESTART_TIME_LIMIT = 60;  // 1 mins

enum class OPERATION_MODE {
    RESET,
    MAINTENANCE,
    USER,
    NA,
};

/*
****** ESP32 PINS ******
These are hardware ESP32 pin number where each external pin/component is
connected
PIN_RGB_LED     -> RGB LED WS2812
PIN_BUTTON_UP   -> Up button
PIN_BUTTON_DOWN -> Down button

Motor Driver (TMC2209) Pins
PIN_MD_ENABLE   -> Enable
PIN_MD_RX       -> Serial Receiver
PIN_MD_TX       -> Serial Transmitter
PIN_MD_MS1      -> MS1
PIN_MD_MS2      -> MS2
PIN_MD_STEP     -> Step
PIN_MD_DIR      -> Dir
PIN_MD_INDEX    -> Index
PIN_MD_DIAG     -> Diag (Stall)
*/
const int PIN_RGB_LED = 32;
static const int PIN_BUTTON_UP = 33;
static const int PIN_BUTTON_DOWN = 25;
const int PIN_MD_ENABLE = 4;
const int PIN_MD_RX = 16;
const int PIN_MD_TX = 17;
const int PIN_MD_MS1 = 5;
const int PIN_MD_MS2 = 18;
const int PIN_MD_STEP = 22;
const int PIN_MD_DIR = 23;
const int PIN_MD_INDEX = 21;
const int PIN_MD_DIAG = 19;

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

enum class BUTTON_PRESS {
    NO_PUSH,
    LONG_PRESS,
    DOUBLE_TAP,
};

enum class DEVICE_STATUS {
    FAULT,
    OPERATION_MODE,
    RESET_MODE,
    MAINTENANCE_MODE,
};

enum class DRIVER_STATUS {
    AVAILABLE,
    BUSY,
    IN_ACTIVE,
};

struct MOTION_REQUEST {
    int PERCENTAGE;  // 0, 100 for blind traversal
};

struct DEVICE_CRED {
    String DEVICE_ID = "MaD Automatic Blinds";
    String SSID = "madac_blinds";
    String PASSWORD = "AutomaticCurtain";
};

struct CALIB_PARAMS {
    int TOTAL_STEP_COUNT = INT_MAX;
    bool DIRECTION = false;
};

// alias for time variables
using time_var = std::chrono::time_point<std::chrono::system_clock>;
using current_time = std::chrono::system_clock;

}  // namespace CONFIG_SET

#endif
