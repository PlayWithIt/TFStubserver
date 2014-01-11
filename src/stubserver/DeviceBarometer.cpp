/*
 * DeviceBarometer.cpp
 *
 *  Created on: 14.12.2013
 *      Author: holger
 */

#include <bricklet_barometer.h>

#include "DeviceBarometer.h"


DeviceBarometer::DeviceBarometer()
  : values(new utils::LinearValueProvider(900000, 1100000, 123, 1100))
  , averaging{ 26,11,12,0 }
  , getSet(new GetSet<uint16_t>(BAROMETER_FUNCTION_GET_CHIP_TEMPERATURE, 0, 2430))   // 24.3 degrees
  , changedPressureCb(0, BAROMETER_FUNCTION_SET_AIR_PRESSURE_CALLBACK_PERIOD, BAROMETER_CALLBACK_AIR_PRESSURE, 0)
  , changedHeightCb(0, BAROMETER_FUNCTION_SET_ALTITUDE_CALLBACK_PERIOD, BAROMETER_CALLBACK_ALTITUDE, 0)
{
    rangeCallback.callbackCode = BAROMETER_CALLBACK_AIR_PRESSURE_REACHED;
    rangeCallback.setFunctionCode = BAROMETER_FUNCTION_GET_AIR_PRESSURE_CALLBACK_THRESHOLD;
    rangeCallback.getFunctionCode = BAROMETER_FUNCTION_GET_AIR_PRESSURE_CALLBACK_THRESHOLD;
    rangeCallback.setDebounceFunctionCode = BAROMETER_FUNCTION_SET_DEBOUNCE_PERIOD;
    rangeCallback.getDebounceFunctionCode = BAROMETER_FUNCTION_GET_DEBOUNCE_PERIOD;

    // TODO: still not all functions prepared
    getSet = new GetSetRaw(getSet, BAROMETER_FUNCTION_GET_AVERAGING, BAROMETER_FUNCTION_SET_AVERAGING, 3, averaging);
    getSetRefPressure = new GetSet<int32_t>(getSet, BAROMETER_FUNCTION_GET_REFERENCE_AIR_PRESSURE, BAROMETER_FUNCTION_SET_REFERENCE_AIR_PRESSURE, 1013000);
}

DeviceBarometer::~DeviceBarometer()
{
    delete getSetRefPressure;
    delete values;
}

// calculate altitude from height
int DeviceBarometer::getAltitude(int pressure) const
{
    // TODO: not correct here ...
    return (1013000 - pressure);
}


bool DeviceBarometer::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, bool &stateChanged)
{
    if (values == NULL)
        throw utils::Exception("ValueProvider not set!");

    uint8_t func = p.header.function_id;
    p.header.length = sizeof(p.header);
    if (func == BAROMETER_FUNCTION_GET_AIR_PRESSURE) {
        p.header.length += 4;
        p.int32Value = values->getValue(relativeTimeMs);
        return true;
    }
    if (func == BAROMETER_FUNCTION_GET_ALTITUDE) {
        p.header.length += 4;
        p.int32Value = getAltitude(values->getValue(relativeTimeMs));
        return true;
    }

    // get/set debounce period
    if (func == rangeCallback.getDebounceFunctionCode) {
        p.header.length += sizeof(uint32_t);
        p.uint32Value = rangeCallback.period;
        return true;
    }
    if (func == rangeCallback.setDebounceFunctionCode) {
        rangeCallback.period = p.uint32Value;
        return true;
    }

    if (func == changedPressureCb.setPeriodFunc) {
        changedPressureCb.period = p.int32Value;
        if (changedPressureCb.period > 0) {
            changedPressureCb.relativeStartTime = relativeTimeMs;
            changedPressureCb.param1 = values->getValue(relativeTimeMs);
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
            changedHeightCb.param1 = values->getValue(relativeTimeMs);
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
    return getSetRefPressure->consumeCommand(relativeTimeMs, p, stateChanged);
}


void DeviceBarometer::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, bool &stateChanged)
{
    int currentValue = values->getValue(relativeTimeMs);

    if (changedPressureCb.mayExecute(relativeTimeMs) && currentValue != changedPressureCb.param1)
    {
        triggerCallbackInt(relativeTimeMs, uid, brickStack, changedPressureCb, currentValue);
        changedPressureCb.param1 = currentValue;
    }

    // calculate height
    int currentHeight = getAltitude(currentValue);
    if (changedHeightCb.mayExecute(relativeTimeMs) && currentHeight != changedHeightCb.param1)
    {
        triggerCallbackInt(relativeTimeMs, uid, brickStack, changedHeightCb, currentHeight);
        changedHeightCb.param1 = currentValue;
    }

    if (!rangeCallback.mayExecute(relativeTimeMs))
        return;

    char option = rangeCallback.getOption();
    if ( (option == 'i' && currentValue >= rangeCallback.param1 && currentValue <= rangeCallback.param2)
      || (option == 'o' && (currentValue < rangeCallback.param1 || currentValue > rangeCallback.param2))
      || (option == '<' && currentValue < rangeCallback.param1)
      || (option == '<' && currentValue > rangeCallback.param2)
       )
    {
        triggerCallbackInt(relativeTimeMs, uid, brickStack, rangeCallback, currentValue);
    }
}

/**
 * Changes the value provider
 */
void DeviceBarometer::setValueProvider(utils::ValueProvider *_values)
{
    if (!_values)
        throw utils::Exception("new ValueProvider must be non-null");
    delete values;
    values = _values;
}
