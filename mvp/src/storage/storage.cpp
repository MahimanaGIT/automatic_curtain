/**
 * @file storage.cpp
 * @author Dhiraj Deshmukh (deshmukhdhiraj15@gmail.com)
 * @brief Contains algorithms to read and write appropriate device data to
 * Flash
 * @version 0.1
 * @date 2022-07-19
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "storage.h"

#include <Preferences.h>

#include "../config/config.h"
#include "../logging/logging.h"

Storage::Storage(std::shared_ptr<Logging> logging) : logger_(logging) {}

Storage::~Storage() {}

bool Storage::SaveDeviceCred(const CONFIG_SET::DEVICE_CRED* device_cred) {
    preferences_.begin(CONFIG_SET::STORAGE_NAMESPACE.c_str(), false);
    size_t status_device_id = preferences_.putString(CONFIG_SET::KEY_DEVICE_ID.c_str(), device_cred->DEVICE_ID);
    size_t status_ssid = preferences_.putString(CONFIG_SET::KEY_SSID.c_str(), device_cred->SSID);
    size_t status_pass = preferences_.putString(CONFIG_SET::KEY_PASSWORD.c_str(), device_cred->PASSWORD);
    preferences_.end();
    if (status_device_id == 0 || status_ssid == 0) {
        return false;
    }
    return true;
}

bool Storage::PopulateDeviceCred(CONFIG_SET::DEVICE_CRED* device_cred) {
    preferences_.begin(CONFIG_SET::STORAGE_NAMESPACE.c_str(), false);
    device_cred->DEVICE_ID = preferences_.getString(CONFIG_SET::KEY_DEVICE_ID.c_str(), "");
    device_cred->SSID = preferences_.getString(CONFIG_SET::KEY_SSID.c_str(), "");
    device_cred->PASSWORD = preferences_.getString(CONFIG_SET::KEY_PASSWORD.c_str(), "");
    preferences_.end();
    if (device_cred->DEVICE_ID == "" || device_cred->SSID == "") {
        return false;
    }
    return true;
}

bool Storage::SaveCalibParam(const CONFIG_SET::CALIB_PARAMS* calib_param) {
    preferences_.begin(CONFIG_SET::STORAGE_NAMESPACE.c_str(), false);
    size_t status_direc = preferences_.putBool(CONFIG_SET::KEY_DIRECTION.c_str(), calib_param->DIRECTION);
    size_t status_total_step =
        preferences_.putInt(CONFIG_SET::KEY_TOTAL_STEP_COUNT.c_str(), calib_param->TOTAL_STEP_COUNT);
    preferences_.end();
    if (status_direc == 0 || status_total_step == 0) {
        return false;
    }
    return true;
}

bool Storage::PopulateCalibParam(CONFIG_SET::CALIB_PARAMS* calib_param) {
    preferences_.begin(CONFIG_SET::STORAGE_NAMESPACE.c_str(), false);
    calib_param->TOTAL_STEP_COUNT = preferences_.getInt(CONFIG_SET::KEY_TOTAL_STEP_COUNT.c_str(), -1);
    calib_param->DIRECTION = preferences_.getBool(CONFIG_SET::KEY_DIRECTION.c_str(), false);
    preferences_.end();
    if (calib_param->TOTAL_STEP_COUNT == -1) {
        return false;
    }
    return true;
}

bool Storage::SaveOperationMode(const CONFIG_SET::OPERATION_MODE* mode) {
    preferences_.begin(CONFIG_SET::STORAGE_NAMESPACE.c_str(), false);
    size_t status_mode = preferences_.putInt(CONFIG_SET::KEY_MODE.c_str(), static_cast<int>(*mode));
    preferences_.end();
    if (status_mode == 0) {
        return false;
    }
    return true;
}

bool Storage::PopulateOperationMode(CONFIG_SET::OPERATION_MODE* mode) {
    preferences_.begin(CONFIG_SET::STORAGE_NAMESPACE.c_str(), false);
    *mode = static_cast<CONFIG_SET::OPERATION_MODE>(
        preferences_.getInt(CONFIG_SET::KEY_MODE.c_str(), static_cast<int>(CONFIG_SET::OPERATION_MODE::NA)));
    preferences_.end();
    if (*mode == CONFIG_SET::OPERATION_MODE::NA) {
        *mode = CONFIG_SET::OPERATION_MODE::RESET;
        return false;
    }
    return true;
}

void Storage::Clear() {
    preferences_.begin(CONFIG_SET::STORAGE_NAMESPACE.c_str(), false);
    preferences_.clear();
    preferences_.end();
}
