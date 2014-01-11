/*
 * DeviceRelay.cpp
 *
 * Copyright (C) 2013 Holger Grosenick
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

#include <cstring>
#include <sstream>

#include <bricklet_multi_touch.h>

#include <utils/Log.h>
#include <utils/Exceptions.h>

#include "BrickStack.h"
#include "DeviceButtons.h"

using utils::Log;


/**
 * Default init.
 */
DeviceTouchPad::DeviceTouchPad(unsigned _numButtons, ValueProvider *vp)
  : numButtons(_numButtons), sensitivity(181), currentState(0), enabledBits(0xFFFF), values(vp)
{ }


DeviceTouchPad::~DeviceTouchPad()
{
    delete values;
}

/**
 * Check for known function codes.
 */
bool DeviceTouchPad::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, bool &stateChanged)
{
    // set default dummy response size: header only
    p.header.length = sizeof(p.header);

    // check function to perform
    switch(p.header.function_id)
    {
    case MULTI_TOUCH_FUNCTION_RECALIBRATE:
        // do nothing
        return true;

    case MULTI_TOUCH_FUNCTION_GET_TOUCH_STATE:
        p.header.length = sizeof(p.header) + 2;
        p.uint16Value = currentState;
        return true;

    case MULTI_TOUCH_FUNCTION_SET_ELECTRODE_CONFIG:
        enabledBits = p.uint16Value;
        return true;

    case MULTI_TOUCH_FUNCTION_GET_ELECTRODE_CONFIG:
        sensitivity = p.uint8Value;
        return true;

    case MULTI_TOUCH_FUNCTION_SET_ELECTRODE_SENSITIVITY:
        p.header.length = sizeof(p.header) + 2;
        p.uint16Value = enabledBits;
        return true;

    case MULTI_TOUCH_FUNCTION_GET_ELECTRODE_SENSITIVITY:
        p.header.length = sizeof(p.header) + 1;
        p.uint8Value = sensitivity;
        return true;
    }
    return false;
}

/**
 * Check for monoflop callbacks.
 */
void DeviceTouchPad::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, bool &stateChanged)
{
    unsigned newValue = values->getValue(relativeTimeMs) & enabledBits;
    if (newValue != currentState)
    {
        printf("New value: %X\n", newValue);

        // trigger button press / release
        IOPacket packet(uid, MULTI_TOUCH_CALLBACK_TOUCH_STATE, 2);
        packet.uint16Value = newValue;
        currentState = newValue;
        //brickStack->dispatchCallback(packet);
    }
}
