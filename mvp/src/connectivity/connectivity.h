/**
 * @file connectivity.h
 * @author Mahimana Bhatt (mahimanabhatt@gmail.com)
 * @brief Define overall structure for class containing WiFi, OTA and webpage
 * functionality
 * @version 0.1
 * @date 2022-07-19
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef _CONNECTIVITY_INCLUDE_GUARD
#define _CONNECTIVITY_INCLUDE_GUARD

#include <ArduinoOTA.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <memory>
#include <tuple>

#include "../config/config.h"
#include "../logging/logging.h"
#include "WiFi.h"
#include "webpage.h"

class Connectivity {
   public:
    /**

     * @brief Fetches the LED pin from config, setups pin mode, setup WS2812
     * RGBLED, initializes logger
     *
     */
    Connectivity(std::shared_ptr<Logging> logging, CONFIG_SET::DEVICE_CRED* device_cred);

    /**
     * @brief Destroy the Connectivity object
     *
     */
    ~Connectivity();

    /**
     * @brief Set the up Arduino OTA
     *
     */
    void startOTA();

    /**
     * @brief Check if the device is connected, if not, try to connect it
     *
     * @return true : wifi is connected
     * @return false : failed to connect, device is no longer connected, despite
     * of trying
     */
    bool ensureConnectivity(const CONFIG_SET::DEVICE_CRED* device_cred);

    /**
     * @brief Displays webpage (for getting WiFi creds and device name), might be
     * a blocking function, returns after a timeout or a submission request is
     * made
     *
     * @return true : if submission was successful, updates CONFIG_SET::WiFi_CRED
     * and CONFIG_SET::DEVICE_ID
     * @return false : otherwise
     */
    void displayWebpage();

    /**
     * @brief Regular call function for syncing OTA requests
     *
     */
    void handleOTA();

    /**
     * @brief Disable OTA
     *
     */
    void stopOTA();

    /**
     * @brief Checks if the device is connected to the WiFi
     *
     * @return true : if connected
     * @return false : otherwise
     */
    bool isConnected();

    /**
     * @brief Creates a server and starts hosting webpage
     *
     */
    void startWebpage();

    /**
     * @brief Stops webpage server
     *
     */
    void stopWebpage();

    /**
     * @brief disconnects WiFi
     *
     */
    void stopWiFi();

    /**
     * @brief Get the latest submission
     *
     * @return std::tuple<bool, CONFIG_SET::DEVICE_CRED>: bool returning if there
     * is a new submission available, device_cred: new submission
     */
    std::tuple<bool, CONFIG_SET::DEVICE_CRED> getWebpageSubmission();

   private:
    std::shared_ptr<Logging> logger_;

    // boolean vars to store the status of functionalities
    bool ota_enabled_ = false;
    bool webpage_enabled_ = false;
    bool hotspot_enabled_ = false;
    bool wifi_client_enabled_ = false;

    bool is_new_submission_available_ = false;
    CONFIG_SET::DEVICE_CRED webpage_submitted_device_cred_;

    /**
     * @brief Starts wifi hotspot, basically start wifi in soft access point mode
     *
     */
    void startHotspot();

    /**
     * @brief Stops wifi hotspot
     *
     */
    void stopHotspot();
};

#endif
