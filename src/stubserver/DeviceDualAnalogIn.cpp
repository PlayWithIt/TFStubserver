/*
 * DeviceDualAnalogIn.cpp
 *
 * Copyright (C) 2015-2022 Holger Grosenick
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
#include <bricklet_industrial_dual_analog_in_v2.h>
#include <utils/Log.h>
#include <utils/utils.h>

#include "BrickStack.h"
#include "DeviceDualAnalogIn.h"

namespace stubserver {

/**
 * Init value provider and BasicCallback (RangeCallback is optional).
 */
DeviceDualAnalogIn::SensorData::SensorData(ValueProvider *v, uint8_t cbCode)
   : changedCb(0, 0, cbCode)
   , rangeCallback(4)
   , values(v)
   , channelStatusMin(0)
   , channelStatusMax(0)
   , channelLedConfig(INDUSTRIAL_DUAL_ANALOG_IN_V2_CHANNEL_LED_CONFIG_ON)
{
    setStatusLedConfig(StatusLedConfig::LED_ON);
}

DeviceDualAnalogIn::SensorData::~SensorData()
{
    delete values;
}


/**
 * Init the dual analog in.
 */
DeviceDualAnalogIn::DeviceDualAnalogIn(ValueProvider *v1, ValueProvider *v2, bool v2Enabled)
    : V2Device(nullptr, this, v2Enabled)
    , SensorState(-35000, 35000)
    , getValueFunc(isV2 ? INDUSTRIAL_DUAL_ANALOG_IN_V2_FUNCTION_GET_VOLTAGE : INDUSTRIAL_DUAL_ANALOG_IN_FUNCTION_GET_VOLTAGE)
    , allVoltagesPeriod(0)
    , allVoltagesHasToChange(true)
{
    SensorData *s1 = nullptr;
    SensorData *s2 = nullptr;

    if (isV2) {
        s1 = new SensorData(v1, INDUSTRIAL_DUAL_ANALOG_IN_V2_CALLBACK_VOLTAGE);
        s1->setMinMax(-35 * 1000, 35 * 1000);
        s1->setInternalSensorNo(0);

        s1->rangeCallback.setFunctions(INDUSTRIAL_DUAL_ANALOG_IN_V2_FUNCTION_SET_VOLTAGE_CALLBACK_CONFIGURATION,
                                       INDUSTRIAL_DUAL_ANALOG_IN_V2_FUNCTION_GET_VOLTAGE_CALLBACK_CONFIGURATION,
                                       0, 0, INDUSTRIAL_DUAL_ANALOG_IN_V2_CALLBACK_VOLTAGE);

        s2 = new SensorData(v2, INDUSTRIAL_DUAL_ANALOG_IN_V2_CALLBACK_VOLTAGE);
        s2->rangeCallback = s1->rangeCallback;
        s2->setStatusLedConfig(StatusLedConfig::LED_ON);
        s2->setMinMax(-35 * 1000, 35 * 1000);
        s2->setInternalSensorNo(1);

        other = new GetSet<uint8_t>(INDUSTRIAL_DUAL_ANALOG_IN_V2_FUNCTION_GET_SAMPLE_RATE, INDUSTRIAL_DUAL_ANALOG_IN_V2_FUNCTION_SET_SAMPLE_RATE);

        // 4*4 = 16 byte values
        other = new GetSetRaw(other, INDUSTRIAL_DUAL_ANALOG_IN_V2_FUNCTION_GET_CALIBRATION, INDUSTRIAL_DUAL_ANALOG_IN_V2_FUNCTION_SET_CALIBRATION, 16);

        // 2*4 = 8 byte values
        other = new GetSet<uint64_t>(other, INDUSTRIAL_DUAL_ANALOG_IN_V2_FUNCTION_GET_ADC_VALUES, 0);
    }
    else {
        s1 = new SensorData(v1, INDUSTRIAL_DUAL_ANALOG_IN_CALLBACK_VOLTAGE);
        s1->setStatusLedConfig(StatusLedConfig::LED_OFF);
        s1->setMinMax(-35 * 1000, 35 * 1000);
        s1->setInternalSensorNo(0);
        s1->rangeCallback.setFunctions(INDUSTRIAL_DUAL_ANALOG_IN_FUNCTION_SET_VOLTAGE_CALLBACK_THRESHOLD, INDUSTRIAL_DUAL_ANALOG_IN_FUNCTION_GET_VOLTAGE_CALLBACK_THRESHOLD,
                INDUSTRIAL_DUAL_ANALOG_IN_FUNCTION_SET_DEBOUNCE_PERIOD, INDUSTRIAL_DUAL_ANALOG_IN_FUNCTION_GET_DEBOUNCE_PERIOD,
                INDUSTRIAL_DUAL_ANALOG_IN_CALLBACK_VOLTAGE_REACHED );

        s2 = new SensorData(v2, INDUSTRIAL_DUAL_ANALOG_IN_CALLBACK_VOLTAGE);
        s2->setStatusLedConfig(StatusLedConfig::LED_OFF);
        s2->setMinMax(-35 * 1000, 35 * 1000);
        s2->setInternalSensorNo(1);
        s2->rangeCallback = s1->rangeCallback;

        other = new GetSet<uint8_t>(INDUSTRIAL_DUAL_ANALOG_IN_FUNCTION_GET_SAMPLE_RATE, INDUSTRIAL_DUAL_ANALOG_IN_FUNCTION_SET_SAMPLE_RATE);

        // 4*4 = 16 byte values
        other = new GetSetRaw(other, INDUSTRIAL_DUAL_ANALOG_IN_FUNCTION_GET_CALIBRATION, INDUSTRIAL_DUAL_ANALOG_IN_FUNCTION_SET_CALIBRATION, 16);

        // 2*4 = 8 byte values
        other = new GetSet<uint64_t>(other, INDUSTRIAL_DUAL_ANALOG_IN_FUNCTION_GET_ADC_VALUES, 0);
    }

    sensors.push_back(s1);
    sensors.push_back(s2);
}

/**
 * Clear sensor array
 */
DeviceDualAnalogIn::~DeviceDualAnalogIn()
{
    for (auto it : sensors)
        delete it;
}

/**
 * Check sensor index and return NULL if invalid.
 */
DeviceDualAnalogIn::SensorData* DeviceDualAnalogIn::checkIndex(uint8_t index)
{
    if (index > sensors.size()) {
        utils::Log::error("DeviceMultiSensor: invalid sensor index", index);
        throw utils::OutOfRange("Sensor index", index, sensors.size() - 1);
    }
    return sensors[index];
}

/**
 * Check the command which came in.
 */
bool DeviceDualAnalogIn::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    SensorData *sensor = nullptr;
    uint8_t func = p.header.function_id;
    p.header.length = sizeof(p.header);

    // function used most often: get sensor value
    if (func == getValueFunc) {
        p.header.length = sizeof(p.header) + 4;
        sensor = checkIndex(p.uint8Value);
        p.int32Value = sensor->getSensorValue();
        return true;
    }

    // printf("v2=%d - Func %d\n", isV2, func);

    if (isV2) {
        if (func == INDUSTRIAL_DUAL_ANALOG_IN_V2_FUNCTION_GET_ALL_VOLTAGES) {
            p.header.length = sizeof(p.header) + 8;
            p.ints.value1 = sensors[0]->getSensorValue();
            p.ints.value2 = sensors[1]->getSensorValue();
            return true;
        }
        if (func == INDUSTRIAL_DUAL_ANALOG_IN_V2_FUNCTION_SET_ALL_VOLTAGES_CALLBACK_CONFIGURATION) {
            allVoltagesPeriod = p.callbackConfigInt.period;
            allVoltagesHasToChange = p.callbackConfigInt.value_has_to_change;
            sensors[0]->changedCb.relativeStartTime = relativeTimeMs;
            utils::Log() << utils::base58Encode(p.header.uid) <<": set callback #ALL_VOLTAGES period="
                         << allVoltagesPeriod << " valueHasToChange=" << allVoltagesHasToChange;
            return true;
        }
        if (func == INDUSTRIAL_DUAL_ANALOG_IN_V2_FUNCTION_GET_ALL_VOLTAGES_CALLBACK_CONFIGURATION) {
            p.header.length = sizeof(p.header) + 5;
            p.callbackConfigInt.period = allVoltagesPeriod;
            p.callbackConfigInt.value_has_to_change = allVoltagesHasToChange;
            return true;
        }

        if (func == INDUSTRIAL_DUAL_ANALOG_IN_V2_FUNCTION_GET_CHANNEL_LED_CONFIG) {
            sensor = checkIndex(p.uint8Value);
            p.header.length = sizeof(p.header) + 1;
            p.uint8Value = static_cast<uint8_t>(sensor->getStatusLedConfig());
            return true;
        }
        if (func == INDUSTRIAL_DUAL_ANALOG_IN_V2_FUNCTION_SET_CHANNEL_LED_CONFIG) {
            sensor = checkIndex(p.uint8Value);
            sensor->setStatusLedConfig(p.thresholdChannelInt.option);
            sensor->notify(visualizationClient, VisibleDeviceState::LED_CHANGE);
            return true;
        }
        if (func == INDUSTRIAL_DUAL_ANALOG_IN_V2_FUNCTION_GET_CHANNEL_LED_STATUS_CONFIG) {
            // int32_t *ret_min, int32_t *ret_max, uint8_t *ret_config
            sensor = checkIndex(p.uint8Value);
            p.header.length = sizeof(p.header) + 9;
            p.ints.value1 = sensor->channelStatusMin;
            p.ints.value2 = sensor->channelStatusMax;
            p.touchBits[8] = sensor->channelLedConfig;
            return true;
        }
        if (func == INDUSTRIAL_DUAL_ANALOG_IN_V2_FUNCTION_SET_CHANNEL_LED_STATUS_CONFIG) {
            sensor = checkIndex(p.uint8Value);
            sensor->channelStatusMin = p.channelRequest.value1;
            sensor->channelStatusMax = p.channelRequest.value2;
            sensor->channelLedConfig = p.touchBits[9];
            return true;
        }
        if (func == INDUSTRIAL_DUAL_ANALOG_IN_V2_FUNCTION_SET_VOLTAGE_CALLBACK_CONFIGURATION) {
            // uint8_t channel, uint32_t period, bool value_has_to_change, char option, int32_t min, int32_t max
            sensor = checkIndex(p.uint8Value);

            // move pointer by one byte to skip the channel value
            char *io = reinterpret_cast<char*>(&p);
            ++io;
            IOPacket *pp = reinterpret_cast<IOPacket*>(io);

            sensor->rangeCallback.setOption(pp->callbackConfigInt.option);

            sensor->rangeCallback.valueHasToChange = pp->callbackConfigShort.value_has_to_change;
            sensor->rangeCallback.period = pp->callbackConfigInt.period;
            sensor->rangeCallback.param1 = pp->callbackConfigInt.min;
            sensor->rangeCallback.param2 = pp->callbackConfigInt.max;

            sensor->rangeCallback.logCallbackStatus(p.header.uid);
            return true;
        }
        if (func == INDUSTRIAL_DUAL_ANALOG_IN_V2_FUNCTION_GET_VOLTAGE_CALLBACK_CONFIGURATION) {
            sensor = checkIndex(p.uint8Value);

            p.header.length = sizeof(p.header) + sizeof(p.callbackConfigInt);
            p.callbackConfigInt.option = sensor->rangeCallback.getOption();
            p.callbackConfigInt.period = sensor->rangeCallback.period;
            p.callbackConfigInt.min    = sensor->rangeCallback.param1;
            p.callbackConfigInt.max    = sensor->rangeCallback.param2;
            return true;
        }
    }
    else {
        // get/set debounce period: period is common for both channels
        if (func == INDUSTRIAL_DUAL_ANALOG_IN_FUNCTION_GET_DEBOUNCE_PERIOD) {
            p.header.length = sizeof(p.header) + sizeof(uint32_t);
            p.uint32Value = sensors[0]->rangeCallback.period;
            return true;
        }

        // Debounce period is common for both channels
        if (func == INDUSTRIAL_DUAL_ANALOG_IN_FUNCTION_SET_DEBOUNCE_PERIOD) {
            for (auto it : sensors)
                it->rangeCallback.period = p.uint32Value;
            return true;
        }

        if (func == INDUSTRIAL_DUAL_ANALOG_IN_FUNCTION_SET_VOLTAGE_CALLBACK_PERIOD) {
            p.header.length = sizeof(p.header);
            sensor = checkIndex(p.uint8Value);
            sensor->changedCb.period = p.channelRequest.value1;
            if (sensor->changedCb.period > 0) {
                sensor->changedCb.relativeStartTime = relativeTimeMs;
                sensor->changedCb.lastValue = sensor->getSensorValue();
                sensor->changedCb.active = true;
            }
            else
                sensor->changedCb.active = false;
            return true;
        }

        if (func == INDUSTRIAL_DUAL_ANALOG_IN_FUNCTION_GET_VOLTAGE_CALLBACK_PERIOD) {
            p.header.length = sizeof(p.header) + sizeof(uint32_t);
            sensor = checkIndex(p.uint8Value);
            p.uint32Value = sensor->changedCb.period;
            return true;
        }

        // get/set range callback options
        if (func == INDUSTRIAL_DUAL_ANALOG_IN_FUNCTION_GET_VOLTAGE_CALLBACK_THRESHOLD) {
            sensor = checkIndex(p.uint8Value);
            return sensor->rangeCallback.consumeGetSetThreshold(p);
        }
        if (func == INDUSTRIAL_DUAL_ANALOG_IN_FUNCTION_SET_VOLTAGE_CALLBACK_THRESHOLD) {
            sensor = checkIndex(p.uint8Value);

            p.header.length = sizeof(p.header);
            sensor->rangeCallback.setOption(p.thresholdChannelInt.option);
            sensor->rangeCallback.param1 = p.thresholdChannelInt.min;
            sensor->rangeCallback.param2 = p.thresholdChannelInt.max;
            sensor->rangeCallback.logCallbackStatus(p.header.uid);
            return true;
        }
    }

    return V2Device::consumeCommand(relativeTimeMs, p, visualizationClient);
}


void DeviceDualAnalogIn::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &visualizationClient)
{
    int currentValue;

    for (auto it : sensors)
    {
        unsigned no = it->getInternalSensorNo();
        if (visualizationClient.useAsInputSource(no)) {
            currentValue = visualizationClient.getInputState(no);
            if (currentValue != it->getSensorValue()) {
                it->rangeCallback.valueChanged = true;
                it->setSensorValue(currentValue);
            }
        }
        else {
            currentValue = it->values->getValue(relativeTimeMs);
            if (currentValue != it->getSensorValue())
            {
                it->rangeCallback.valueChanged = true;
                it->setSensorValue(currentValue);
                it->notify(visualizationClient, VisibleDeviceState::VALUE_CHANGE);
            }
        }

        if (it->changedCb.mayExecute(relativeTimeMs) && currentValue != it->changedCb.lastValue)
        {
            IOPacket packet(uid, it->changedCb.callbackCode, 5);
            packet.channelRequest.channel = it->getInternalSensorNo();
            packet.channelRequest.value1 = currentValue;
            brickStack->dispatchCallback(packet);

            it->changedCb.relativeStartTime = relativeTimeMs;
            it->changedCb.lastValue = currentValue;
            it->changedCb.valueChanged = false;
        }

        // shouldTriggerRangeCallback also checks the 'option' value ...
        if (it->rangeCallback.shouldTriggerRangeCallback(relativeTimeMs, currentValue))
        {
            IOPacket packet(uid, it->rangeCallback.callbackCode, 5);
            packet.channelRequest.channel = it->getInternalSensorNo();
            packet.channelRequest.value1 = currentValue;
            brickStack->dispatchCallback(packet);

            it->rangeCallback.relativeStartTime = relativeTimeMs;
            it->rangeCallback.lastValue = currentValue;
            it->rangeCallback.valueChanged = false;
        }
    }

    // is the 'all voltages' callback active and a trigger due ?
    if (isV2 && allVoltagesPeriod > 0 && sensors[0]->changedCb.relativeStartTime + allVoltagesPeriod <= relativeTimeMs) {
        int current = sensors[0]->getSensorValue() + sensors[1]->getSensorValue();
        if (allVoltagesHasToChange) {
            if (sensors[0]->changedCb.lastValue == current) {
                // printf("allVoltagesPeriod unchanged\n");
                return;
            }
        }
        sensors[0]->changedCb.lastValue = current;
        sensors[0]->changedCb.relativeStartTime = relativeTimeMs;

        IOPacket packet(uid, INDUSTRIAL_DUAL_ANALOG_IN_V2_CALLBACK_ALL_VOLTAGES, 8);
        packet.ints.value1 = sensors[0]->getSensorValue();
        packet.ints.value2 = sensors[1]->getSensorValue();
        brickStack->dispatchCallback(packet);
    }
}

} /* namespace stubserver */
