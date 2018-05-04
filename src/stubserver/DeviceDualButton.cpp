/*
 * DeviceDualButton.cpp
 *
 * Copyright (C) 2017 Holger Grosenick
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

#include <bricklet_dual_button.h>

#include "BrickStack.h"
#include "DeviceDualButton.h"

namespace stubserver {

DeviceDualButton::DeviceDualButton(ValueProvider *vp)
    : valueProvider(vp)
    , led0(DUAL_BUTTON_LED_STATE_AUTO_TOGGLE_OFF)
    , led1(DUAL_BUTTON_LED_STATE_AUTO_TOGGLE_OFF)
{
}

DeviceDualButton::~DeviceDualButton() {
    delete valueProvider;
}


bool DeviceDualButton::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    uint8_t led;
    uint8_t states[4];

    // set default dummy response size: header only
    p.header.length = sizeof(p.header);

    // check function to perform
    switch (p.header.function_id) {
    case DUAL_BUTTON_FUNCTION_SET_LED_STATE:
        led0 = p.fullData.payload[0];
        led1 = p.fullData.payload[1];

        // printf("Set LED Config: %u  %u\n", led0, led1);
        if (calculateStates(buttonStates, states))
            notify(visualizationClient);
        return true;

    case DUAL_BUTTON_FUNCTION_GET_LED_STATE:
        p.fullData.payload[0] = led0;
        p.fullData.payload[1] = led1;
        p.header.length += 2;
        return true;

    case DUAL_BUTTON_FUNCTION_GET_BUTTON_STATE:
        p.fullData.payload[0] = buttonStates & 1 ? DUAL_BUTTON_BUTTON_STATE_PRESSED : DUAL_BUTTON_BUTTON_STATE_RELEASED;
        p.fullData.payload[1] = buttonStates & 2 ? DUAL_BUTTON_BUTTON_STATE_PRESSED : DUAL_BUTTON_BUTTON_STATE_RELEASED;
        p.header.length += 2;
        return true;

    case DUAL_BUTTON_FUNCTION_SET_SELECTED_LED_STATE:
        led = p.fullData.payload[0];
        if (led == 0)
            this->led0 = p.fullData.payload[1];
        else
            this->led1 = p.fullData.payload[1];

        // printf("Set LED Config: %u  %u\n", led0, led1);
        if (calculateStates(buttonStates, states))
            notify(visualizationClient);
        return true;

    default:
        return false;
    }
}

/**
 * Calculate LED state based on button state and LED behavior config.
 */
static uint8_t ledState(uint8_t buttonState, uint8_t ledConfig)
{
    if (ledConfig == DUAL_BUTTON_LED_STATE_ON || ledConfig == DUAL_BUTTON_LED_STATE_OFF)
        return ledConfig;

    if (ledConfig == DUAL_BUTTON_LED_STATE_AUTO_TOGGLE_OFF)
    {
        return buttonState == DUAL_BUTTON_BUTTON_STATE_PRESSED ? DUAL_BUTTON_LED_STATE_AUTO_TOGGLE_ON : DUAL_BUTTON_LED_STATE_AUTO_TOGGLE_OFF;
    }

    if (ledConfig == DUAL_BUTTON_LED_STATE_AUTO_TOGGLE_ON)
    {
        return buttonState == DUAL_BUTTON_BUTTON_STATE_PRESSED ? DUAL_BUTTON_LED_STATE_AUTO_TOGGLE_OFF : DUAL_BUTTON_LED_STATE_AUTO_TOGGLE_ON;
    }
    return 0;
}

/**
 * Calculates LED state and return true if state was changed.
 * @param buttonValue current button value
 * @param dest 4 byte array for callback return data
 */
bool DeviceDualButton::calculateStates(int buttonValue, uint8_t dest[])
{
    dest[0] = buttonValue & 1 ? DUAL_BUTTON_BUTTON_STATE_PRESSED : DUAL_BUTTON_BUTTON_STATE_RELEASED;
    dest[1] = buttonValue & 2 ? DUAL_BUTTON_BUTTON_STATE_PRESSED : DUAL_BUTTON_BUTTON_STATE_RELEASED;
    dest[2] = ledState(dest[0], led0);
    dest[3] = ledState(dest[1], led1);

    bool led_l = dest[2] == DUAL_BUTTON_LED_STATE_ON || dest[2] == DUAL_BUTTON_LED_STATE_AUTO_TOGGLE_ON;
    bool led_r = dest[3] == DUAL_BUTTON_LED_STATE_ON || dest[3] == DUAL_BUTTON_LED_STATE_AUTO_TOGGLE_ON;

    if (led_l != ledOn_l || led_r != ledOn_r)
    {
        ledOn_l = led_l;
        ledOn_r = led_r;
        return true;
    }
    return false;
}

/**
 * Periodically called by BrickStack.
 */
void DeviceDualButton::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &visualizationClient)
{
    IOPacket packet(uid, DUAL_BUTTON_CALLBACK_STATE_CHANGED, 4);
    int newValue;

    if (visualizationClient.useAsInputSource())
        newValue = visualizationClient.getInputState();
    else
        newValue = valueProvider->getValue(relativeTimeMs);

    bool changed = calculateStates(newValue, packet.fullData.payload);
    if (changed || newValue != buttonStates)
    {
        //printf("CB  LED Config: %u  %u  Buttons: %u  %u\n",
        //       packet.fullData.payload[2], packet.fullData.payload[3], packet.fullData.payload[0], packet.fullData.payload[1]);

        buttonStates = newValue;
        brickStack->dispatchCallback(packet);
        notify(visualizationClient);
    }
}

} /* namespace stubserver */
