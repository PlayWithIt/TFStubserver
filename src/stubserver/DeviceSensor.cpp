/*
 * DeviceSensor.cpp
 *
 * Copyright (C) 2013-2022 Holger Grosenick
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

namespace stubserver {

using utils::Log;



DeviceSensor::DeviceSensor(uint8_t _getValueFunc, uint8_t _setCallbackFunc, uint8_t _callbackCode)
  : V2Device(nullptr, this, false)
  , getValueFunc(_getValueFunc)
  , getValueAnalogFunc(0)
  , getStatusLedFunc(0)
  , setStatusLedOnFunc(0)
  , setStatusLedOffFunc(0)
  , calibrateZeroFunc(0)
  , maxAnalogValue(4095)
  , valueSize(2)
  , zeroPoint(0)
  , values(NULL)
  , changedCb(0, _setCallbackFunc, _callbackCode, 0)
  , changedAnalogCb(0, 0, 0, 0)
{
}

/**
 * Constructor for V2 devices which have getSetCallbackConfig methods
 */
DeviceSensor::DeviceSensor(uint8_t _getValueFunc, uint8_t _getCallbackConfigFunc, uint8_t _setCallbackConfigFunc, uint8_t _callbackCode)
  : V2Device(nullptr, this, true)
  , getValueFunc(_getValueFunc)
  , getValueAnalogFunc(0)
  , getStatusLedFunc(0)
  , setStatusLedOnFunc(0)
  , setStatusLedOffFunc(0)
  , calibrateZeroFunc(0)
  , maxAnalogValue(4095)
  , valueSize(2)
  , zeroPoint(0)
  , values(NULL)
  , changedCb(0, 0, 0, 0)
  , changedAnalogCb(0, 0, 0, 0)
{
    rangeCallback.setThresholdFunctionCode = _setCallbackConfigFunc;
    rangeCallback.getThresholdFunctionCode = _getCallbackConfigFunc;
    rangeCallback.callbackCode = _callbackCode;
}

DeviceSensor::DeviceSensor(uint8_t _getValueFunc, uint8_t _getValueAnalogFunc, uint8_t _setCallbackFunc,
                           uint8_t _setCallbackFuncAnalog, uint8_t _callbackCode, uint8_t _callbackCodeAnalog)
  : V2Device(nullptr, this, false)
  , getValueFunc(_getValueFunc)
  , getValueAnalogFunc(_getValueAnalogFunc)
  , getStatusLedFunc(0)
  , setStatusLedOnFunc(0)
  , setStatusLedOffFunc(0)
  , calibrateZeroFunc(0)
  , maxAnalogValue(4095)
  , valueSize(2)
  , zeroPoint(0)
  , values(NULL)
  , changedCb(0, _setCallbackFunc, _callbackCode, 0)
  , changedAnalogCb(0, _setCallbackFuncAnalog, _callbackCodeAnalog, 0)
{
}

DeviceSensor::DeviceSensor(ValueProvider *values,
                           uint8_t _getValueFunc, uint8_t _getValueAnalogFunc, uint8_t _setCallbackFunc,
                           uint8_t _setCallbackFuncAnalog, uint8_t _callbackCode, uint8_t _callbackCodeAnalog)
  : V2Device(nullptr, this, false)
  , getValueFunc(_getValueFunc)
  , getValueAnalogFunc(_getValueAnalogFunc)
  , getStatusLedFunc(0)
  , setStatusLedOnFunc(0)
  , setStatusLedOffFunc(0)
  , calibrateZeroFunc(0)
  , maxAnalogValue(4095)
  , valueSize(2)
  , zeroPoint(0)
  , values(values)
  , changedCb(0, _setCallbackFunc, _callbackCode, 0)
  , changedAnalogCb(0, _setCallbackFuncAnalog, _callbackCodeAnalog, 0)
{
    // can be changed later on...
    if (values)
        setMinMax(values->getMin(), values->getMax());
}


DeviceSensor::~DeviceSensor()
{
    delete values;
}


bool DeviceSensor::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    if (values == NULL)
        throw utils::Exception("ValueProvider not set!");

    uint8_t func = p.header.function_id;
    if (func == getValueFunc) {
        p.header.length = sizeof(p.header) + valueSize;
        if (valueSize == 2)
            p.int16Value = sensorValue;
        else
            p.int32Value = sensorValue;
        return true;
    }
    if (func == getValueAnalogFunc && getValueAnalogFunc > 0) {
        p.header.length = sizeof(p.header) + valueSize;
        if (valueSize == 2)
            p.int16Value = calculateAnalogValue();
        else
            p.int32Value = calculateAnalogValue();
        return true;
    }

    // get/set debounce period
    if (func == rangeCallback.getPeriodFunc) {
        p.header.length = sizeof(p.header) + sizeof(uint32_t);
        p.uint32Value = rangeCallback.period;
        return true;
    }
    if (func == rangeCallback.setPeriodFunc) {
        rangeCallback.period = p.uint32Value;
        rangeCallback.logCallbackStatus(p.header.uid);
        return true;
    }

    if (func == changedCb.setPeriodFunc) {
        p.header.length = sizeof(p.header);
        changedCb.period = p.int32Value;
        if (changedCb.period > 0) {
            changedCb.relativeStartTime = relativeTimeMs;
            changedCb.lastValue = sensorValue;
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
            changedAnalogCb.lastValue = sensorValue;
            changedAnalogCb.active = true;
        }
        else
            changedAnalogCb.active = false;
        return true;
    }

    // get/set range callback options
    if (!isV2 && rangeCallback.consumeGetSetThreshold(p)) {
        // TODO: rangeCallback cannot detect if the value was really changed, so might be triggered too often
        rangeCallback.relativeStartTime = relativeTimeMs;
        return true;
    }
    if (isV2 && rangeCallback.consumeGetSetConfig(p)) {
        // TODO: rangeCallback cannot detect if the value was really changed, so might be triggered too often
        rangeCallback.relativeStartTime = relativeTimeMs;
        return true;
    }

    // is the status LED functionality enabled (Bricks + LoadCell + V2 devices) ?
    if (getStatusLedFunc > 0)
    {
        if (isV2) {
            if (func == getStatusLedFunc) {
                p.uint8Value = static_cast<uint8_t>(getStatusLedConfig());
                p.header.length = sizeof(p.header) + 1;
                return true;
            }
            if (func == setStatusLedOnFunc) {
                setStatusLedConfig(p.uint8Value);
                notify(visualizationClient, LED_CHANGE);
                return true;
            }
        }
        if (func == setStatusLedOnFunc) {
            //utils::Log::log("Toggle status led ON");
            setStatusLedConfig(StatusLedConfig::LED_ON);
            notify(visualizationClient, LED_CHANGE);
            return true;
        }
        if (func == setStatusLedOffFunc) {
            //utils::Log::log("Toggle status led OFF");
            setStatusLedConfig(StatusLedConfig::LED_OFF);
            notify(visualizationClient, LED_CHANGE);
            return true;
        }
        if (func == getStatusLedFunc) {
            p.boolValue = isStatusLedOn();
            p.header.length = sizeof(p.header) + 1;
            return true;
        }
    }
    if (func == calibrateZeroFunc) {
        zeroPoint = sensorValue;
        sensorValue = 0;
        notify(visualizationClient, VALUE_CHANGE);
        return true;
    }

    return V2Device::consumeCommand(relativeTimeMs, p, visualizationClient);
}

int DeviceSensor::calculateAnalogValue()
{
    // create an analog value in the range 0..4095
    int v = sensorValue;
    double d = maxAnalogValue;
    if (values->getMax() > values->getMin()) {
        d = d / (values->getMax() - values->getMin()) * static_cast<double>(v);
    }
    return d;
}

void DeviceSensor::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &visualizationClient)
{
    int currentValue;

    if (visualizationClient.useAsInputSource(getInternalSensorNo())) {
        currentValue = visualizationClient.getInputState(getInternalSensorNo());
        if (currentValue != sensorValue) {
            sensorValue = currentValue - zeroPoint;
        }
    }
    else {
        currentValue = values->getValue(relativeTimeMs);
        if (currentValue != sensorValue)
        {
            sensorValue = currentValue - zeroPoint;
            notify(visualizationClient, VALUE_CHANGE);
            // Log() << "New value " << sensorValue;
        }
    }

    if (changedCb.mayExecute(relativeTimeMs) && currentValue != changedCb.lastValue)
    {
        if (valueSize == 4)
            triggerCallbackInt(relativeTimeMs, uid, brickStack, changedCb, currentValue);
        else
            triggerCallbackShort(relativeTimeMs, uid, brickStack, changedCb, currentValue);
        changedCb.lastValue = currentValue;
    }

    if (changedAnalogCb.mayExecute(relativeTimeMs) && currentValue != changedAnalogCb.lastValue)
    {
        // create an analog value in the range 0..4095
        int v = calculateAnalogValue();

        if (valueSize == 4)
            triggerCallbackInt(relativeTimeMs, uid, brickStack, changedAnalogCb, v);
        else
            triggerCallbackShort(relativeTimeMs, uid, brickStack, changedAnalogCb, v);
        changedAnalogCb.lastValue = currentValue;
    }

    if (other)
        other->checkCallbacks(relativeTimeMs, uid, brickStack, visualizationClient);

    // shouldTriggerRangeCallback also checks the 'option' value ...
    if (rangeCallback.shouldTriggerRangeCallback(relativeTimeMs, currentValue))
    {
        // printf("DeviceSensor %p::rangeCallback: current = %5d, time %lu %lu %u\n", this, currentValue, relativeTimeMs, rangeCallback.relativeStartTime, rangeCallback.period);

        if (valueSize == 2)
            triggerCallbackShort(relativeTimeMs, uid, brickStack, rangeCallback, currentValue);
        else
            triggerCallbackInt(relativeTimeMs, uid, brickStack, rangeCallback, currentValue);
    }
}

/**
 * Setting these function codes enables status LED functionality.
 */
void DeviceSensor::enableStatusLed(uint8_t _getStatusLedFunc, uint8_t _setStatusLedOnFunc, uint8_t _setStatusLedOffFunc)
{
    getStatusLedFunc = _getStatusLedFunc;
    setStatusLedOnFunc = _setStatusLedOnFunc;
    setStatusLedOffFunc = _setStatusLedOffFunc;
    setStatusLedConfig(StatusLedConfig::LED_ON);
}

/**
 * Changes the value provider
 */
void DeviceSensor::setValueProvider(ValueProvider *_values)
{
    if (!_values)
        throw utils::Exception("DeviceSensor::setValueProvider: ValueProvider must be non-null");
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

} /* namespace stubserver */
