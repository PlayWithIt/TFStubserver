/*
 * DeviceInOut.cpp
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

#include <bricklet_industrial_digital_in_4.h>
#include <bricklet_io16.h>
#include <bricklet_io4.h>

#include <utils/Log.h>

#include "BrickStack.h"
#include "DeviceInOut.h"

namespace stubserver {

DeviceDigitalIn::EdgeCount::EdgeCount()
  : count(0)
  , debouncePeriod(100)
  , relativeTimeMs(0)
  , active(false)
  , directions(0)
{
}

/**
 * Init digital in with get/set and monoflop.
 */
DeviceDigitalIn::DeviceDigitalIn(utils::ValueProvider *vp, unsigned numPins)
  : DeviceRelay(numPins, true)
  , valueProvider(vp)
  , interruptCallback(INDUSTRIAL_DIGITAL_IN_4_FUNCTION_GET_DEBOUNCE_PERIOD, INDUSTRIAL_DIGITAL_IN_4_FUNCTION_SET_DEBOUNCE_PERIOD, INDUSTRIAL_DIGITAL_IN_4_CALLBACK_INTERRUPT)
  , interruptMask(0)
  , currentValue(0)
{
    interruptCallback.period = 100;
    bzero(edgeCount, sizeof(edgeCount));
}

/**
 * Release old value provider and use the new one.
 */
void DeviceDigitalIn::changeValueProvider(utils::ValueProvider *newValueProvider)
{
    delete valueProvider;
    valueProvider = newValueProvider;
}


bool DeviceDigitalIn::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    // set default dummy response size: header only
    p.header.length = sizeof(p.header);

    // check function to perform
    if (p.header.function_id == interruptCallback.setPeriodFunc) {
        interruptCallback.period = p.uint32Value;
        return true;
    }
    if (p.header.function_id == interruptCallback.getPeriodFunc) {
        p.header.length += sizeof(uint32_t);
        p.uint32Value = interruptCallback.period;
        return true;
    }
    if (p.header.function_id == INDUSTRIAL_DIGITAL_IN_4_FUNCTION_SET_INTERRUPT) {
        // set the interrupt mask
        interruptCallback.param1 = p.uint16Value;
        if (numSwitches == 4)
            interruptCallback.param1 &= 31;
        else if (numSwitches == 8)
            interruptCallback.param1 &= 255;
        interruptCallback.active = (p.uint16Value != 0);
        return true;
    }
    if (p.header.function_id == INDUSTRIAL_DIGITAL_IN_4_FUNCTION_GET_INTERRUPT) {
        // get the interrupt mask
        p.header.length += sizeof(uint16_t);
        p.uint16Value = interruptCallback.param1;
        return true;
    }
    if (p.header.function_id == INDUSTRIAL_DIGITAL_IN_4_FUNCTION_GET_VALUE) {
        // get the interrupt mask
        p.header.length += sizeof(uint16_t);
        p.uint16Value = currentValue;
        return true;
    }

    if (p.header.function_id == INDUSTRIAL_DIGITAL_IN_4_FUNCTION_GET_EDGE_COUNT)
    {
        unsigned index = p.uint8Value;
        if (index >= numSwitches) {
            utils::Log::log("ERROR, invalid switch index", (int)p.uint8Value);
            return false;
        }
        bool reset = p.fullData.payload[1];

        p.header.length += sizeof(uint32_t);
        p.uint32Value = edgeCount[index].count;
        if (reset)
            edgeCount[index].count = 0;
        return true;
    }

    if (p.header.function_id == INDUSTRIAL_DIGITAL_IN_4_FUNCTION_SET_EDGE_COUNT_CONFIG)
    {
        uint16_t selectionMask = p.uint16Value;
        uint8_t edgeType = p.fullData.payload[2];
        uint8_t debounce = p.fullData.payload[3];

        for (unsigned i = 0; i < numSwitches; ++i)
        {
            unsigned m = (1 << i);
            edgeCount[i].active = (selectionMask & m) != 0;
            edgeCount[i].directions = edgeType;
            edgeCount[i].debouncePeriod = debounce;

            //if (edgeCount[i].active)
            //    utils::Log::log("Activate edge count for switch", (int) i);
        }
        return true;
    }
    if (p.header.function_id == INDUSTRIAL_DIGITAL_IN_4_FUNCTION_GET_EDGE_COUNT_CONFIG)
    {
        unsigned index = p.uint8Value;
        if (index >= numSwitches) {
            utils::Log::log("ERROR, invalid switch index", (int)p.uint8Value);
            return false;
        }

        p.header.length += sizeof(uint8_t) * 2;
        p.fullData.payload[0] = edgeCount[index].directions;
        p.fullData.payload[1] = edgeCount[index].debouncePeriod;
        return true;
    }
    return false;
}


void DeviceDigitalIn::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &visualizationClient)
{
    unsigned newValue = valueProvider->getValue(relativeTimeMs);

    if (newValue != currentValue)
    {
        // trigger callback if the last interrupt is after the debounce period
        // -> don't trigger again, if the change is within the debounce period
        unsigned mask = currentValue ^ newValue;
        currentValue = newValue;

        if (interruptCallback.active && (mask & interruptCallback.param1) != 0 &&
                relativeTimeMs > interruptCallback.relativeStartTime + interruptCallback.period)
        {
            // interrupt that return the value.
            IOPacket packet(uid, interruptCallback.callbackCode, sizeof(packet.ushorts));
            packet.ushorts.value1 = mask;
            packet.ushorts.value2 = newValue;
            brickStack->dispatchCallback(packet);

            // utils::Log::log("Change value to", (int) newValue);
            for (unsigned i = 0; i < numSwitches; ++i)
            {
                unsigned m = (1 << i);
                if ((m & mask) == 0 || !edgeCount[i].active)
                    continue;

                uint8_t dir = edgeCount[i].directions;
                if (dir == INDUSTRIAL_DIGITAL_IN_4_EDGE_TYPE_BOTH ||
                        (dir == INDUSTRIAL_DIGITAL_IN_4_EDGE_TYPE_RISING && (newValue & m) != 0) ||
                        (dir == INDUSTRIAL_DIGITAL_IN_4_EDGE_TYPE_FALLING && (newValue & m) == 0))
                {
                    ++edgeCount[i].count;
                }
            }
        }
    }
}

//------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------


DeviceInOut::DeviceInOut(utils::ValueProvider *vp, unsigned numPins)
  : DeviceDigitalIn(vp, numPins)
  , directionMask(numPins == 8 ? 255 : 15)
  , valueMask(0)
{
    functionCodes[FUNC_SET_STATE] = IO4_FUNCTION_SET_VALUE;
    functionCodes[FUNC_SET_SELECTED] = IO4_FUNCTION_SET_SELECTED_VALUES;
    functionCodes[FUNC_GET_STATE] = IO4_FUNCTION_GET_VALUE;
    functionCodes[FUNC_SET_MONOFLOP] = IO4_FUNCTION_SET_MONOFLOP;
    functionCodes[FUNC_GET_MONOFLOP] = IO4_FUNCTION_GET_MONOFLOP;
    initMonoflopCallbacks(IO4_CALLBACK_MONOFLOP_DONE);

    interruptCallback.callbackCode  = IO4_CALLBACK_INTERRUPT;
    interruptCallback.setPeriodFunc = IO4_FUNCTION_SET_DEBOUNCE_PERIOD;
    interruptCallback.getPeriodFunc = IO4_FUNCTION_GET_DEBOUNCE_PERIOD;
}


bool DeviceInOut::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    // set default dummy response size: header only
    p.header.length = sizeof(p.header);

    // check function to perform
    switch(p.header.function_id)
    {
    case IO4_FUNCTION_GET_CONFIGURATION:
        p.header.length += 2;
        p.fullData.payload[0] = directionMask;
        p.fullData.payload[1] = valueMask;
        return true;
    case IO4_FUNCTION_SET_CONFIGURATION:
        directionMask = p.fullData.payload[0];
        valueMask = p.fullData.payload[1];
        return true;
    }

    if (DeviceDigitalIn::consumeCommand(relativeTimeMs, p, visualizationClient))
        return true;

    return DeviceRelay::consumeCommand(relativeTimeMs, p, visualizationClient);
}

void DeviceInOut::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &visualizationClient)
{

}

//------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------

DeviceInOut16::DeviceInOut16(utils::ValueProvider *a, utils::ValueProvider *b)
  : portA(a, 8), portB(b, 8)
{
}


bool DeviceInOut16::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    return false;
}


void DeviceInOut16::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &visualizationClient)
{
    portA.checkCallbacks(relativeTimeMs, uid, brickStack, visualizationClient);
    portB.checkCallbacks(relativeTimeMs, uid, brickStack, visualizationClient);
}

} /* namespace stubserver */
