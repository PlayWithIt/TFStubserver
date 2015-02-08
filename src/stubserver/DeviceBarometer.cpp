/*
 * DeviceBarometer.cpp
 *
 *  Created on: 14.12.2013
 *      Author: holger
 */

#include <bricklet_barometer.h>

#include "DeviceBarometer.h"

namespace stubserver {


DeviceBarometer::DeviceBarometer(ValueProvider *vp)
  : values(vp)
  , averaging{ 26,11,12,0 }
  , getSet(new GetSet<uint16_t>(BAROMETER_FUNCTION_GET_CHIP_TEMPERATURE, 0, 2430))   // 24.3 degrees
  , changedPressureCb(0, BAROMETER_FUNCTION_SET_AIR_PRESSURE_CALLBACK_PERIOD, BAROMETER_CALLBACK_AIR_PRESSURE, 0)
  , changedHeightCb(0, BAROMETER_FUNCTION_SET_ALTITUDE_CALLBACK_PERIOD, BAROMETER_CALLBACK_ALTITUDE, 0)
{
    rangeCallback.callbackCode = BAROMETER_CALLBACK_AIR_PRESSURE_REACHED;
    rangeCallback.setFunctionCode = BAROMETER_FUNCTION_SET_AIR_PRESSURE_CALLBACK_THRESHOLD;
    rangeCallback.getFunctionCode = BAROMETER_FUNCTION_GET_AIR_PRESSURE_CALLBACK_THRESHOLD;
    rangeCallback.setDebounceFunctionCode = BAROMETER_FUNCTION_SET_DEBOUNCE_PERIOD;
    rangeCallback.getDebounceFunctionCode = BAROMETER_FUNCTION_GET_DEBOUNCE_PERIOD;

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
    return (getSetRefPressure->getTargetValue() - pressure);
}


bool DeviceBarometer::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualisationClient &visualisationClient)
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
    return getSetRefPressure->consumeCommand(relativeTimeMs, p, visualisationClient);
}


void DeviceBarometer::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualisationClient &visualisationClient)
{
    int currentValue;

    if (visualisationClient.useAsInputSource()) {
        currentValue = visualisationClient.getInputState();
        if (currentValue != sensorValue)
            sensorValue = currentValue;
    }
    else {
        currentValue = values->getValue(relativeTimeMs);
        if (currentValue != sensorValue)
        {
            sensorValue = currentValue;
            notify(visualisationClient, VALUE_CHANGE);
        }
    }

    if (changedPressureCb.mayExecute(relativeTimeMs) && currentValue != changedPressureCb.param1)
    {
        triggerCallbackInt(relativeTimeMs, uid, brickStack, changedPressureCb, currentValue);
        changedPressureCb.param1 = currentValue;
    }

    // changed callback: if pressure has changed, height has changed too.
    // param1 of height callback holds the latest pressure value...
    if (changedHeightCb.mayExecute(relativeTimeMs) && currentValue != changedHeightCb.param1)
    {
        // calculate height
        int currentHeight = getAltitude(currentValue);
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

} /* namespace stubserver */
