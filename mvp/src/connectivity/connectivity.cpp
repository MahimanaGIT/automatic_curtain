/**
 * @file connectivity.cpp
 * @author Dhiraj Deshmukh (deshmukhdhiraj15@gmail.com)
 * @brief Contains algorithms related to connecting to WiFi, OTA updates and
 * webpage display
 * @version 0.1
 * @date 2022-07-19
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "connectivity.h"
#include "../config/config.h"
#include "../logging/logging.h"
#include "WiFi.h"
#include "webpage.h"
#include <ArduinoOTA.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <mutex>
#include <tuple>

std::mutex webpage_submission_mutex;
AsyncWebServer webpage_server_(80);

Connectivity::Connectivity() {}

Connectivity::~Connectivity() {}

Connectivity::Connectivity(Logging *logging,
                           CONFIG_SET::DEVICE_CRED *device_cred)
    : logger_(logging) {
  this->ensureConnectivity(device_cred);
}

bool Connectivity::ensureConnectivity(
    const CONFIG_SET::DEVICE_CRED *device_cred) {
  if (!this->isConnected()) {
    WiFi.begin(device_cred->SSID.c_str(), device_cred->PASSWORD.c_str());
    WiFi.waitForConnectResult();
  } else {
    return true;
  }
  bool is_connected = this->isConnected();
  if (is_connected) {
    this->wifi_client_enabled_ = true;
  }
  return is_connected;
}

void Connectivity::stopWiFi() {
  if (this->wifi_client_enabled_) {
    WiFi.disconnect();
  }
  this->wifi_client_enabled_ = false;
  this->logger_->log(CONFIG_SET::LOG_TYPE::INFO,
                     CONFIG_SET::LOG_CLASS::CONNECTIVITY, "Stopping WiFi");
}

void Connectivity::startOTA() {
  if (this->ota_enabled_) {
    return;
  }

  this->startHotspot();

  ArduinoOTA.onStart([&]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {
      type = "filesystem";
    }
    this->logger_->log(CONFIG_SET::LOG_TYPE::INFO,
                       CONFIG_SET::LOG_CLASS::CONNECTIVITY,
                       String("Start updating " + type).c_str());
  });

  ArduinoOTA.onEnd([&]() {
    this->logger_->log(CONFIG_SET::LOG_TYPE::INFO,
                       CONFIG_SET::LOG_CLASS::CONNECTIVITY, "End");
  });

  ArduinoOTA.onProgress([&](unsigned int progress, unsigned int total) {
    this->logger_->log(
        CONFIG_SET::LOG_TYPE::INFO, CONFIG_SET::LOG_CLASS::CONNECTIVITY,
        String("Progress: " + (progress / (total / 100))).c_str());
  });

  ArduinoOTA.onError([&](ota_error_t error) {
    this->logger_->log(CONFIG_SET::LOG_TYPE::INFO,
                       CONFIG_SET::LOG_CLASS::CONNECTIVITY,
                       String("Error: " + error).c_str());
    if (error == OTA_AUTH_ERROR) {
      this->logger_->log(CONFIG_SET::LOG_TYPE::ERROR,
                         CONFIG_SET::LOG_CLASS::CONNECTIVITY, "Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      this->logger_->log(CONFIG_SET::LOG_TYPE::ERROR,
                         CONFIG_SET::LOG_CLASS::CONNECTIVITY, "Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      this->logger_->log(CONFIG_SET::LOG_TYPE::ERROR,
                         CONFIG_SET::LOG_CLASS::CONNECTIVITY, "Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      this->logger_->log(CONFIG_SET::LOG_TYPE::ERROR,
                         CONFIG_SET::LOG_CLASS::CONNECTIVITY, "Receive Failed");
    } else if (error == OTA_END_ERROR) {
      this->logger_->log(CONFIG_SET::LOG_TYPE::ERROR,
                         CONFIG_SET::LOG_CLASS::CONNECTIVITY, "End Failed");
    }
  });

  ArduinoOTA.begin();
  this->ota_enabled_ = true;
  this->logger_->log(CONFIG_SET::LOG_TYPE::INFO,
                     CONFIG_SET::LOG_CLASS::CONNECTIVITY, "Starting OTA");
}

void Connectivity::handleOTA() {
  if (this->ota_enabled_) {
    ArduinoOTA.handle();
  }
}

void Connectivity::stopOTA() {
  if (this->ota_enabled_) {
    ArduinoOTA.end();
  }
  this->stopHotspot();
  this->ota_enabled_ = false;
  this->logger_->log(CONFIG_SET::LOG_TYPE::INFO,
                     CONFIG_SET::LOG_CLASS::CONNECTIVITY, "Stopping OTA");
}

bool Connectivity::isConnected() { return WiFi.status() == WL_CONNECTED; }

void Connectivity::startHotspot() {
  CONFIG_SET::DEVICE_CRED device_cred;
  IPAddress local_IP(192, 168, 0, 1);
  IPAddress gateway(192, 168, 0, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(device_cred.SSID.c_str(), device_cred.PASSWORD.c_str());
  this->hotspot_enabled_ = true;
  this->logger_->log(CONFIG_SET::LOG_TYPE::INFO,
                     CONFIG_SET::LOG_CLASS::CONNECTIVITY, "Starting Hotspot");
}

void Connectivity::stopHotspot() {
  if (this->hotspot_enabled_) {
    WiFi.softAPdisconnect(true);
  }
  this->logger_->log(CONFIG_SET::LOG_TYPE::INFO,
                     CONFIG_SET::LOG_CLASS::CONNECTIVITY, "Stopping Hotspot");
  this->hotspot_enabled_ = false;
}

void Connectivity::startWebpage() {
  this->startHotspot();

  if (this->webpage_enabled_) {
    return;
  }

  webpage_server_.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  });

  // Send a GET request to
  // <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
  webpage_server_.on("/submit", HTTP_GET, [&](AsyncWebServerRequest *request) {
    if (request->hasArg("device_name")) {
      String arg = request->arg("device_name");
      this->webpage_submitted_device_cred_.DEVICE_ID = String(arg.c_str());
    }
    if (request->hasArg("wifi_ssid")) {
      String arg = request->arg("wifi_ssid");
      this->webpage_submitted_device_cred_.SSID = String(arg.c_str());
    }
    if (request->hasArg("wifi_password")) {
      String arg = request->arg("wifi_password");
      this->webpage_submitted_device_cred_.PASSWORD = String(arg.c_str());
    }
    request->send_P(200, "text/html", dialog_html);

    this->logger_->log(CONFIG_SET::LOG_TYPE::INFO,
                       CONFIG_SET::LOG_CLASS::CONNECTIVITY,
                       "Got webpage submission");
    const std::lock_guard<std::mutex> lock(webpage_submission_mutex);
    this->is_new_submission_available_ = true;
  });

  // Start server
  webpage_server_.begin();
  this->webpage_enabled_ = true;
  this->logger_->log(CONFIG_SET::LOG_TYPE::INFO,
                     CONFIG_SET::LOG_CLASS::CONNECTIVITY, "Starting Webpage");
}

void Connectivity::displayWebpage() {}

void Connectivity::stopWebpage() {
  this->stopHotspot();
  if (this->webpage_enabled_) {
    webpage_server_.end();
  }
  this->logger_->log(CONFIG_SET::LOG_TYPE::INFO,
                     CONFIG_SET::LOG_CLASS::CONNECTIVITY, "Stopping Webpage");
  this->webpage_enabled_ = false;
}

std::tuple<bool, CONFIG_SET::DEVICE_CRED> Connectivity::getWebpageSubmission() {
  const std::lock_guard<std::mutex> lock(webpage_submission_mutex);
  std::tuple<bool, CONFIG_SET::DEVICE_CRED> return_value(
      this->is_new_submission_available_, this->webpage_submitted_device_cred_);
  this->is_new_submission_available_ = false;
  return return_value;
}