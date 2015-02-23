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
 * Init with build-in value provider
 */
DeviceVoltageCurrent::DeviceVoltageCurrent(uint8_t _funcGetVoltage, uint8_t _funcGetCurrent,
                                           uint8_t _funcSetCallbackVoltage, uint8_t _funcSetCallbackCurrent,
                                           uint8_t _funcCallbackVoltage, uint8_t _funcCallbackCurrent)
  : funcGetVoltage(_funcGetVoltage)
  , funcGetCurrent(_funcGetCurrent)
  , volts(new LinearValueProvider(11000, 12000, 56, 1200))
  , amps(new LinearValueProvider(200, 500, 32, 500))
  , voltsChangedCb(0, _funcSetCallbackVoltage, _funcCallbackVoltage, 0)
  , ampsChangedCb(0, _funcSetCallbackCurrent, _funcCallbackCurrent, 0)
  , powerChangedCb(0, 0, 0, 0)
  , hasPowerSensor(false)
{
}

DeviceVoltageCurrent::DeviceVoltageCurrent()
  : funcGetVoltage(VOLTAGE_CURRENT_FUNCTION_GET_VOLTAGE)
  , funcGetCurrent(VOLTAGE_CURRENT_FUNCTION_GET_CURRENT)
  , volts(new LinearValueProvider(11000, 12000, 56, 1200))
  , amps(new LinearValueProvider(200, 500, 32, 500))
  , voltsChangedCb(0, VOLTAGE_CURRENT_FUNCTION_SET_VOLTAGE_CALLBACK_PERIOD, VOLTAGE_CURRENT_CALLBACK_VOLTAGE, 0)
  , ampsChangedCb(0, VOLTAGE_CURRENT_FUNCTION_SET_CURRENT_CALLBACK_PERIOD, VOLTAGE_CURRENT_CALLBACK_CURRENT, 0)
  , powerChangedCb(0, VOLTAGE_CURRENT_FUNCTION_SET_POWER_CALLBACK_PERIOD, VOLTAGE_CURRENT_CALLBACK_POWER, 0)
  , hasPowerSensor(true)
{
}

DeviceVoltageCurrent::DeviceVoltageCurrent(DeviceFunctions* _other, uint8_t _funcGetVoltage, uint8_t _funcGetCurrent)
  : DeviceFunctions(_other)
  , funcGetVoltage(_funcGetVoltage)
  , funcGetCurrent(_funcGetCurrent)
  , volts(new LinearValueProvider(11000, 12000, 56, 1200))
  , amps(new LinearValueProvider(200, 500, 32, 500))
  , voltsChangedCb(0, 0, 0, 0)
  , ampsChangedCb(0, 0, 0, 0)
  , powerChangedCb(0, 0, 0, 0)
  , hasPowerSensor(false)
{
}


DeviceVoltageCurrent::~DeviceVoltageCurrent()
{
    delete volts;
    delete amps;
}

/**
 * Change the value provider.
 */
void DeviceVoltageCurrent::setVoltageValueProvider(ValueProvider *vp)
{
    delete volts;
    volts = vp;
}

void DeviceVoltageCurrent::setCurrentValueProvider(ValueProvider *vp)
{
    delete amps;
    amps = vp;
}


bool DeviceVoltageCurrent::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    uint8_t func = p.header.function_id;
    p.header.length = sizeof(p.header);

    if (funcGetCurrent == func) {
        p.header.length = sizeof(p.header) + 4;
        p.uints.value1 = amps->getValue(relativeTimeMs);
        return true;
    }
    if (funcGetVoltage == func) {
        p.header.length = sizeof(p.header) + 4;
        p.uints.value1 = volts->getValue(relativeTimeMs);
        return true;
    }
    if (hasPowerSensor && VOLTAGE_CURRENT_FUNCTION_GET_POWER == func) {
        p.header.length = sizeof(p.header) + 4;
        p.uints.value1 = volts->getValue(relativeTimeMs) * amps->getValue(relativeTimeMs) / 1000;
        return true;
    }

    if (ampsChangedCb.setPeriodFunc == func) {
        ampsChangedCb.period = p.int32Value;
        if (ampsChangedCb.period > 0) {
            ampsChangedCb.relativeStartTime = relativeTimeMs;
            ampsChangedCb.param1 = amps->getValue(relativeTimeMs);
            ampsChangedCb.active = true;
            Log::log("ADD current callback, period", static_cast<uint64_t>(ampsChangedCb.period));
        }
        else
            ampsChangedCb.active = false;
        return true;
    }
    if (voltsChangedCb.setPeriodFunc == func) {
        voltsChangedCb.period = p.int32Value;
        if (voltsChangedCb.period > 0) {
            voltsChangedCb.relativeStartTime = relativeTimeMs;
            voltsChangedCb.param1 = volts->getValue(relativeTimeMs);
            voltsChangedCb.active = true;
            Log::log("ADD voltage callback, period", static_cast<uint64_t>(voltsChangedCb.period));
        }
        else
            voltsChangedCb.active = false;
        return true;
    }

    if (hasPowerSensor)
    {
        if (powerChangedCb.setPeriodFunc == func) {
            powerChangedCb.period = p.int32Value;
            if (powerChangedCb.period > 0) {
                powerChangedCb.relativeStartTime = relativeTimeMs;

                int power = amps->getValue(relativeTimeMs) * volts->getValue(relativeTimeMs) / 1000;
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
            p.fullData.payload[0] = 0;
            p.fullData.payload[1] = 0;
            p.fullData.payload[2] = 0;
            return true;
        }
        if (VOLTAGE_CURRENT_FUNCTION_GET_CALIBRATION == func) {
            p.header.length = sizeof(p.header) + 4;
            p.shorts.value1 = 1000;
            p.shorts.value2 = 1000;
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
    int value;
    if (voltsChangedCb.mayExecute(relativeTimeMs) &&
            (value = volts->getValue(relativeTimeMs)) != voltsChangedCb.param1)
    {
        triggerCallbackInt(relativeTimeMs, uid, brickStack, voltsChangedCb, value);
        voltsChangedCb.param1 = value;
    }

    if (ampsChangedCb.mayExecute(relativeTimeMs) &&
            (value = amps->getValue(relativeTimeMs)) != ampsChangedCb.param1)
    {
        triggerCallbackInt(relativeTimeMs, uid, brickStack, ampsChangedCb, value);
        ampsChangedCb.param1 = value;
    }

    if (powerChangedCb.mayExecute(relativeTimeMs))
    {
        value = amps->getValue(relativeTimeMs) * volts->getValue(relativeTimeMs) / 1000;
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
