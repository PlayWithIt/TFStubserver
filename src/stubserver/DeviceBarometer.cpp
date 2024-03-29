/*
 * DeviceBarometer.cpp
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

#include <bricklet_barometer.h>
#include <bricklet_barometer_v2.h>

#include "DeviceBarometer.h"

namespace stubserver {


DeviceBarometer::DeviceBarometer(ValueProvider *vp)
  : values(vp)
  , averaging{ 26,11,12,0 }
  , getSet(new GetSet<uint16_t>(BAROMETER_FUNCTION_GET_CHIP_TEMPERATURE, 0, 2430))   // 24.3 degrees
  , changedPressureCb(0, BAROMETER_FUNCTION_SET_AIR_PRESSURE_CALLBACK_PERIOD, BAROMETER_CALLBACK_AIR_PRESSURE, 0)
  , changedHeightCb(0, BAROMETER_FUNCTION_SET_ALTITUDE_CALLBACK_PERIOD, BAROMETER_CALLBACK_ALTITUDE, 0)
{
    if (!values)
        throw utils::Exception("ValueProvider not set!");

    rangeCallback.callbackCode = BAROMETER_CALLBACK_AIR_PRESSURE_REACHED;
    rangeCallback.setThresholdFunctionCode = BAROMETER_FUNCTION_SET_AIR_PRESSURE_CALLBACK_THRESHOLD;
    rangeCallback.getThresholdFunctionCode = BAROMETER_FUNCTION_GET_AIR_PRESSURE_CALLBACK_THRESHOLD;
    rangeCallback.setPeriodFunc = BAROMETER_FUNCTION_SET_DEBOUNCE_PERIOD;
    rangeCallback.getPeriodFunc = BAROMETER_FUNCTION_GET_DEBOUNCE_PERIOD;

    // TODO: still not all functions prepared
    getSet = new GetSetRaw(getSet, BAROMETER_FUNCTION_GET_AVERAGING, BAROMETER_FUNCTION_SET_AVERAGING, 3, averaging);
    getSetRefPressure = new GetSet<int32_t>(getSet, BAROMETER_FUNCTION_GET_REFERENCE_AIR_PRESSURE, BAROMETER_FUNCTION_SET_REFERENCE_AIR_PRESSURE, 1013250);

    min = 500000;
    max = 1400000;
}

DeviceBarometer::~DeviceBarometer()
{
    delete getSetRefPressure;
    delete values;
}

// calculate altitude (in cm) from pressure.
int DeviceBarometer::getAltitude(int pressure) const
{
    int result = getSetRefPressure->getInt() - pressure;
    return result;
}


bool DeviceBarometer::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    uint8_t func = p.header.function_id;
    p.header.length = sizeof(p.header);
    if (func == BAROMETER_FUNCTION_GET_AIR_PRESSURE) {
        p.header.length += 4;
        p.int32Value = sensorValue;
        return true;
    }
    if (func == BAROMETER_FUNCTION_GET_ALTITUDE) {
        p.header.length += 4;
        p.int32Value = getAltitude(sensorValue);
        return true;
    }

    // get/set debounce period
    if (func == rangeCallback.getPeriodFunc) {
        p.header.length += sizeof(uint32_t);
        p.uint32Value = rangeCallback.period;
        return true;
    }
    if (func == rangeCallback.setPeriodFunc) {
        rangeCallback.period = p.uint32Value;
        return true;
    }

    if (func == changedPressureCb.setPeriodFunc) {
        changedPressureCb.period = p.int32Value;
        if (changedPressureCb.period > 0) {
            changedPressureCb.relativeStartTime = relativeTimeMs;
            changedPressureCb.lastValue = sensorValue;
            changedPressureCb.active = true;
        }
        else
            changedPressureCb.active = false;
        return true;
    }
    if (func == changedHeightCb.setPeriodFunc) {
        changedHeightCb.period = p.int32Value;
        if (changedHeightCb.period > 0) {
            changedHeightCb.relativeStartTime = relativeTimeMs;
            changedHeightCb.lastValue = sensorValue;
            changedHeightCb.active = true;
        }
        else
            changedHeightCb.active = false;
        return true;
    }

    // get/set range callback options
    if (rangeCallback.consumeGetSetThreshold(p)) {
        return true;
    }

    // other get set functions
    return getSetRefPressure->consumeCommand(relativeTimeMs, p, visualizationClient);
}


void DeviceBarometer::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &visualizationClient)
{
    int currentValue;

    if (visualizationClient.useAsInputSource()) {
        currentValue = visualizationClient.getInputState();
        if (currentValue != sensorValue)
            sensorValue = currentValue;
    }
    else {
        currentValue = values->getValue(relativeTimeMs);
        if (currentValue != sensorValue)
        {
            sensorValue = currentValue;
            notify(visualizationClient, VALUE_CHANGE);
        }
    }

    if (changedPressureCb.mayExecute(relativeTimeMs) && currentValue != changedPressureCb.lastValue)
    {
        triggerCallbackInt(relativeTimeMs, uid, brickStack, changedPressureCb, currentValue);
        changedPressureCb.lastValue = currentValue;
    }

    // changed callback: if pressure has changed, height has changed too.
    if (changedHeightCb.mayExecute(relativeTimeMs) && currentValue != changedHeightCb.lastValue)
    {
        // calculate height
        int currentHeight = getAltitude(currentValue);
        triggerCallbackInt(relativeTimeMs, uid, brickStack, changedHeightCb, currentHeight);
        changedHeightCb.lastValue = currentValue;
    }

    if (rangeCallback.shouldTriggerRangeCallback(relativeTimeMs, currentValue))
    {
        triggerCallbackInt(relativeTimeMs, uid, brickStack, rangeCallback, currentValue);
    }
}

//------------------------------------------------------------------------------------------------------
// DeviceBarometerV2
//------------------------------------------------------------------------------------------------------


DeviceBarometerV2::DeviceBarometerV2(ValueProvider *_vpAir, utils::ValueProvider *_vpTemp)
  : V2Device(nullptr, this, true)
  , vpAir(_vpAir)
  , vpTemp(_vpTemp)
  , sensorConfig{ 4, 1 }
  , getSet(new GetSet<uint16_t>(BAROMETER_V2_FUNCTION_GET_CHIP_TEMPERATURE, 0, 2430))   // 24.3 degrees
  , cbAirPressure(4)
  , cbTemp(4)
{
    if (!vpAir)
        throw utils::Exception("vpAir not set!");
    if (!vpTemp)
        throw utils::Exception("vpTemp not set!");

    // TODO: still not all functions prepared
    getSet = new GetSetRaw(getSet, BAROMETER_V2_FUNCTION_GET_SENSOR_CONFIGURATION, BAROMETER_V2_FUNCTION_SET_SENSOR_CONFIGURATION, 2, sensorConfig);
    getSet = new GetSet<uint32_t>(getSet, BAROMETER_V2_FUNCTION_GET_MOVING_AVERAGE_CONFIGURATION, BAROMETER_V2_FUNCTION_SET_MOVING_AVERAGE_CONFIGURATION, 100*256 + 100);
    getSetRefPressure = new GetSet<int32_t>(getSet, BAROMETER_V2_FUNCTION_GET_REFERENCE_AIR_PRESSURE, BAROMETER_V2_FUNCTION_SET_REFERENCE_AIR_PRESSURE, 1013250);

    cbAirPressure.setFunctions(BAROMETER_V2_FUNCTION_SET_AIR_PRESSURE_CALLBACK_CONFIGURATION,
                               BAROMETER_V2_FUNCTION_GET_AIR_PRESSURE_CALLBACK_CONFIGURATION,
                               0, 0, BAROMETER_V2_CALLBACK_AIR_PRESSURE);
    cbTemp.setFunctions(BAROMETER_V2_FUNCTION_SET_TEMPERATURE_CALLBACK_CONFIGURATION,
                        BAROMETER_V2_FUNCTION_GET_TEMPERATURE_CALLBACK_CONFIGURATION,
                        0, 0, BAROMETER_V2_CALLBACK_TEMPERATURE);

    min = 500000;
    max = 1400000;
}

DeviceBarometerV2::~DeviceBarometerV2()
{
    delete getSetRefPressure;
    delete vpAir;
    delete vpTemp;
}

// calculate altitude (in cm) from pressure.
int DeviceBarometerV2::getAltitude(int pressure) const
{
    int result = getSetRefPressure->getInt() - pressure;
    return result;
}


bool DeviceBarometerV2::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    uint8_t func = p.header.function_id;
    p.header.length = sizeof(p.header);
    switch (func) {
    case BAROMETER_V2_FUNCTION_GET_AIR_PRESSURE:
        p.header.length += 4;
        p.int32Value = sensorValue;
        return true;

    case BAROMETER_V2_FUNCTION_GET_ALTITUDE:
        p.header.length += 4;
        p.int32Value = getAltitude(sensorValue);
        return true;

    case BAROMETER_V2_FUNCTION_GET_TEMPERATURE:
        p.header.length += 4;
        p.int32Value = sensorValue1;
        return true;
    }

    if (cbAirPressure.consumeGetSetConfig(p))
        return true;
    if (cbTemp.consumeGetSetConfig(p))
        return true;

    // other get set functions
    if (getSetRefPressure->consumeCommand(relativeTimeMs, p, visualizationClient))
        return true;

    return V2Device::consumeCommand(relativeTimeMs, p, visualizationClient);
}


void DeviceBarometerV2::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &visualizationClient)
{
    int newValue;

    //--------------- air pressure callback ---------------------------------------
    if (visualizationClient.useAsInputSource(0))
        newValue = visualizationClient.getInputState(0);
    else
        newValue = vpAir->getValue(relativeTimeMs);

    if (newValue != sensorValue)
    {
        sensorValue = newValue;
        notify(0, visualizationClient, VALUE_CHANGE);
    }
    if (cbAirPressure.shouldTriggerRangeCallback(relativeTimeMs, newValue))
    {
        // printf("cbAirPressure callback %lu (old=%d, new=%d)\n", relativeTimeMs, sensorValue, newValue);
        triggerCallbackInt(relativeTimeMs, uid, brickStack, cbAirPressure, newValue);
    }

    //--------------- temperature callback ------------------------------------------
    if (visualizationClient.useAsInputSource(1))
        newValue = visualizationClient.getInputState(1);
    else
        newValue = vpTemp->getValue(relativeTimeMs);

    if (newValue != sensorValue1)
    {
        cbTemp.valueChanged = true;
        sensorValue = sensorValue1 = newValue;
        notify(1, visualizationClient, VALUE_CHANGE);
    }
    if (cbTemp.shouldTriggerRangeCallback(relativeTimeMs, newValue)) {
        // printf("cbT callback %lu (old=%d, new=%d)\n", relativeTimeMs, sensorValue1, newValue);
        triggerCallbackInt(relativeTimeMs, uid, brickStack, cbTemp, newValue);
    }
}



} /* namespace stubserver */
