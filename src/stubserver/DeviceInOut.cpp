/*
 * DeviceInOut.cpp
 *
 * Copyright (C) 2013-2021 Holger Grosenick
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
#include <bricklet_industrial_digital_in_4_v2.h>
#include <bricklet_io16.h>
#include <bricklet_io4.h>

#include <utils/Log.h>
#include <utils/Exceptions.h>

#include "BrickStack.h"
#include "DeviceInOut.h"

namespace stubserver {

DeviceDigitalIn::EdgeCount::EdgeCount()
  : count(0)
  , debouncePeriod(0)
  , relativeTimeMs(0)
  , active(false)
  , directions(0)
{
}

/**
 * Init digital in with get/set and monoflop.
 */
DeviceDigitalIn::DeviceDigitalIn(utils::ValueProvider *vp, unsigned numPins)
  : DeviceRelay(numPins, true, false)
  , valueProvider(vp)
  , getValueCallback(INDUSTRIAL_DIGITAL_IN_4_FUNCTION_GET_DEBOUNCE_PERIOD, INDUSTRIAL_DIGITAL_IN_4_FUNCTION_SET_DEBOUNCE_PERIOD, INDUSTRIAL_DIGITAL_IN_4_CALLBACK_INTERRUPT)
  , interruptMask(0)
{
    getValueCallback.period = 100;
}


/**
 * Protected V2 constructor.
 */
DeviceDigitalIn::DeviceDigitalIn(utils::ValueProvider *vp)
  : DeviceRelay(4, false, true)
  , valueProvider(vp)
  , getValueCallback(INDUSTRIAL_DIGITAL_IN_4_V2_FUNCTION_GET_VALUE_CALLBACK_CONFIGURATION, INDUSTRIAL_DIGITAL_IN_4_V2_FUNCTION_SET_VALUE_CALLBACK_CONFIGURATION, INDUSTRIAL_DIGITAL_IN_4_V2_CALLBACK_VALUE)
  , interruptMask(0)
{
    getValueCallback.period = 100;
}

DeviceDigitalIn::~DeviceDigitalIn() {
    delete valueProvider;
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
    if (p.header.function_id == getValueCallback.setPeriodFunc) {
        getValueCallback.period = p.uint32Value;
        return true;
    }
    if (p.header.function_id == getValueCallback.getPeriodFunc) {
        p.header.length += sizeof(uint32_t);
        p.uint32Value = getValueCallback.period;
        return true;
    }
    if (p.header.function_id == INDUSTRIAL_DIGITAL_IN_4_FUNCTION_SET_INTERRUPT) {
        // set the interrupt mask
        getValueCallback.param1 = p.uint16Value;
        if (numSwitches == 4)
            getValueCallback.param1 &= 31;
        else if (numSwitches == 8)
            getValueCallback.param1 &= 255;
        getValueCallback.active = (p.uint16Value != 0);
        return true;
    }
    if (p.header.function_id == INDUSTRIAL_DIGITAL_IN_4_FUNCTION_GET_INTERRUPT) {
        // get the interrupt mask
        p.header.length += sizeof(uint16_t);
        p.uint16Value = getValueCallback.param1;
        return true;
    }
    if (p.header.function_id == INDUSTRIAL_DIGITAL_IN_4_FUNCTION_GET_VALUE) {
        // get the interrupt mask
        p.header.length += sizeof(uint16_t);
        p.uint16Value = getSwitchStates();
        return true;
    }

    if (p.header.function_id == INDUSTRIAL_DIGITAL_IN_4_FUNCTION_GET_EDGE_COUNT)
        return getEdgeCount(p);

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

            // reset on change
            if (edgeCount[i].active)
                edgeCount[i].count = 0;

            //if (edgeCount[i].active)
            //    utils::Log::log("Activate edge count for switch", (int) i);
        }
        return true;
    }
    if (p.header.function_id == INDUSTRIAL_DIGITAL_IN_4_FUNCTION_GET_EDGE_COUNT_CONFIG)
    {
        unsigned index = p.uint8Value;
        if (index >= numSwitches) {
            throw utils::OutOfRange("invalid channel number", p.uint8Value, 3);
        }

        p.header.length += sizeof(uint8_t) * 2;
        p.fullData.payload[0] = edgeCount[index].directions;
        p.fullData.payload[1] = edgeCount[index].debouncePeriod;
        return true;
    }
    return false;
}

/**
 * Return the edge count for one counter
 */
bool DeviceDigitalIn::getEdgeCount(IOPacket &p)
{
    p.header.length += sizeof(uint32_t);

    // uint8_t channel, bool reset_counter, uint32_t *ret_count
    unsigned index = p.uint8Value;
    if (index >= numSwitches)
        throw utils::OutOfRange("invalid channel number", p.uint8Value, 3);

    bool reset = p.fullData.payload[1];
    p.uint32Value = edgeCount[index].count;
    if (reset)
        edgeCount[index].count = 0;
    return true;
}


void DeviceDigitalIn::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &visualizationClient)
{
    unsigned newValue;

    if (visualizationClient.useAsInputSource())
        newValue = visualizationClient.getInputState();
    else
        newValue = valueProvider->getValue(relativeTimeMs);

    if (newValue != getSwitchStates())
    {
        // trigger callback if the last interrupt is after the debounce period
        // -> don't trigger again, if the change is within the debounce period
        unsigned mask = getSwitchStates() ^ newValue;
        setSwitchStates(newValue);

        if (getValueCallback.active && (mask & getValueCallback.param1) != 0 &&
                relativeTimeMs > getValueCallback.relativeStartTime + getValueCallback.period)
        {
            // interrupt that return the value.
            IOPacket packet(uid, getValueCallback.callbackCode, sizeof(packet.ushorts));
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
// DeviceDigitalIn V2
//------------------------------------------------------------------------------------------------

/**
 * Init digital in with get/set and monoflop.
 */
DeviceDigitalInV2::DeviceDigitalInV2(utils::ValueProvider *vp)
  : DeviceDigitalIn(vp)
{
    ledConfig[0] = INDUSTRIAL_DIGITAL_IN_4_V2_CHANNEL_LED_CONFIG_SHOW_CHANNEL_STATUS;
    ledConfig[1] = INDUSTRIAL_DIGITAL_IN_4_V2_CHANNEL_LED_CONFIG_SHOW_CHANNEL_STATUS;
    ledConfig[2] = INDUSTRIAL_DIGITAL_IN_4_V2_CHANNEL_LED_CONFIG_SHOW_CHANNEL_STATUS;
    ledConfig[3] = INDUSTRIAL_DIGITAL_IN_4_V2_CHANNEL_LED_CONFIG_SHOW_CHANNEL_STATUS;
}


bool DeviceDigitalInV2::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    uint8_t channel, config;

    // set default dummy response size: header only
    p.header.length = sizeof(p.header);

    // utils::Log() << "DeviceDigitalInV2::consumeCommand function " << (int) p.header.function_id;

    switch (p.header.function_id) {
    case INDUSTRIAL_DIGITAL_IN_4_V2_FUNCTION_GET_VALUE:
        p.header.length += 1;
        p.fullData.payload[0] = getSwitchStates();
        return true;

    case INDUSTRIAL_DIGITAL_IN_4_V2_FUNCTION_SET_VALUE_CALLBACK_CONFIGURATION:
        channel = p.channelRequest.channel;
        if (channel >= 4)
            throw utils::OutOfRange("invalid channel number", channel, 3);

        // uint32_t period, bool value_has_to_change
        getValueCallbacks[channel].period = p.channelRequest.value1;
        getValueCallbacks[channel].valueHasToChange = p.fullData.payload[6];
        getValueCallbacks[channel].active = getValueCallbacks[channel].period > 0;
        getValueCallbacks[channel].relativeStartTime = relativeTimeMs;
        getValueCallbacks[channel].lastValue = isOn(channel);

        //utils::Log() << "DeviceDigitalInV2 valueCallback[" << (int) channel << "] "
        //             << (getValueCallbacks[channel].active ? "enabled" : "disabled") << " " << getValueCallbacks[channel].period;
        return true;

    case INDUSTRIAL_DIGITAL_IN_4_V2_FUNCTION_GET_VALUE_CALLBACK_CONFIGURATION:
        p.header.length += 5;
        channel = p.channelRequest.channel;
        if (channel >= 4)
            throw utils::OutOfRange("invalid channel number", channel, 3);

        p.callbackConfigInt.period = getValueCallbacks[channel].period;
        p.callbackConfigInt.value_has_to_change = getValueCallbacks[channel].valueHasToChange;
        return true;

    case INDUSTRIAL_DIGITAL_IN_4_V2_FUNCTION_SET_ALL_VALUE_CALLBACK_CONFIGURATION:
        // uint32_t period, bool value_has_to_change
        allValuesCallback.period = p.callbackConfigInt.period;
        allValuesCallback.valueHasToChange = p.callbackConfigInt.value_has_to_change;
        allValuesCallback.active = allValuesCallback.period > 0;
        allValuesCallback.relativeStartTime = relativeTimeMs;
        allValuesCallback.lastValue = getSwitchStates();

        utils::Log() << "DeviceDigitalInV2 allValueCallback " << (allValuesCallback.active ? "enabled" : "disabled") << " " << allValuesCallback.period;
        return true;

    case INDUSTRIAL_DIGITAL_IN_4_V2_FUNCTION_GET_ALL_VALUE_CALLBACK_CONFIGURATION:
        p.header.length += 5;
        p.callbackConfigInt.period = allValuesCallback.period;
        p.callbackConfigInt.value_has_to_change = allValuesCallback.valueHasToChange;
        return true;

    case INDUSTRIAL_DIGITAL_IN_4_V2_FUNCTION_GET_EDGE_COUNT:
        return getEdgeCount(p);

    case INDUSTRIAL_DIGITAL_IN_4_V2_FUNCTION_SET_EDGE_COUNT_CONFIGURATION:
        // uint8_t channel, uint8_t edge_type, uint8_t debounce
        channel = p.fullData.payload[0];
        if (channel >= 4)
            throw utils::OutOfRange("invalid channel number", channel, 3);

        edgeCount[channel].directions = p.fullData.payload[1];
        edgeCount[channel].debouncePeriod = p.fullData.payload[2];
        edgeCount[channel].active = p.fullData.payload[2] != 0;
        edgeCount[channel].count  = 0;
        return true;

    case INDUSTRIAL_DIGITAL_IN_4_V2_FUNCTION_GET_EDGE_COUNT_CONFIGURATION:
        // uint8_t channel, uint8_t *ret_edge_type, uint8_t *ret_debounce
        p.header.length += 2;
        channel = p.fullData.payload[0];
        if (channel >= 4)
            throw utils::OutOfRange("invalid channel number", channel, 3);

        p.fullData.payload[0] = edgeCount[channel].directions;
        p.fullData.payload[1] = edgeCount[channel].debouncePeriod;
        return true;

    case INDUSTRIAL_DIGITAL_IN_4_V2_FUNCTION_SET_CHANNEL_LED_CONFIG:
        channel = p.fullData.payload[0];
        if (channel >= 4)
            throw utils::OutOfRange("invalid channel number", channel, 3);
        config = p.fullData.payload[1];
        ledConfig[channel] = config;

        if (config == INDUSTRIAL_DIGITAL_IN_4_V2_CHANNEL_LED_CONFIG_OFF)
            ledState[channel] = 0;
        else if (config == INDUSTRIAL_DIGITAL_IN_4_V2_CHANNEL_LED_CONFIG_ON)
            ledState[channel] = 1;
        else if (config == INDUSTRIAL_DIGITAL_IN_4_V2_CHANNEL_LED_CONFIG_SHOW_CHANNEL_STATUS)
            ledState[channel] = isOn(channel);
        notify(visualizationClient);
        return true;

    case INDUSTRIAL_DIGITAL_IN_4_V2_FUNCTION_GET_CHANNEL_LED_CONFIG:
        p.header.length += 1;
        channel = p.fullData.payload[0];
        if (channel >= 4)
            throw utils::OutOfRange("invalid channel number", channel, 3);
        p.fullData.payload[0] = ledConfig[channel];
        return true;

    default:
        return V2Device::consumeCommand(relativeTimeMs, p, visualizationClient);
    }
}


void DeviceDigitalInV2::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &visualizationClient)
{
    unsigned newValue;

    if (visualizationClient.useAsInputSource())
        newValue = visualizationClient.getInputState();
    else
        newValue = valueProvider->getValue(relativeTimeMs);
    bool valueChanged = newValue != getSwitchStates();

    if (allValuesCallback.mayExecute(relativeTimeMs) && (valueChanged || !allValuesCallback.valueHasToChange))
    {
        // void callback(bool changed[4], bool value[4], void *user_data)
        // The bools on client side are transported in 2 bytes with 4 bits defining the bool values
        IOPacket packet(uid, INDUSTRIAL_DIGITAL_IN_4_V2_CALLBACK_ALL_VALUE, 2);

        // changed from old to new ?
        packet.fullData.payload[0] = 0;
        if ((allValuesCallback.lastValue & 1) != (newValue & 1))
            packet.fullData.payload[0] |= 1;
        if ((allValuesCallback.lastValue & 2) != (newValue & 2))
            packet.fullData.payload[0] |= 2;
        if ((allValuesCallback.lastValue & 4) != (newValue & 4))
            packet.fullData.payload[0] |= 4;
        if ((allValuesCallback.lastValue & 8) != (newValue & 8))
            packet.fullData.payload[0] |= 8;
        packet.fullData.payload[1] = newValue;
        brickStack->dispatchCallback(packet);

        // utils::Log() << "DeviceDigitalInV2 trigger allValueCallback";

        allValuesCallback.relativeStartTime = relativeTimeMs;
        allValuesCallback.lastValue = newValue;
    }

    if (valueChanged)
    {
        // trigger callback if the last interrupt is after the debounce period
        // -> don't trigger again, if the change is within the debounce period
        unsigned mask = getSwitchStates() ^ newValue;
        setSwitchStates(newValue);

        if (ledConfig[0] == INDUSTRIAL_DIGITAL_IN_4_V2_CHANNEL_LED_CONFIG_SHOW_CHANNEL_STATUS)
            ledState[0] = (newValue & 1);
        if (ledConfig[1] == INDUSTRIAL_DIGITAL_IN_4_V2_CHANNEL_LED_CONFIG_SHOW_CHANNEL_STATUS)
            ledState[1] = (newValue & 2);
        if (ledConfig[2] == INDUSTRIAL_DIGITAL_IN_4_V2_CHANNEL_LED_CONFIG_SHOW_CHANNEL_STATUS)
            ledState[2] = (newValue & 4);
        if (ledConfig[3] == INDUSTRIAL_DIGITAL_IN_4_V2_CHANNEL_LED_CONFIG_SHOW_CHANNEL_STATUS)
            ledState[3] = (newValue & 8);

        // TODO: edge count
        for (unsigned i = 0; i < numSwitches; ++i)
        {
            unsigned m = (1 << i);
            if ((m & mask) == 0 || !edgeCount[i].active)
                continue;

            uint8_t dir = edgeCount[i].directions;
            if (dir == INDUSTRIAL_DIGITAL_IN_4_V2_EDGE_TYPE_BOTH ||
                    (dir == INDUSTRIAL_DIGITAL_IN_4_V2_EDGE_TYPE_RISING && (newValue & m) != 0) ||
                    (dir == INDUSTRIAL_DIGITAL_IN_4_V2_EDGE_TYPE_FALLING && (newValue & m) == 0))
            {
                ++edgeCount[i].count;
            }
        }

        // utils::Log() << "DeviceDigitalInV2::consumeCommand set states to " << newValue;
        notify(visualizationClient);
    }

    // after value update execute single value callbacks
    for (unsigned i = 0; i < 4; ++i)
    {
        if (getValueCallbacks[i].mayExecute(relativeTimeMs) && (valueChanged || !getValueCallbacks[i].valueHasToChange))
        {
            // void callback(uint8_t channel, bool changed, bool value, void *user_data)
            IOPacket packet(uid, INDUSTRIAL_DIGITAL_IN_4_V2_CALLBACK_VALUE, 3);

            // set channel number
            packet.fullData.payload[0] = i;
            // set changed flag
            packet.fullData.payload[1] = getValueCallbacks[i].lastValue != isOn(i);
            // set value flag
            packet.fullData.payload[2] = isOn(i);
            brickStack->dispatchCallback(packet);

            //utils::Log() << "DeviceDigitalInV2 trigger valueCallback(channel " << i << "): " << (int) packet.fullData.payload[1]
            //             << ' ' << (int) packet.fullData.payload[2] << ", states: " << getSwitchStates();

            getValueCallbacks[i].relativeStartTime = relativeTimeMs;
            getValueCallbacks[i].lastValue = isOn(i);
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

    getValueCallback.callbackCode  = IO4_CALLBACK_INTERRUPT;
    getValueCallback.setPeriodFunc = IO4_FUNCTION_SET_DEBOUNCE_PERIOD;
    getValueCallback.getPeriodFunc = IO4_FUNCTION_GET_DEBOUNCE_PERIOD;
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
