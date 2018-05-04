/*
 * DeviceDualSensor.cpp
 *
 * Copyright (C) 2015 Holger Grosenick
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

#include <bricklet_industrial_dual_analog_in.h>
#include <utils/Log.h>

#include "BrickStack.h"
#include "DeviceMultiSensor.h"

namespace stubserver {

/**
 * Init value provider and BasicCallback (RangeCallback is optional).
 */
DeviceMultiSensor::SensorData::SensorData(ValueProvider *v, uint8_t getFuncCode, uint8_t setFuncCode, uint8_t cbCode)
   : changedCb(getFuncCode, setFuncCode, cbCode)
   , values(v)
{ }

DeviceMultiSensor::SensorData::~SensorData()
{
    delete values;
}


/**
 * Init MultiSensor yet without SensorData.
 */
DeviceMultiSensor::DeviceMultiSensor(unsigned _valueSize, uint8_t _getValueFunc)
  : valueSize(_valueSize)
  , getValueFunc(_getValueFunc)
{
}

/**
 * Clear sensor array
 */
DeviceMultiSensor::~DeviceMultiSensor()
{
    for (auto it : sensors)
        delete it;
}

/**
 * Check sensor index and return NULL if invalid.
 */
DeviceMultiSensor::SensorData* DeviceMultiSensor::checkIndex(uint8_t index)
{
    if (index > sensors.size()) {
        utils::Log::error("DeviceMultiSensor: invalid sensor index", index);
        return NULL;
    }
    return sensors[index];
}

/**
 * Check the command which came in.
 */
bool DeviceMultiSensor::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    SensorData *sensor = NULL;
    uint8_t func = p.header.function_id;

    if (func == getValueFunc) {
        p.header.length = sizeof(p.header) + valueSize;
        if (NULL == (sensor = checkIndex(p.uint8Value)))
            return true;

        if (valueSize == 2)
            p.int16Value = sensor->getSensorValue();
        else
            p.int32Value = sensor->getSensorValue();
        return true;
    }

    if (NULL == (sensor = checkIndex(0)))
        throw utils::Exception("Number of sensors is 0 -> invalid!");

    // get/set debounce period: period is common for both channels
    if (func == sensor->rangeCallback.getPeriodFunc) {
        p.header.length = sizeof(p.header) + sizeof(uint32_t);
        p.uint32Value = sensors[0]->rangeCallback.period;
        return true;
    }

    // Debounce period is common for both channels
    if (func == sensor->rangeCallback.setPeriodFunc) {
        for (auto it : sensors)
            it->rangeCallback.period = p.uint32Value;
        return true;
    }

    if (func == sensor->changedCb.setPeriodFunc) {
        p.header.length = sizeof(p.header);
        if (NULL == (sensor = checkIndex(p.uint8Value)))
            return true;
        sensor->changedCb.period = p.channelRequest.value1;
        if (sensor->changedCb.period > 0) {
            sensor->changedCb.relativeStartTime = relativeTimeMs;
            sensor->changedCb.param1 = sensor->getSensorValue();
            sensor->changedCb.active = true;
        }
        else
            sensor->changedCb.active = false;
        return true;
    }

    if (func == sensor->changedCb.getPeriodFunc) {
        p.header.length = sizeof(p.header) + sizeof(uint32_t);
        if (NULL == (sensor = checkIndex(p.uint8Value)))
            return true;
        p.uint32Value = sensor->changedCb.period;
        return true;
    }

    // get/set range callback options
    if (func == sensor->rangeCallback.getThresholdFunctionCode) {
        if (NULL == (sensor = checkIndex(p.uint8Value)))
            return true;
        return sensor->rangeCallback.consumeGetSetThreshold(p);
    }
    if (func == sensor->rangeCallback.setThresholdFunctionCode) {
        if (NULL == (sensor = checkIndex(p.thresholdChannelInt.channel)))
            return true;

        p.header.length = sizeof(p.header);
        sensor->rangeCallback.setOption(p.thresholdChannelInt.option);

        if (valueSize == 4) {
            sensor->rangeCallback.param1 = p.thresholdChannelInt.min;
            sensor->rangeCallback.param2 = p.thresholdChannelInt.max;
        }
        else {
            utils::Log::log("DeviceMultiSensor::consumeCommand - missing implementation");
        }
        sensor->rangeCallback.logCallbackStatus(p.header.uid);
        return true;
    }

    if (other)
        return other->consumeCommand(relativeTimeMs, p, visualizationClient);

    return false;
}


void DeviceMultiSensor::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &visualizationClient)
{
    int currentValue;

    if (other)
        other->checkCallbacks(relativeTimeMs, uid, brickStack, visualizationClient);

    for (auto it : sensors)
    {
        unsigned no = it->getInternalSensorNo();
        if (visualizationClient.useAsInputSource(no)) {
            currentValue = visualizationClient.getInputState(no);
            if (currentValue != it->getSensorValue()) {
                it->setSensorValue(currentValue);
            }
        }
        else {
            currentValue = it->values->getValue(relativeTimeMs);
            if (currentValue != it->getSensorValue())
            {
                it->setSensorValue(currentValue);
                it->notify(visualizationClient, VisibleDeviceState::VALUE_CHANGE);
            }
        }

        if (it->changedCb.mayExecute(relativeTimeMs) && currentValue != it->changedCb.param1)
        {
            IOPacket packet(uid, it->changedCb.callbackCode, valueSize + 1);
            packet.channelRequest.channel = it->getInternalSensorNo();
            packet.channelRequest.value1 = currentValue;
            brickStack->dispatchCallback(packet);

            it->changedCb.relativeStartTime = relativeTimeMs;
            it->changedCb.param1 = currentValue;

            if (valueSize != 4) {
                utils::Log::log("DeviceMultiSensor::checkCallbacks - missing implementation");
            }
        }

        // mayExecute also checks the 'option' value ...
        if (it->rangeCallback.mayExecute(relativeTimeMs))
        {
            char option = it->rangeCallback.getOption();
            if ( (option == 'i' && currentValue >= it->rangeCallback.param1 && currentValue <= it->rangeCallback.param2)
                    || (option == 'o' && (currentValue < it->rangeCallback.param1 || currentValue > it->rangeCallback.param2))
                    || (option == '<' && currentValue < it->rangeCallback.param1)
                    || (option == '>' && currentValue > it->rangeCallback.param1)
            )
            {
                IOPacket packet(uid, it->rangeCallback.callbackCode, valueSize + 1);
                packet.channelRequest.channel = it->getInternalSensorNo();
                packet.channelRequest.value1 = currentValue;
                brickStack->dispatchCallback(packet);

                it->rangeCallback.relativeStartTime = relativeTimeMs;

                if (valueSize != 4) {
                    utils::Log::log("DeviceMultiSensor::checkCallbacks - missing implementation");
                }
            }
        }
    }
}

/**
 * Init the dual analog in.
 */
DeviceDualAnalogIn::DeviceDualAnalogIn(ValueProvider *v1, ValueProvider *v2)
   : DeviceMultiSensor(4, INDUSTRIAL_DUAL_ANALOG_IN_FUNCTION_GET_VOLTAGE)
{
    SensorData *s1 = new SensorData(v1, INDUSTRIAL_DUAL_ANALOG_IN_FUNCTION_GET_VOLTAGE_CALLBACK_PERIOD, INDUSTRIAL_DUAL_ANALOG_IN_FUNCTION_SET_VOLTAGE_CALLBACK_PERIOD, INDUSTRIAL_DUAL_ANALOG_IN_CALLBACK_VOLTAGE);

    s1->rangeCallback.setFunctions(INDUSTRIAL_DUAL_ANALOG_IN_FUNCTION_SET_VOLTAGE_CALLBACK_THRESHOLD, INDUSTRIAL_DUAL_ANALOG_IN_FUNCTION_GET_VOLTAGE_CALLBACK_THRESHOLD,
            INDUSTRIAL_DUAL_ANALOG_IN_FUNCTION_SET_DEBOUNCE_PERIOD, INDUSTRIAL_DUAL_ANALOG_IN_FUNCTION_GET_DEBOUNCE_PERIOD,
            INDUSTRIAL_DUAL_ANALOG_IN_CALLBACK_VOLTAGE_REACHED );
    s1->setStatusLedConfig(STATUS_LED_OFF);
    s1->setMinMax(-35 * 1000, 35 * 1000);
    s1->setInternalSensorNo(0);

    SensorData *s2 = new SensorData(v2, INDUSTRIAL_DUAL_ANALOG_IN_FUNCTION_GET_VOLTAGE_CALLBACK_PERIOD, INDUSTRIAL_DUAL_ANALOG_IN_FUNCTION_SET_VOLTAGE_CALLBACK_PERIOD, INDUSTRIAL_DUAL_ANALOG_IN_CALLBACK_VOLTAGE);
    s2->rangeCallback = s1->rangeCallback;
    s2->setStatusLedConfig(STATUS_LED_OFF);
    s2->setMinMax(-35 * 1000, 35 * 1000);
    s2->setInternalSensorNo(1);

    sensors.push_back(s1);
    sensors.push_back(s2);

    other = new GetSet<uint8_t>(INDUSTRIAL_DUAL_ANALOG_IN_FUNCTION_GET_SAMPLE_RATE, INDUSTRIAL_DUAL_ANALOG_IN_FUNCTION_SET_SAMPLE_RATE, 0);
}

} /* namespace stubserver */
