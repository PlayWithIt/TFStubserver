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

#include <bricklet_voltage_current.h>
#include <utils/Log.h>

#include "DeviceVoltageCurrent.h"
#include "BrickStack.h"

using utils::Log;
using utils::ValueProvider;
using utils::LinearValueProvider;

namespace stubserver {

/**
 * Init with build-in value provider: this is the default for MasterBrick current/voltage.
 */
DeviceVoltageCurrent::DeviceVoltageCurrent(uint8_t _funcGetVoltage, uint8_t _funcGetCurrent,
                                           uint8_t _funcSetCallbackVoltage, uint8_t _funcSetCallbackCurrent,
                                           uint8_t _funcCallbackVoltage, uint8_t _funcCallbackCurrent)
  : DeviceSensor(_funcGetVoltage, _funcSetCallbackVoltage, _funcCallbackVoltage)
  , amps(_funcGetCurrent, _funcSetCallbackCurrent, _funcCallbackCurrent)
  , powerChangedCb(0, 0, 0, 0)
  , hasPowerSensor(false)
{
    configureCurrentSensor();
}

/**
 * Init for the 'real' voltage/current bricklets.
 */
DeviceVoltageCurrent::DeviceVoltageCurrent()
  : DeviceSensor(VOLTAGE_CURRENT_FUNCTION_GET_VOLTAGE, VOLTAGE_CURRENT_FUNCTION_SET_VOLTAGE_CALLBACK_PERIOD, VOLTAGE_CURRENT_CALLBACK_VOLTAGE)
  , amps(VOLTAGE_CURRENT_FUNCTION_GET_CURRENT, VOLTAGE_CURRENT_FUNCTION_SET_CURRENT_CALLBACK_PERIOD, VOLTAGE_CURRENT_CALLBACK_CURRENT)
  , powerChangedCb(0, VOLTAGE_CURRENT_FUNCTION_SET_POWER_CALLBACK_PERIOD, VOLTAGE_CURRENT_CALLBACK_POWER, 0)
  , hasPowerSensor(true)
{
    configureCurrentSensor();

    setValueSize(4);
    setRangeCallback(VOLTAGE_CURRENT_FUNCTION_SET_VOLTAGE_CALLBACK_THRESHOLD,
                     VOLTAGE_CURRENT_FUNCTION_GET_VOLTAGE_CALLBACK_THRESHOLD,
                     VOLTAGE_CURRENT_FUNCTION_SET_DEBOUNCE_PERIOD,
                     VOLTAGE_CURRENT_FUNCTION_GET_DEBOUNCE_PERIOD,
                     VOLTAGE_CURRENT_CALLBACK_VOLTAGE_REACHED);

    amps.setValueSize(4);
    amps.setMinMax(-20000, 20000);
    amps.setRangeCallback(VOLTAGE_CURRENT_FUNCTION_SET_CURRENT_CALLBACK_THRESHOLD,
                          VOLTAGE_CURRENT_FUNCTION_GET_CURRENT_CALLBACK_THRESHOLD,
                          VOLTAGE_CURRENT_FUNCTION_SET_DEBOUNCE_PERIOD,
                          VOLTAGE_CURRENT_FUNCTION_GET_DEBOUNCE_PERIOD,
                          VOLTAGE_CURRENT_CALLBACK_CURRENT_REACHED);
}

DeviceVoltageCurrent::DeviceVoltageCurrent(DeviceFunctions* _other, uint8_t _funcGetVoltage, uint8_t _funcGetCurrent)
  : DeviceSensor(_funcGetVoltage, 0, 0)
  , amps(_funcGetCurrent, 0, 0)
  , powerChangedCb(0, 0, 0, 0)
  , hasPowerSensor(false)
{
    setOther(_other);
    configureCurrentSensor();
}


void DeviceVoltageCurrent::configureCurrentSensor()
{
    setValueProvider(new LinearValueProvider(11000, 12000, 56, 1200));
    setMinMax(0, 36000);

    amps.setValueProvider(new LinearValueProvider(200, 500, 32, 500));
    amps.setInternalSensorNo(1);
    amps.setMinMax(0, 3000);
}


bool DeviceVoltageCurrent::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    if (DeviceSensor::consumeCommand(relativeTimeMs, p, visualizationClient))
    {
        // debounce period must be applied to both callbacks.
        if (p.header.function_id == rangeCallback.setPeriodFunc)
            amps.consumeCommand(relativeTimeMs, p, visualizationClient);
        return true;
    }
    if (amps.consumeCommand(relativeTimeMs, p, visualizationClient))
        return true;

    uint8_t func = p.header.function_id;
    p.header.length = sizeof(p.header);

    if (hasPowerSensor)
    {
        if (VOLTAGE_CURRENT_FUNCTION_GET_POWER == func) {
            p.header.length = sizeof(p.header) + 4;
            p.uints.value1 = getValue() * amps.getValue() / 1000;
            return true;
        }

        if (powerChangedCb.setPeriodFunc == func) {
            powerChangedCb.period = p.int32Value;
            if (powerChangedCb.period > 0) {
                powerChangedCb.relativeStartTime = relativeTimeMs;

                int power = amps.getValue() * getValue() / 1000;
                powerChangedCb.param1 = power;
                powerChangedCb.active = true;
                Log::log("ADD power callback, period", static_cast<uint64_t>(powerChangedCb.period));
            }
            else
                powerChangedCb.active = false;
            return true;
        }

        // returns 3 bytes:
        // uint8_t averaging;        uint8_t voltage_conversion_time;      uint8_t current_conversion_time;
        if (VOLTAGE_CURRENT_FUNCTION_GET_CONFIGURATION == func) {
            p.header.length = sizeof(p.header) + 3;
            p.fullData.payload[0] = 3;
            p.fullData.payload[1] = 4;
            p.fullData.payload[2] = 4;
            return true;
        }
        if (VOLTAGE_CURRENT_FUNCTION_GET_CALIBRATION == func) {
            p.header.length = sizeof(p.header) + 4;
            p.shorts.value1 = 4474;
            p.shorts.value2 = 4861;
            return true;
        }
        if (VOLTAGE_CURRENT_FUNCTION_SET_CONFIGURATION == func ||
                VOLTAGE_CURRENT_FUNCTION_SET_CALIBRATION == func) {
            return true;
        }
    }

    if (other)
        return other->consumeCommand(relativeTimeMs, p, visualizationClient);

    return false;
}


void DeviceVoltageCurrent::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &visualizationClient)
{
    DeviceSensor::checkCallbacks(relativeTimeMs, uid, brickStack, visualizationClient);
    amps.checkCallbacks(relativeTimeMs, uid, brickStack, visualizationClient);

    int value;

    if (powerChangedCb.mayExecute(relativeTimeMs))
    {
        value = amps.getValue() * getValue() / 1000;
        if (value != powerChangedCb.param1)
        {
            triggerCallbackInt(relativeTimeMs, uid, brickStack, powerChangedCb, value);
            powerChangedCb.param1 = value;
        }
    }

    if (other)
        other->checkCallbacks(relativeTimeMs, uid, brickStack, visualizationClient);
}

} /* namespace stubserver */
