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

#include "CallbackData.h"

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
RangeCallback::RangeCallback()
  : BasicCallback(0, 0, 0, 0)
  , option('x')     // not active
  , setFunctionCode(0)
  , getFunctionCode(0)
  , setDebounceFunctionCode(0)
  , getDebounceFunctionCode(0)
{
    period = 100;
}

void RangeCallback::setFunctions(uint8_t _setFunctionCode, uint8_t _getFunctionCode,
                                 uint8_t _setDebounceFunctionCode, uint8_t _getDebounceFunctionCode,
                                 uint8_t _callbackCode)
{
    callbackCode = _callbackCode;
    setFunctionCode = _setFunctionCode;
    getFunctionCode = _getFunctionCode;
    setDebounceFunctionCode = _setDebounceFunctionCode;
    getDebounceFunctionCode = _getDebounceFunctionCode;
}

/**
 * Check if the function in the packet is get/set threshold
 * @return true if the function was get/set threshold, false otherwise
 */
bool RangeCallback::consumeGetSetThreshold(IOPacket &p)
{
    uint8_t func = p.header.function_id;
    if (func  == getFunctionCode) {
        p.header.length = sizeof(p.header) + sizeof(p.threshold);
        p.threshold.option = getOption();
        p.threshold.min    = param1;
        p.threshold.max    = param2;
        return true;
    }
    if (func == setFunctionCode) {
        p.header.length = sizeof(p.header);
        setOption(p.threshold.option);
        param1 = p.threshold.min;
        param2 = p.threshold.max;
        return true;
    }
    return false;
}

/**
 * Change the callback option, if the option is 'x' or invalid,
 * the callback gets disabled
 * @param o one of 'x', '<', '>', 'o', 'i'
 */
void RangeCallback::setOption(char o)
{
    if (strchr("x<>oi", o))
        option = o;
    else
        option ='x';
    active = option != 'x';
}
