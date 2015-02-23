/*
 * DeviceSensor.h
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
#ifndef DEVICESENSOR_H_
#define DEVICESENSOR_H_

#include "DeviceFunctions.h"
#include "VisualizationClient.h"

namespace stubserver {

/**
 * A simulated sensor device which has a 'normal' value and an analog value which
 * is the base for the derived value.
 *
 * This device serves get requests and changed callbacks.
 */
class DeviceSensor : public DeviceFunctions, public SensorState
{
    uint8_t  getValueFunc;
    uint8_t  getValueAnalogFunc;
    unsigned maxAnalogValue;

    ValueProvider *values;
    BasicCallback changedCb;
    BasicCallback changedAnalogCb;
    RangeCallback rangeCallback;

public:
    DeviceSensor(uint8_t _getValueFunc, uint8_t _setCallbackFunc, uint8_t _callbackFunc);
    DeviceSensor(uint8_t _getValueFunc, uint8_t _getValueAnalogFunc, uint8_t _setCallbackFunc,
                 uint8_t _setCallbackFuncAnalog, uint8_t _callbackFunc, uint8_t _callbackFuncAnalog);
    DeviceSensor(ValueProvider *values,
                 uint8_t _getValueFunc, uint8_t _getValueAnalogFunc, uint8_t _setCallbackFunc,
                 uint8_t _setCallbackFuncAnalog, uint8_t _callbackFunc, uint8_t _callbackFuncAnalog);
    ~DeviceSensor();

    /**
     * Changes the max analog value (default is 4095).
     */
    void setMaxAnalogValue(unsigned m) {
        maxAnalogValue = m;
    }

    /**
     * Sets the sensor dependent min / max value.
     */
    void setMinMax(int min, int max) {
        this->min = min;
        this->max = max;
    }

    /**
     * When settings these functions the get/set functions for the range callbacks get active.
     */
    void setRangeCallback(uint8_t _setFunctionCode, uint8_t _getFunctionCode,
                          uint8_t _setDebounceFunctionCode, uint8_t _getDebounceFunctionCode,
                          uint8_t _callbackCode)
    {
        rangeCallback.callbackCode = _callbackCode;
        rangeCallback.setFunctionCode = _setFunctionCode;
        rangeCallback.getFunctionCode = _getFunctionCode;
        rangeCallback.setDebounceFunctionCode = _setDebounceFunctionCode;
        rangeCallback.getDebounceFunctionCode = _getDebounceFunctionCode;
    }

    /**
     * Changes the value provider: must be non-null, the ownership is taken
     * into this object and released when the object is destroyed.
     */
    void setValueProvider(ValueProvider *values);

    DECLARE_OWN_DEVICE_CALLBACKS
};


/**
 * Potentiometer is (when simulated) just a sensor that changes on its own ...
 */
class DevicePotentiometer : public DeviceSensor
{
    bool linear;

public:
    DevicePotentiometer(bool _linear);

    bool isLinear() const {
        return linear;
    }
};


} /* namespace stubserver */

#endif /* DEVICESENSOR_H_ */
