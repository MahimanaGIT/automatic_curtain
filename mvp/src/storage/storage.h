/**
 * @file storage.h
 * @author Mahimana Bhatt (mahimanabhatt@gmail.com)
 * @brief Defines structure for Flash storage for ESP32
 * @version 0.1
 * @date 2022-07-19
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef _STORAGE_INCLUDE_GUARD
#define _STORAGE_INCLUDE_GUARD

#include <Preferences.h>

#include <memory>

#include "../config/config.h"
#include "../logging/logging.h"

class Storage {
   public:
    /**
     * @brief Construct a new Storage object, initializes logger and calls
     * Storage()
     *
     */
    Storage(std::shared_ptr<Logging>);

    /**
     * @brief Destroy the Storage object, closes Flash
     *
     */
    ~Storage();

    /**
     * @brief Saves the wifi creds and device id to Flash, fetching the data from
     * the variable whose pointer is provided as an argument
     *
     * @return true : if save successful
     * @return false : otherwise
     */
    bool SaveDeviceCred(const CONFIG_SET::DEVICE_CRED* device_cred);

    /**
     * @brief Retrieves the wifi creds and device id from the flash and populates
     * them in variable whose pointer is provided as an argument
     *
     * @return true : populate successful
     * @return false : otherwise
     */
    bool PopulateDeviceCred(CONFIG_SET::DEVICE_CRED* device_cred);

    /**
     * @brief Saves the calib params to Flash, fetching the data from the
     * variable whose pointer is provided as an argument
     *
     * @return true : if save successful
     * @return false : otherwise
     */
    bool SaveCalibParam(const CONFIG_SET::CALIB_PARAMS* calib_param);

    /**
     * @brief Retrieves the calib params from the flash and populates
     * them in variable whose pointer is provided as an argument
     *
     * @return true : populate successful
     * @return false : otherwise
     */
    bool PopulateCalibParam(CONFIG_SET::CALIB_PARAMS* calib_param);

    /**
     * @brief Saves the operation mode to Flash, fetching the data from the
     * variable whose pointer is provided as an argument
     *
     * @return true : if save successful
     * @return false : otherwise
     */
    bool SaveOperationMode(const CONFIG_SET::OPERATION_MODE* mode);

    /**
     * @brief Retrieves the operation mode from the flash and populates
     * them in variable whose pointer is provided as an argument
     *
     * @return true : populate successful
     * @return false : otherwise
     */
    bool PopulateOperationMode(CONFIG_SET::OPERATION_MODE* mode);

    /**
     * @brief Clears the memory for CONFIG_SET::STORAGE_NAMESPACE workspace
     *
     */
    void Clear();

   private:
    Preferences preferences_;
    std::shared_ptr<Logging> logger_;
};

#endif
