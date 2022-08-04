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

#include "../config/config.h"

class Connectivity {
public:
  /**
   * @brief Construct a new Connectivity object, connects to wifi or starts
   * hotspot
   *
   */
  Connectivity();

  /**
   * @brief Destroy the Connectivity object
   *
   */
  ~Connectivity();

  /**
   * @brief Set the up Arduino OTA
   *
   */
  void setupOTA();

  /**
   * @brief Check if the device is connected, if not, try to connect it
   *
   * @return true : wifi is connected
   * @return false : failed to connect, device is no longer connected, despite
   * of trying
   */
  bool ensureConnectivity();

  /**
   * @brief Displays webpage (for getting WiFi creds and device name), might be
   * a blocking function, returns after a timeout or a submission request is
   * made
   *
   * @return true : if submission was successful, updates CONFIG_SET::WiFi_CRED
   * and CONFIG_SET::DEVICE_ID
   * @return false : otherwise
   */
  bool displayWebpage();

  /**
   * @brief Regular call function for syncing OTA requests
   *
   */
  void handleOTA();

  /**
   * @brief Checks if the device is connected to the WiFi
   *
   * @return true : if connected
   * @return false : otherwise
   */
  bool isConnected();
};

#endif
