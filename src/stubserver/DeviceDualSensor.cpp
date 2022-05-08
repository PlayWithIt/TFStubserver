/*
 * DeviceDualSensor.cpp
 *
 * Copyright (C) 2021 Holger Grosenick
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

#include <bricklet_humidity_v2.h>

#include "utils/Log.h"
#include "DeviceDualSensor.h"
#include "BrickStack.h"


namespace stubserver {

/**
 * The ValueProvider needs to provide data per sensor. Other attributes
 * like the callback and function codes needs to be set in derived classes.
 * <p>
 * Humidity V2: vp1 = humidity,  vp2 = temperature
 */
DeviceDualSensor::DeviceDualSensor(ValueProvider *_vp1, ValueProvider *_vp2, unsigned _valueSize, uint8_t _getValue1Func, uint8_t _getValue2Func)
    : V2Device(nullptr, this, true)
    , vp1(_vp1)
    , vp2(_vp2)
    , valueSize(_valueSize)
    , getValue1Func(_getValue1Func)
    , getValue2Func(_getValue2Func)
{
}

DeviceDualSensor::~DeviceDualSensor()
{
    delete vp1;
    delete vp2;
}


bool DeviceDualSensor::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    // set default dummy response size: header only
    p.header.length = sizeof(p.header);

    uint8_t func = p.header.function_id;
    if (func == getValue1Func) {
        p.header.length = sizeof(p.header) + valueSize;
        if (valueSize == 2)
            p.int16Value = sensorValue1;
        else
            p.int32Value = sensorValue1;
        return true;
    }
    if (func == getValue2Func) {
        p.header.length = sizeof(p.header) + valueSize;
        if (valueSize == 2)
            p.int16Value = sensorValue2;
        else
            p.int32Value = sensorValue2;
        return true;
    }

    if (rangeCallback1.consumeGetSetConfig(p))
        return true;
    if (rangeCallback2.consumeGetSetConfig(p))
        return true;

    return V2Device::consumeCommand(relativeTimeMs, p, visualizationClient);
}

/**
 * Check callback for both sensor states.
 */
void DeviceDualSensor::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &visualizationClient)
{
    int currentValue;

    if (visualizationClient.useAsInputSource(0)) {
        currentValue = visualizationClient.getInputState(0);
        if (currentValue != sensorValue1) {
            rangeCallback1.valueChanged = true;
            sensorValue1 = currentValue;
        }
    }
    else {
        currentValue = vp1->getValue(relativeTimeMs);
        if (currentValue != sensorValue1)
        {
            rangeCallback1.valueChanged = true;
            sensorValue = sensorValue1 = currentValue;
            notify(0, visualizationClient, VALUE_CHANGE);
            // utils::Log() << "New value1 " << sensorValue;
        }
    }

    if (rangeCallback1.triggerRangeCallbackIfChanged(relativeTimeMs, sensorValue1))
    {
        if (valueSize == 2)
            triggerCallbackShort(relativeTimeMs, uid, brickStack, rangeCallback1, sensorValue1);
        else
            triggerCallbackInt(relativeTimeMs, uid, brickStack, rangeCallback1, sensorValue1);
    }

    if (visualizationClient.useAsInputSource(1)) {
        currentValue = visualizationClient.getInputState(1);
        if (currentValue != sensorValue2) {
            rangeCallback2.valueChanged = true;
            sensorValue2 = currentValue;
        }
    }
    else {
        currentValue = vp2->getValue(relativeTimeMs);
        if (currentValue != sensorValue2)
        {
            rangeCallback2.valueChanged = true;
            sensorValue = sensorValue2 = currentValue;
            notify(1, visualizationClient, VALUE_CHANGE);
            // utils::Log() << "New value2 " << sensorValue;
        }
    }

    if (rangeCallback2.triggerRangeCallbackIfChanged(relativeTimeMs, sensorValue2))
    {
        if (valueSize == 2)
            triggerCallbackShort(relativeTimeMs, uid, brickStack, rangeCallback2, sensorValue2);
        else
            triggerCallbackInt(relativeTimeMs, uid, brickStack, rangeCallback2, sensorValue2);
    }
}


/**
 * Humidity V2 bricklet:<br>
 * sensorValue1 = humidity,  sensorValue2 = temperature
 */
DeviceHumidityV2::DeviceHumidityV2(ValueProvider *vp1, ValueProvider *vp2)
  : DeviceDualSensor(vp1, vp2, 2, HUMIDITY_V2_FUNCTION_GET_HUMIDITY, HUMIDITY_V2_FUNCTION_GET_TEMPERATURE)
{
    // min/max for humidity
    min = 0;
    max = 100*100;

    other = new GetSet<uint8_t>(HUMIDITY_V2_FUNCTION_GET_HEATER_CONFIGURATION, HUMIDITY_V2_FUNCTION_SET_HEATER_CONFIGURATION);
    other = new GetSet<int32_t>(other, HUMIDITY_V2_FUNCTION_GET_MOVING_AVERAGE_CONFIGURATION, HUMIDITY_V2_FUNCTION_SET_MOVING_AVERAGE_CONFIGURATION);
    other = new GetSet<uint8_t>(other, HUMIDITY_V2_FUNCTION_GET_SAMPLES_PER_SECOND, HUMIDITY_V2_FUNCTION_SET_SAMPLES_PER_SECOND);

    rangeCallback1.setFunctions(HUMIDITY_V2_FUNCTION_SET_HUMIDITY_CALLBACK_CONFIGURATION,
                                HUMIDITY_V2_FUNCTION_GET_HUMIDITY_CALLBACK_CONFIGURATION,
                                0, 0, HUMIDITY_V2_CALLBACK_HUMIDITY);

    rangeCallback2.setFunctions(HUMIDITY_V2_FUNCTION_SET_TEMPERATURE_CALLBACK_CONFIGURATION,
                                HUMIDITY_V2_FUNCTION_GET_TEMPERATURE_CALLBACK_CONFIGURATION,
                                0, 0, HUMIDITY_V2_CALLBACK_TEMPERATURE);
}


} /* namespace stubserver */
