/*
 * CallbackDatas.cpp
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

#include <cstdint>
#include <cstring>

#include <utils/Exceptions.h>
#include <utils/Log.h>
#include <utils/utils.h>

#include "CallbackData.h"

namespace stubserver {

/**
 * Init all attributes
 */
BasicCallback::BasicCallback(uint64_t _relativeStartTime, uint8_t setFuncCode, uint8_t cbCode, int p1, int p2, uint32_t _period)
: relativeStartTime(_relativeStartTime)
, period(_period)
, callbackCode(cbCode)
, getPeriodFunc(0)
, setPeriodFunc(setFuncCode)
, param1(p1)
, param2(p2)
, active(false)
{
}

/**
 * Init all attributes
 */
BasicCallback::BasicCallback(uint8_t getFuncCode, uint8_t setFuncCode, uint8_t cbCode, uint32_t _period)
: relativeStartTime(0)
, period(_period)
, callbackCode(cbCode)
, getPeriodFunc(getFuncCode)
, setPeriodFunc(setFuncCode)
, param1(0)
, param2(0)
, active(false)
{
}

/**
 * Checks if the callback is active and if the current time is after the next
 * planed execution of the callback (relativeStartTime + period).
 */
bool BasicCallback::mayExecute(uint64_t currentTimeMs) const
{
    if (!active)
        return false;
    return (relativeStartTime + period <= currentTimeMs);
}


/**
 * Init all attributes, default callback period is 100ms.
 */
RangeCallback::RangeCallback(uint8_t psize)
  : BasicCallback(0, 0, 0, 0)
  , option('x')     // not active
  , paramSize(psize)
  , setThresholdFunctionCode(0)
  , getThresholdFunctionCode(0)
{
    period = 100;
}

void RangeCallback::setFunctions(uint8_t _setThresholdFunctionCode, uint8_t _getThresholdFunctionCode,
                                 uint8_t _setDebounceFunctionCode, uint8_t _getDebounceFunctionCode,
                                 uint8_t _callbackCode)
{
    callbackCode = _callbackCode;
    setThresholdFunctionCode = _setThresholdFunctionCode;
    getThresholdFunctionCode = _getThresholdFunctionCode;
    setPeriodFunc = _setDebounceFunctionCode;
    getPeriodFunc = _getDebounceFunctionCode;
}

/**
 * Check if the function in the packet is get/set threshold
 * @return true if the function was get/set threshold, false otherwise
 */
bool RangeCallback::consumeGetSetThreshold(IOPacket &p)
{
    uint8_t func = p.header.function_id;
    if (func == getThresholdFunctionCode)
    {
        if (paramSize == 2) {
            p.header.length = sizeof(p.header) + sizeof(p.threshold);
            p.threshold.option = getOption();
            p.threshold.min    = param1;
            p.threshold.max    = param2;
        }
        else {
            p.header.length = sizeof(p.header) + sizeof(p.thresholdInt);
            p.thresholdInt.option = getOption();
            p.thresholdInt.min    = param1;
            p.thresholdInt.max    = param2;
        }
        return true;
    }

    if (func == setThresholdFunctionCode)
    {
        p.header.length = sizeof(p.header);
        setOption(p.threshold.option);

        if (paramSize == 2) {
            param1 = p.threshold.min;
            param2 = p.threshold.max;
        }
        else {
            param1 = p.thresholdInt.min;
            param2 = p.thresholdInt.max;
        }
        logCallbackStatus(p.header.uid);
        return true;
    }
    return false;
}

/**
 * Check if the function in the packet is get/set callback config,
 * this is similar to callback threshold, but structure is a little different.
 * Used in newer V2 devices.
 *
 * @return true if the function was get/set callback config, false otherwise
 */
bool RangeCallback::consumeGetSetConfig(IOPacket &p)
{
    uint8_t func = p.header.function_id;
    if (func == getThresholdFunctionCode)
    {
        if (paramSize == 4) {
            p.header.length = sizeof(p.header) + sizeof(p.callbackConfigInt);
            p.callbackConfigInt.option = getOption();
            p.callbackConfigInt.period = period;
            p.callbackConfigInt.min    = param1;
            p.callbackConfigInt.max    = param2;
            return true;
        }
        if (paramSize == 2) {
            p.header.length = sizeof(p.header) + sizeof(p.callbackConfigShort);
            p.callbackConfigShort.option = getOption();
            p.callbackConfigShort.period = period;
            p.callbackConfigShort.min    = param1;
            p.callbackConfigShort.max    = param2;

            return true;
        }

        throw utils::Exception("RangeCallback::getThreshold config with size != 2 or 4 not supported, size is %d", paramSize);
    }

    if (func == setThresholdFunctionCode)
    {
        if (paramSize == 4) {
            p.header.length = sizeof(p.header);
            setOption(p.callbackConfigInt.option);

            period = p.callbackConfigInt.period;
            param1 = p.callbackConfigInt.min;
            param2 = p.callbackConfigInt.max;

            logCallbackStatus(p.header.uid);
            return true;
        }
        if (paramSize == 2) {
            p.header.length = sizeof(p.header);
            setOption(p.callbackConfigShort.option);

            period = p.callbackConfigShort.period;
            param1 = p.callbackConfigShort.min;
            param2 = p.callbackConfigShort.max;

            logCallbackStatus(p.header.uid);
            return true;
        }

        throw utils::Exception("RangeCallback::setThreshold config with size != 2 or 4 not supported, size is %d", paramSize);
}
    return false;
}

/**
 * Checks if the given time allows a new callback and if yes, checks if the given value
 * is within the given range depending on the range option.
 *
 * @return true if a callback should be triggered
 */
bool RangeCallback::shouldTriggerRangeCallback(uint64_t relativeTimeMs, int currentValue) const
{
    if (option == 'x' && period == 0)
        return false;
    if (relativeStartTime + period > relativeTimeMs)
        return false;

    // From TF-documentation:
    // Wird die Option auf 'x' gesetzt (Threshold abgeschaltet), so wird der Callback mit der festen Periode ausgelöst.

    if (option == 'i' && (currentValue < param1 || currentValue > param2))
        return false;
    if (option == 'o' && currentValue >= param1 && currentValue <= param2)
        return false;
    if (option == '<' && currentValue >= param1)
        return false;
    if (option == '>' && currentValue <= param1)
        return false;

    // fall through: option == 'x' and time reached or range option fulfilled
    return true;
}

/**
 * Print current state into log-file.
 */
void RangeCallback::logCallbackStatus(uint32_t uid) const
{
    utils::Log() << utils::base58Encode(uid) <<": set callback #" << (int) callbackCode << " option '" << option
                 << "', min=" << param1 << " max=" << param2 << " debounce=" << period;
}

/**
 * Change the callback option, if the option is 'x' or invalid,
 * the callback gets disabled
 * @param o one of 'x', '<', '>', 'o', 'i'
 */
void RangeCallback::setOption(char o)
{
    if (o && strchr("x<>oi", o))
        option = o;
    else
        option ='x';
}

} /* namespace stubserver */
