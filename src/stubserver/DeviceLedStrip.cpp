/*
 * DeviceLedStrip.cpp
 *
 * Copyright (C) 2014 Holger Grosenick
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

#include <bricklet_led_strip.h>

#include "DeviceLedStrip.h"
#include "BrickStack.h"


namespace stubserver {

/**
 * Default init.
 */
DeviceLedStrip::DeviceLedStrip()
  : chipType(LED_STRIP_CHIP_TYPE_WS2801)
  , frequency(1666666)
  , length(0)
  , frameRenderedCb(LED_STRIP_FUNCTION_GET_FRAME_DURATION, LED_STRIP_FUNCTION_SET_FRAME_DURATION, LED_STRIP_CALLBACK_FRAME_RENDERED, 100)
{
    memset(r, 0, sizeof(r));
    memset(g, 0, sizeof(r));
    memset(b, 0, sizeof(r));
}


bool DeviceLedStrip::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, bool &stateChanged)
{
    unsigned index, length, last;

    // set default dummy response size: header only
    p.header.length = sizeof(p.header);

    // check function to perform
    switch (p.header.function_id) {
    case LED_STRIP_FUNCTION_SET_RGB_VALUES:
        index  = p.ledColors.index;
        length = p.ledColors.length;
        last   = index + length;

        if (last > 320 || length > 16) {
            p.setErrorCode(IOPacket::INVALID_PARAMETER);
        }
        else {
            // copy data
            memcpy(r + index, p.ledColors.r, length);
            memcpy(g + index, p.ledColors.g, length);
            memcpy(b + index, p.ledColors.b, length);

            if (last > this->length)
                this->length = last;
        }

        if (length > 0)
            frameRenderedCb.active = true;
        return true;

    case LED_STRIP_FUNCTION_GET_RGB_VALUES:
        index  = p.ledColors.index;
        length = p.ledColors.length;
        last   = index + length;

        if (last > 320 || length > 16) {
            p.setErrorCode(IOPacket::INVALID_PARAMETER);
        }
        else {
            // copy data
            memcpy(p.fullData.payload,      r + index, length);
            memcpy(p.fullData.payload + 16, g + index, length);
            memcpy(p.fullData.payload + 32, b + index, length);

            p.header.length += (3*16);
        }

        return true;

    case LED_STRIP_FUNCTION_SET_FRAME_DURATION:
        frameRenderedCb.period = p.uint16Value;
        return true;

    case LED_STRIP_FUNCTION_GET_FRAME_DURATION:
        p.uint16Value = frameRenderedCb.period;
        p.header.length += sizeof(p.uint16Value);
        return true;

    case LED_STRIP_FUNCTION_GET_SUPPLY_VOLTAGE:
        // voltage with a small random value
        p.uint16Value = 5000 + (relativeTimeMs % 80 - 40);
        p.header.length += sizeof(p.uint16Value);
        return true;

    case LED_STRIP_FUNCTION_SET_CLOCK_FREQUENCY:
        frequency = p.uint32Value;
        return true;

    case LED_STRIP_FUNCTION_GET_CLOCK_FREQUENCY:
        p.uint32Value = frequency;
        p.header.length += sizeof(p.uint32Value);
        return true;

    case LED_STRIP_FUNCTION_SET_CHIP_TYPE:
        chipType = p.uint16Value;
        return true;

    case LED_STRIP_FUNCTION_GET_CHIP_TYPE:
        p.uint16Value = chipType;
        p.header.length += sizeof(p.uint16Value);
        return true;

    default:
        return false;
    }
}

void DeviceLedStrip::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, bool &stateChanged)
{
    if (frameRenderedCb.mayExecute(relativeTimeMs))
    {
        IOPacket packet(uid, frameRenderedCb.callbackCode, sizeof(packet.uint16Value));
        packet.uint16Value = length;
        brickStack->dispatchCallback(packet);
    }
}

} /* namespace stubserver */
