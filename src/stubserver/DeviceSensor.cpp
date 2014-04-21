/*
 * DeviceVoltageCurrent.cpp
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

#include <bricklet_rotary_poti.h>
#include <bricklet_linear_poti.h>
#include <utils/Log.h>
#include <utils/Exceptions.h>

#include "DeviceSensor.h"
#include "BrickStack.h"

using utils::Log;



DeviceSensor::DeviceSensor(uint8_t _getValueFunc, uint8_t _setCallbackFunc, uint8_t _callbackCode)
  : getValueFunc(_getValueFunc)
  , getValueAnalogFunc(0)
  , maxAnalogValue(4095)
  , values(NULL)
  , changedCb(0, _setCallbackFunc, _callbackCode, 0)
  , changedAnalogCb(0, 0, 0, 0)
{
}

DeviceSensor::DeviceSensor(uint8_t _getValueFunc, uint8_t _getValueAnalogFunc, uint8_t _setCallbackFunc,
                           uint8_t _setCallbackFuncAnalog, uint8_t _callbackCode, uint8_t _callbackCodeAnalog)
  : getValueFunc(_getValueFunc)
  , getValueAnalogFunc(_getValueAnalogFunc)
  , maxAnalogValue(4095)
  , values(NULL)
  , changedCb(0, _setCallbackFunc, _callbackCode, 0)
  , changedAnalogCb(0, _setCallbackFuncAnalog, _callbackCodeAnalog, 0)
{
}

DeviceSensor::DeviceSensor(ValueProvider *values,
                           uint8_t _getValueFunc, uint8_t _getValueAnalogFunc, uint8_t _setCallbackFunc,
                           uint8_t _setCallbackFuncAnalog, uint8_t _callbackCode, uint8_t _callbackCodeAnalog)
  : getValueFunc(_getValueFunc)
  , getValueAnalogFunc(_getValueAnalogFunc)
  , maxAnalogValue(4095)
  , values(values)
  , changedCb(0, _setCallbackFunc, _callbackCode, 0)
  , changedAnalogCb(0, _setCallbackFuncAnalog, _callbackCodeAnalog, 0)
{
}


DeviceSensor::~DeviceSensor()
{
    delete values;
}


bool DeviceSensor::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, bool &stateChanged)
{
    if (values == NULL)
        throw utils::Exception("ValueProvider not set!");

    uint8_t func = p.header.function_id;
    if (func == getValueFunc) {
        p.header.length = sizeof(p.header) + 2;
        p.int16Value = values->getValue(relativeTimeMs);
        return true;
    }
    if (func == getValueAnalogFunc && getValueAnalogFunc > 0) {
        p.header.length = sizeof(p.header) + 2;
        p.int16Value = values->getValue(relativeTimeMs);
        return true;
    }

    // get/set debounce period
    if (func == rangeCallback.getDebounceFunctionCode) {
        p.header.length = sizeof(p.header) + sizeof(uint32_t);
        p.uint32Value = rangeCallback.period;
        return true;
    }
    if (func == rangeCallback.setDebounceFunctionCode) {
        rangeCallback.period = p.uint32Value;
        return true;
    }

    if (func == changedCb.setPeriodFunc) {
        p.header.length = sizeof(p.header);
        changedCb.period = p.int32Value;
        if (changedCb.period > 0) {
            changedCb.relativeStartTime = relativeTimeMs;
            changedCb.param1 = values->getValue(relativeTimeMs);
            changedCb.active = true;
        }
        else
            changedCb.active = false;
        return true;
    }
    if (func == changedAnalogCb.setPeriodFunc && changedAnalogCb.setPeriodFunc > 0) {
        p.header.length = sizeof(p.header);
        changedAnalogCb.period = p.int32Value;
        if (changedAnalogCb.period > 0) {
            changedAnalogCb.relativeStartTime = relativeTimeMs;
            changedAnalogCb.param1 = values->getValue(relativeTimeMs);
            changedAnalogCb.active = true;
        }
        else
            changedAnalogCb.active = false;
        return true;
    }

    // get/set range callback options
    if (rangeCallback.consumeGetSetThreshold(p)) {
        return true;
    }

    return false;
}


void DeviceSensor::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, bool &stateChanged)
{
    int currentValue = values->getValue(relativeTimeMs);

    if (changedCb.mayExecute(relativeTimeMs) && currentValue != changedCb.param1)
    {
        triggerCallbackShort(relativeTimeMs, uid, brickStack, changedCb, currentValue);
        changedCb.param1 = currentValue;
    }

    if (changedAnalogCb.mayExecute(relativeTimeMs) && currentValue != changedAnalogCb.param1)
    {
        // create an analog value in the range 0..4095
        short v = currentValue;
        double d = maxAnalogValue + 1.0;
        d = d / (values->getMax() - values->getMin()) * (double) v;
        v = d;

        triggerCallbackShort(relativeTimeMs, uid, brickStack, changedAnalogCb, v);
        changedAnalogCb.param1 = v;
    }

    // mayExecute also checks the 'option' value ...
    if (!rangeCallback.mayExecute(relativeTimeMs))
        return;

    char option = rangeCallback.getOption();
    if ( (option == 'i' && currentValue >= rangeCallback.param1 && currentValue <= rangeCallback.param2)
      || (option == 'o' && (currentValue < rangeCallback.param1 || currentValue > rangeCallback.param2))
      || (option == '<' && currentValue < rangeCallback.param1)
      || (option == '>' && currentValue > rangeCallback.param1)
       )
    {
        triggerCallbackShort(relativeTimeMs, uid, brickStack, rangeCallback, currentValue);
    }
}

/**
 * Changes the value provider
 */
void DeviceSensor::setValueProvider(ValueProvider *_values)
{
    if (!_values)
        throw utils::Exception("new ValueProvider must be non-null");
    delete values;
    values = _values;
}

/**
 * Potentiometer is (when simulated) just a sensor that changes on its own.
 * Linear poty and rotary poti have exactly the same function codes ...
 * So we can use just one type here.
 */
DevicePotentiometer::DevicePotentiometer(bool _linear)
  : DeviceSensor(_linear ?
                 new utils::LinearValueProvider(0, 100, 1, 200) : new utils::LinearValueProvider(-150, 150, 3, 200),
                 ROTARY_POTI_FUNCTION_GET_POSITION,
                 ROTARY_POTI_FUNCTION_GET_ANALOG_VALUE,
                 ROTARY_POTI_FUNCTION_SET_POSITION_CALLBACK_PERIOD,
                 ROTARY_POTI_FUNCTION_SET_ANALOG_VALUE_CALLBACK_PERIOD,
                 ROTARY_POTI_CALLBACK_POSITION,
                 ROTARY_POTI_CALLBACK_ANALOG_VALUE)
  , linear(_linear)
{
}
