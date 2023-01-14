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

#include <ArduinoOTA.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <mutex>
#include <tuple>

#include "../config/config.h"
#include "../logging/logging.h"
#include "WiFi.h"
#include "webpage.h"

Connectivity::Connectivity(std::shared_ptr<Logging>& logging, CONFIG_SET::DEVICE_CRED* device_cred) : logger_(logging) {
    using namespace CONFIG_SET;
    time_last_connected_ = current_time::now();
}

Connectivity::~Connectivity() {
    StopEnsuringConnectivity();
    StopOTA();
    StopWiFi();
    StopWebpage();
    StopHotspot();
    logger_->Log(CONFIG_SET::LOG_TYPE::INFO, CONFIG_SET::LOG_CLASS::CONNECTIVITY, "Done Destroying");
}

void Connectivity::StartEnsureConnectivity(const CONFIG_SET::DEVICE_CRED device_cred) {
    if (ensure_conn_thread_ != nullptr) {
        return;
    }
    device_cred_ = device_cred;
    ensure_conn_thread_.reset(new std::thread(&Connectivity::EnsureConnectivity, this));
}

void Connectivity::StopEnsuringConnectivity() {
    if (keep_handler_running_) {
        keep_handler_running_ = false;
        ensure_conn_thread_->detach();
    }
}

void Connectivity::EnsureConnectivity() {
    using namespace CONFIG_SET;
    logger_->Log(LOG_TYPE::INFO, LOG_CLASS::CONNECTIVITY, "Starting handler");
    keep_handler_running_ = true;

    while (keep_handler_running_) {
        if (!IsConnected()) {
            logger_->Log(LOG_TYPE::INFO, LOG_CLASS::CONNECTIVITY, "Lost Connectivity, Trying to Connect WiFi");
            WiFi.disconnect(true);
            WiFi.mode(WIFI_STA);
            WiFi.begin(device_cred_.SSID.c_str(), device_cred_.PASSWORD.c_str());
            WiFi.waitForConnectResult();
        }
        if (IsConnected()) {
            time_last_connected_ = current_time::now();
        }
        std::this_thread::sleep_for(std::chrono::seconds(TRY_RECONNECT));
    }
    logger_->Log(LOG_TYPE::INFO, LOG_CLASS::CONNECTIVITY, "Exiting handler");
}

int Connectivity::GetSecLostConnection() {
    using namespace CONFIG_SET;
    return std::chrono::duration_cast<std::chrono::seconds>(current_time::now() - time_last_connected_).count();
}

void Connectivity::StopWiFi() {
    WiFi.disconnect(true);
    logger_->Log(CONFIG_SET::LOG_TYPE::INFO, CONFIG_SET::LOG_CLASS::CONNECTIVITY, "Stopping WiFi");
}

void Connectivity::StartOTA() {
    if (ota_enabled_) {
        return;
    }

    StartHotspot();

    ArduinoOTA.onStart([&]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
            type = "sketch";
        } else {
            type = "filesystem";
        }
        logger_->Log(CONFIG_SET::LOG_TYPE::INFO, CONFIG_SET::LOG_CLASS::CONNECTIVITY,
                     String("Start updating " + type).c_str());
    });

    ArduinoOTA.onEnd([&]() { logger_->Log(CONFIG_SET::LOG_TYPE::INFO, CONFIG_SET::LOG_CLASS::CONNECTIVITY, "End"); });

    ArduinoOTA.onProgress([&](unsigned int progress, unsigned int total) {
        logger_->Log(CONFIG_SET::LOG_TYPE::INFO, CONFIG_SET::LOG_CLASS::CONNECTIVITY,
                     String("Progress: " + (progress / (total / 100))).c_str());
    });

    ArduinoOTA.onError([&](ota_error_t error) {
        logger_->Log(CONFIG_SET::LOG_TYPE::INFO, CONFIG_SET::LOG_CLASS::CONNECTIVITY,
                     String("Error: " + error).c_str());
        if (error == OTA_AUTH_ERROR) {
            logger_->Log(CONFIG_SET::LOG_TYPE::ERROR, CONFIG_SET::LOG_CLASS::CONNECTIVITY, "Auth Failed");
        } else if (error == OTA_BEGIN_ERROR) {
            logger_->Log(CONFIG_SET::LOG_TYPE::ERROR, CONFIG_SET::LOG_CLASS::CONNECTIVITY, "Begin Failed");
        } else if (error == OTA_CONNECT_ERROR) {
            logger_->Log(CONFIG_SET::LOG_TYPE::ERROR, CONFIG_SET::LOG_CLASS::CONNECTIVITY, "Connect Failed");
        } else if (error == OTA_RECEIVE_ERROR) {
            logger_->Log(CONFIG_SET::LOG_TYPE::ERROR, CONFIG_SET::LOG_CLASS::CONNECTIVITY, "Receive Failed");
        } else if (error == OTA_END_ERROR) {
            logger_->Log(CONFIG_SET::LOG_TYPE::ERROR, CONFIG_SET::LOG_CLASS::CONNECTIVITY, "End Failed");
        }
    });

    ArduinoOTA.begin();
    ota_enabled_ = true;
    logger_->Log(CONFIG_SET::LOG_TYPE::INFO, CONFIG_SET::LOG_CLASS::CONNECTIVITY, "Starting OTA");
}

void Connectivity::HandleOTA() {
    if (ota_enabled_) {
        ArduinoOTA.handle();
    }
}

void Connectivity::StopOTA() {
    if (ota_enabled_) {
        ArduinoOTA.end();
    }
    StopHotspot();
    ota_enabled_ = false;
    logger_->Log(CONFIG_SET::LOG_TYPE::INFO, CONFIG_SET::LOG_CLASS::CONNECTIVITY, "Stopping OTA");
}

bool Connectivity::IsConnected() {
    return WiFi.status() == WL_CONNECTED;
}

void Connectivity::StartHotspot() {
    CONFIG_SET::DEVICE_CRED device_cred;
    IPAddress local_IP(192, 168, 0, 10);
    IPAddress gateway(192, 168, 0, 1);
    IPAddress subnet(255, 255, 0, 0);
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(local_IP, gateway, subnet);
    WiFi.softAP(device_cred.SSID.c_str(), device_cred.PASSWORD.c_str());
    hotspot_enabled_ = true;
    logger_->Log(CONFIG_SET::LOG_TYPE::INFO, CONFIG_SET::LOG_CLASS::CONNECTIVITY, "Starting Hotspot");
}

void Connectivity::StopHotspot() {
    if (hotspot_enabled_) {
        WiFi.softAPdisconnect(true);
    }
    logger_->Log(CONFIG_SET::LOG_TYPE::INFO, CONFIG_SET::LOG_CLASS::CONNECTIVITY, "Stopping Hotspot");
    hotspot_enabled_ = false;
}

void Connectivity::StartWebpage() {
    StartHotspot();

    if (webpage_enabled_) {
        return;
    }

    webpage_server_.reset(new AsyncWebServer(80));
    webpage_server_->on("/", HTTP_GET,
                        [](AsyncWebServerRequest* request) { request->send_P(200, "text/html", index_html); });

    // Send a GET request to
    // <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
    webpage_server_->on("/submit", HTTP_GET, [&](AsyncWebServerRequest* request) {
        if (request->hasArg("device_name")) {
            String arg = request->arg("device_name");
            webpage_submitted_device_cred_.DEVICE_ID = String(arg.c_str());
        }
        if (request->hasArg("wifi_ssid")) {
            String arg = request->arg("wifi_ssid");
            webpage_submitted_device_cred_.SSID = String(arg.c_str());
        }
        if (request->hasArg("wifi_password")) {
            String arg = request->arg("wifi_password");
            webpage_submitted_device_cred_.PASSWORD = String(arg.c_str());
        }
        request->send_P(200, "text/html", dialog_html);

        logger_->Log(CONFIG_SET::LOG_TYPE::INFO, CONFIG_SET::LOG_CLASS::CONNECTIVITY, "Got webpage submission");
        const std::lock_guard<std::mutex> lock(webpage_submission_mutex_);
        is_new_submission_available_ = true;
    });

    // Start server
    webpage_server_->begin();
    Serial.println(WiFi.softAPIP());
    webpage_enabled_ = true;
    logger_->Log(CONFIG_SET::LOG_TYPE::INFO, CONFIG_SET::LOG_CLASS::CONNECTIVITY, "Starting Webpage");
}

void Connectivity::StopWebpage() {
    StopHotspot();
    if (webpage_enabled_) {
        webpage_server_->end();
        webpage_server_.reset();
    }
    logger_->Log(CONFIG_SET::LOG_TYPE::INFO, CONFIG_SET::LOG_CLASS::CONNECTIVITY, "Stopping Webpage");
    webpage_enabled_ = false;
}

std::tuple<bool, CONFIG_SET::DEVICE_CRED> Connectivity::GetWebpageSubmission() {
    const std::lock_guard<std::mutex> lock(webpage_submission_mutex_);
    std::tuple<bool, CONFIG_SET::DEVICE_CRED> return_value(is_new_submission_available_,
                                                           webpage_submitted_device_cred_);
    is_new_submission_available_ = false;
    return return_value;
}