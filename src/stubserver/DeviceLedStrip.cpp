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
#include <utils/Exceptions.h>
#include <utils/Log.h>

#include "DeviceLedStrip.h"
#include "BrickStack.h"


namespace stubserver {

/**
 * Default init.
 */
LedStripState::LedStripState()
   : VisibleDeviceState(VisibleDeviceState::DISCONNECT)
   , numLeds(80)
   , frameLength(0)
{
    memset(r, 0, sizeof(r));
    memset(g, 0, sizeof(r));
    memset(b, 0, sizeof(r));
}

/**
 * Return the char definition of the given customer char as an array of
 * 8 bytes. The index must be in the customer char range (0..7) otherwise
 * NULL will be returned.
 */
int LedStripState::getLedColor(unsigned index) const
{
    if (index >= numLeds)
        throw utils::OutOfRange("LED index invalid", 0, numLeds-1, index);
    return (r[index] << 16) + (g[index] << 8) + b[index];
}


/**
 * Copies the actual frame data into the arguments and returns the frameLength which
 * equals to the number of colors copied into the output.
 */
unsigned LedStripState::getFrameData(uint8_t r[MAX_LEDS], uint8_t g[MAX_LEDS], uint8_t b[MAX_LEDS]) const
{
    if (frameLength > 0)
    {
        memcpy(r, this->r, frameLength);
        memcpy(g, this->g, frameLength);
        memcpy(b, this->b, frameLength);
    }
    return frameLength;
}

/**
 * Default init.
 */
DeviceLedStrip::DeviceLedStrip(const char* _numLeds)
  : chipType(LED_STRIP_CHIP_TYPE_WS2801)
  , frequency(1666666)
  , frameRenderedCb(LED_STRIP_FUNCTION_GET_FRAME_DURATION, LED_STRIP_FUNCTION_SET_FRAME_DURATION, LED_STRIP_CALLBACK_FRAME_RENDERED)
{
    if (_numLeds && *_numLeds) {
        int l = atoi(_numLeds);
        if (l < 32 || l > (int) MAX_LEDS)
            throw utils::OutOfRange("Number of LEDs", l, 32, MAX_LEDS);
        numLeds = l;
        utils::Log() << "Update DeviceLedStrip to " << numLeds << " LEDS";
    }
    else
        utils::Log() << "DeviceLedStrip has " << numLeds << " LEDS";
}

/**
 * Consume IOPaket: Frame update..
 */
bool DeviceLedStrip::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
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

        if (last > numLeds || length > 16) {
            p.setErrorCode(IOPacket::INVALID_PARAMETER);
            if (length > 16)
                utils::Log::error("SET_RGB_VALUES error, length ", length);
            else
                utils::Log::error("SET_RGB_VALUES error, last ", last);
            return true;
        }
        else {
            // copy data
            memcpy(r + index, p.ledColors.r, length);
            memcpy(g + index, p.ledColors.g, length);
            memcpy(b + index, p.ledColors.b, length);
        }

        if (length > 0) {
            unsigned last = index + length;
            if (last > frameLength)
                frameLength = last;
            // utils::Log() << "Update frame-data: length=" << length << ", frameLength=" << frameLength;
        }
        return true;

    case LED_STRIP_FUNCTION_GET_RGB_VALUES:
        index  = p.ledColors.index;
        length = p.ledColors.length;
        last   = index + length;

        if (last > numLeds || length > 16) {
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

void DeviceLedStrip::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &visualizationClient)
{
    if (frameRenderedCb.relativeStartTime > relativeTimeMs)
        return;

    // the next frameRenderedCallback depends on the number of LEDs if this is quite high
    frameRenderedCb.relativeStartTime = relativeTimeMs +
            (numLeds < frameRenderedCb.period ? frameRenderedCb.period : numLeds);

    notify(visualizationClient, VALUE_CHANGE);

    // this callback is more or less always active, except if duration/period is 0
    if (frameRenderedCb.period > 0)
    {
        IOPacket packet(uid, frameRenderedCb.callbackCode, sizeof(packet.uint16Value));
        packet.uint16Value = frameLength;
        brickStack->dispatchCallback(packet);
        // utils::Log() << "DeviceLedStrip callback " << (int) frameRenderedCb.callbackCode << "  frame length " << frameLength;
    }

    // reset length for next frame. CAUTION: at this time the asyn VisualisationClient may still be active !
    frameLength = 0;
}

} /* namespace stubserver */
