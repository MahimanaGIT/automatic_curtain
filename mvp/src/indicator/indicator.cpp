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

#include <FastLED.h>

#include "../config/config.h"
#include "../logging/logging.h"

Indicator::~Indicator() {}

Indicator::Indicator(std::shared_ptr<Logging> logging) : logger_(logging) {
    InitializeLED();
}

void Indicator::InitializeLED() {
    FastLED.addLeds<WS2812, CONFIG_SET::PIN_RGB_LED, GRB>(leds_, CONFIG_SET::NUMBER_OF_LEDS);
    FastLED.setBrightness(CONFIG_SET::LED_BRIGHTNESS);
    leds_[0] = CRGB::Green;
    FastLED.show();
}

bool Indicator::UpdateStatus(CONFIG_SET::DEVICE_STATUS status) {
    switch (status) {
        case CONFIG_SET::DEVICE_STATUS::FAULT:
            leds_[0] = CRGB::Red;
            break;
        case CONFIG_SET::DEVICE_STATUS::NOT_CONNECTED:
            leds_[0] = CRGB::Blue;
            break;
        case CONFIG_SET::DEVICE_STATUS::MANUAL_OPERATION:
            leds_[0] = CRGB::Purple;
            break;
        case CONFIG_SET::DEVICE_STATUS::RESET_MODE:
            leds_[0] = CRGB::White;
            break;
        case CONFIG_SET::DEVICE_STATUS::WEBPAGE:
            leds_[0] = CRGB::Yellow;
            break;
        case CONFIG_SET::DEVICE_STATUS::OTA_ENABLED:
            leds_[0] = CRGB::Green;
            break;
        default:
            leds_[0] = CRGB::Pink;
    }
    FastLED.show();
}