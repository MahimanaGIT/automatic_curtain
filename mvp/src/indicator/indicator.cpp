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
    using namespace CONFIG_SET;
    switch (status) {
        case DEVICE_STATUS::OPERATION_MODE:
            leds_[0] = CRGB::Green;
            break;
        case DEVICE_STATUS::RESET_MODE:
            leds_[0] = CRGB::White;
            break;
        case DEVICE_STATUS::MAINTENANCE_MODE:
            leds_[0] = CRGB::Purple;
            break;
        default:
            leds_[0] = CRGB::Red;
    }
    FastLED.show();
}