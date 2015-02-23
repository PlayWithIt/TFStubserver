/*
 * DeviceTilt.cpp
 *
 * Copyright (C) 2015 Holger Grosenick
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

#include <bricklet_tilt.h>

#include "BrickStack.h"
#include "DeviceTilt.h"

namespace stubserver {


DeviceTilt::DeviceTilt(ValueProvider *vp)
  : SensorState(0, TILT_TILT_STATE_CLOSED_VIBRATING)
  , valueProvider(vp)
  , callbackEnabled(false)
{
}

DeviceTilt::~DeviceTilt()
{
    delete valueProvider;
}


bool DeviceTilt::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    // set default dummy response size: header only
    p.header.length = sizeof(p.header);

    // check function to perform
    switch (p.header.function_id) {
    case TILT_FUNCTION_GET_TILT_STATE:
        p.uint8Value = sensorValue;
        p.header.length += sizeof(p.uint8Value);
        return true;

    case TILT_FUNCTION_IS_TILT_STATE_CALLBACK_ENABLED:
        p.boolValue = callbackEnabled;
        p.header.length += sizeof(p.boolValue);
        return true;

    case TILT_FUNCTION_ENABLE_TILT_STATE_CALLBACK:
        callbackEnabled = true;
        return true;

    case TILT_FUNCTION_DISABLE_TILT_STATE_CALLBACK:
        callbackEnabled = false;
        return true;

    default:
        return false;
    }
}

void DeviceTilt::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &visualizationClient)
{
    int newValue = visualizationClient.getInputState();

    if (visualizationClient.useAsInputSource())
        newValue = visualizationClient.getInputState();
    else
        newValue = valueProvider->getValue(relativeTimeMs);

    if (newValue != sensorValue)
    {
        sensorValue = newValue;

        if (callbackEnabled) {
            IOPacket packet(uid, TILT_CALLBACK_TILT_STATE, 1);
            packet.uint8Value = newValue;
            brickStack->dispatchCallback(packet);
        }

        notify(visualizationClient);
    }
}

} /* namespace stubserver */
