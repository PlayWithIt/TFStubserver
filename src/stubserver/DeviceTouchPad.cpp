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
#include <bricklet_multi_touch_v2.h>

#include <utils/utils.h>

#include "BrickStack.h"
#include "DeviceTouchPad.h"

namespace stubserver {


/**
 * Default init.
 */
DeviceTouchPad::DeviceTouchPad(unsigned _numButtons, ValueProvider *vp, bool isV2)
  : V2Device(NULL, NULL, isV2)
  , numButtons(_numButtons), sensitivity(181)
  , currentState(0), enabledBits(0xFFFF), values(vp)
{
}


DeviceTouchPad::~DeviceTouchPad()
{
    delete values;
}

/**
 * Check for known function codes.
 */
bool DeviceTouchPad::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    // set default dummy response size: header only
    p.header.length = sizeof(p.header);

    // check function to perform
    if (isV2) {
        switch(p.header.function_id)
        {
        case MULTI_TOUCH_V2_FUNCTION_RECALIBRATE:
            // do nothing
            return true;

        case MULTI_TOUCH_V2_FUNCTION_GET_TOUCH_STATE:
            p.header.length = sizeof(p.header) + 2;
            p.uint16Value = currentState;
            return true;

        case MULTI_TOUCH_V2_FUNCTION_SET_ELECTRODE_CONFIG:
            enabledBits = p.uint16Value;
            return true;

        case MULTI_TOUCH_V2_FUNCTION_GET_ELECTRODE_CONFIG:
            p.header.length = sizeof(p.header) + 2;
            p.uint16Value = enabledBits;
            return true;

        case MULTI_TOUCH_V2_FUNCTION_SET_ELECTRODE_SENSITIVITY:
            sensitivity = p.uint8Value;
            return true;

        case MULTI_TOUCH_V2_FUNCTION_GET_ELECTRODE_SENSITIVITY:
            p.header.length = sizeof(p.header) + 1;
            p.uint8Value = sensitivity;
            return true;
        }
    }
    else {
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
    		p.header.length = sizeof(p.header) + 2;
    		p.uint16Value = enabledBits;
    		return true;

    	case MULTI_TOUCH_FUNCTION_SET_ELECTRODE_SENSITIVITY:
    		sensitivity = p.uint8Value;
    		return true;

    	case MULTI_TOUCH_FUNCTION_GET_ELECTRODE_SENSITIVITY:
    		p.header.length = sizeof(p.header) + 1;
    		p.uint8Value = sensitivity;
    		return true;
    	}
    }

    return V2Device::consumeCommand(relativeTimeMs, p, visualizationClient);
}

/**
 * Check for monoflop callbacks.
 */
void DeviceTouchPad::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &vc)
{
    unsigned newValue;

    if (vc.useAsInputSource())
        newValue = vc.getInputState();
    else
        newValue = values->getValue(relativeTimeMs) & enabledBits;

    if (newValue != currentState)
    {
        // trigger button press / release
    	if (isV2) {
    		IOPacket packet(uid, MULTI_TOUCH_V2_CALLBACK_TOUCH_STATE, 13);
    		utils::bits2bool(newValue, packet.touchBits, 13);
    		currentState = newValue;
    		brickStack->dispatchCallback(packet);
    	}
    	else {
    		IOPacket packet(uid, MULTI_TOUCH_CALLBACK_TOUCH_STATE, 2);
    		packet.uint16Value = newValue;
    		currentState = newValue;
    		brickStack->dispatchCallback(packet);
    	}
    }
}

} /* namespace stubserver */
