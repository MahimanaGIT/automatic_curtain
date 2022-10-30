/**
 * @file logging.h
 * @author Mahimana Bhatt (mahimanabhatt@gmail.com)
 * @brief Defines code structure for logging of device
 * @version 0.1
 * @date 2022-07-19
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef _LOGGING_INCLUDE_GUARD
#define _LOGGING_INCLUDE_GUARD

#include <Arduino.h>

#include <mutex>

#include "../config/config.h"

class Logging {
   public:
    /**
     * @brief Construct a new Logging object, initializes the serial connection
     *
     */
    Logging();

    /**
     * @brief Destroy the Logging object
     *
     */
    ~Logging();

    /**
     * @brief For now, the function will just print a message on serial, it can be
     * over wifi or just hardware serial
     *
     * @param message
     * @return true : if the print is successful
     * @return false : otherwise
     */
    bool log(CONFIG_SET::LOG_TYPE log_type, CONFIG_SET::LOG_CLASS log_class, const char* message) const;

    /**
     * @brief For now, the function will just print a message on serial, it can be
     * over wifi or just hardware serial
     *
     * @param message
     * @return true : if the print is successful
     * @return false : otherwise
     */
    bool log(CONFIG_SET::LOG_TYPE log_type, CONFIG_SET::LOG_CLASS log_class, String message) const;

    /**
     * @brief This function enables or disables logging
     * true: logging enabled
     * false: logging disabled
     */
    void setLoggingStatus(bool status);

    /**
     * @brief Get the Logging Status object
     *
     * @return bool: logging_status_
     */
    bool getLoggingStatus() const;

   private:
    bool logging_status_;
};

#endif
