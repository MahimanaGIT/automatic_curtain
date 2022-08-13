/**
 * @file indicator.cpp
 * @author Dhiraj Deshmukh (deshmukhdhiraj15@gmail.com)
 * @brief Contains algorithms for RGB LED
 * @version 0.1
 * @date 2022-07-19
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "indicator.h"
#include "../config/config.h"
#include "../logging/logging.h"
#include <FastLED.h>

Indicator::Indicator() {}

Indicator::~Indicator() {}

Indicator::Indicator(Logging *logging) : logger_(logging) { initializeLED(); }

void Indicator::initializeLED() {
  FastLED.addLeds<WS2812, static_cast<int>(CONFIG_SET::PINS::PIN_RGB_LED), GRB>(
      this->leds_, NUMBER_OF_LEDS);
  FastLED.setBrightness(LED_BRIGHTNESS);
  this->leds_[0] = CRGB::Green;
  FastLED.show();
}

bool Indicator::updateStatus(CONFIG_SET::DEVICE_STATUS status) {
  this->logger_->log(CONFIG_SET::LOG_TYPE::INFO,
                     CONFIG_SET::LOG_CLASS::INDICATOR, "Updating Status");
  switch (status) {
  case CONFIG_SET::DEVICE_STATUS::FAULT:
    this->leds_[0] = CRGB::Red;
    break;
  case CONFIG_SET::DEVICE_STATUS::NOT_CONNECTED:
    this->leds_[0] = CRGB::Blue;
    break;
  case CONFIG_SET::DEVICE_STATUS::MANUAL_OPERATION:
    this->leds_[0] = CRGB::Purple;
    break;
  case CONFIG_SET::DEVICE_STATUS::RESET_MODE:
    this->leds_[0] = CRGB::White;
    break;
  case CONFIG_SET::DEVICE_STATUS::WEBPAGE:
    this->leds_[0] = CRGB::Yellow;
    break;
  case CONFIG_SET::DEVICE_STATUS::OTA_ENABLED:
    this->leds_[0] = CRGB::Green;
    break;
  default:
    this->leds_[0] = CRGB::Pink;
  }
  FastLED.show();
}