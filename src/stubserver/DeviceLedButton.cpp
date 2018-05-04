/*
 * DeviceLedButton.cpp
 *
 * Copyright (C) 2018 Holger Grosenick
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <bricklet_rgb_led_button.h>

#include "BrickStack.h"
#include "DeviceLedButton.h"

namespace stubserver {

DeviceLedButton::DeviceLedButton(ValueProvider *vp)
    : valueProvider(vp)
    , ledConfig(RGB_LED_BUTTON_STATUS_LED_CONFIG_SHOW_STATUS)
{
}

DeviceLedButton::~DeviceLedButton() {
    delete valueProvider;
}


bool DeviceLedButton::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    // set default dummy response size: header only
    p.header.length = sizeof(p.header);

    // check function to perform
    switch (p.header.function_id) {
    case RGB_LED_BUTTON_FUNCTION_SET_COLOR:
        red   = p.fullData.payload[0];
        green = p.fullData.payload[1];
        blue  = p.fullData.payload[2];
        notify(visualizationClient);
        return true;

    case RGB_LED_BUTTON_FUNCTION_GET_COLOR:
        p.fullData.payload[0] = red;
        p.fullData.payload[1] = green;
        p.fullData.payload[2] = blue;
        p.header.length += 3;
        return true;

    case RGB_LED_BUTTON_FUNCTION_SET_COLOR_CALIBRATION:
        // data ignored
        return true;

    case RGB_LED_BUTTON_FUNCTION_GET_COLOR_CALIBRATION:
        // always return the default back
        p.fullData.payload[0] = 100;
        p.fullData.payload[1] = 100;
        p.fullData.payload[2] = 50;
        p.header.length += 3;
        return true;

    case RGB_LED_BUTTON_FUNCTION_SET_STATUS_LED_CONFIG:
        ledConfig = p.fullData.payload[0];
        notify(visualizationClient);
        return true;

    case RGB_LED_BUTTON_FUNCTION_GET_STATUS_LED_CONFIG:
        p.fullData.payload[0] = ledConfig;
        p.header.length += 1;
        return true;

    case RGB_LED_BUTTON_FUNCTION_GET_BUTTON_STATE:
        p.fullData.payload[0] = buttonStates & 1 ? RGB_LED_BUTTON_BUTTON_STATE_PRESSED : RGB_LED_BUTTON_BUTTON_STATE_RELEASED;
        p.header.length += 1;
        return true;

    default:
        return false;
    }
}

/**
 * Periodically called by BrickStack.
 */
void DeviceLedButton::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &visualizationClient)
{
    IOPacket packet(uid, RGB_LED_BUTTON_CALLBACK_BUTTON_STATE_CHANGED, 4);
    int newValue;

    if (visualizationClient.useAsInputSource())
        newValue = visualizationClient.getInputState();
    else
        newValue = valueProvider->getValue(relativeTimeMs);

    if (newValue != buttonStates)
    {
        //printf("CB  LED Config: %u  %u  Buttons: %u  %u\n",
        //       packet.fullData.payload[2], packet.fullData.payload[3], packet.fullData.payload[0], packet.fullData.payload[1]);

        buttonStates = newValue;
        brickStack->dispatchCallback(packet);
        notify(visualizationClient);
    }
}

} /* namespace stubserver */
