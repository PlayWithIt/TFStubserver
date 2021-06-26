/*
 * DeviceBrick.cpp
 *
 * Copyright (C) 2015-2020 Holger Grosenick
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

#include <utils/Log.h>
#include <bindings/brick_master.h>
#include <bindings/brick_hat.h>

#include "DeviceBrick.h"


namespace stubserver {

DeviceBrick::DeviceBrick(unsigned typeId, DeviceFunctions* _other, uint8_t _funcGetVoltage, uint8_t _funcGetCurrent)
   : DeviceVoltageCurrent(_other, _funcGetVoltage, _funcGetCurrent)
   , deviceType(typeId)
   , extenstionType0(0)
   , extenstionType1(0)
{
    enableStatusLed(MASTER_FUNCTION_IS_STATUS_LED_ENABLED, MASTER_FUNCTION_ENABLE_STATUS_LED, MASTER_FUNCTION_DISABLE_STATUS_LED);
    setStatusLedConfig(STATUS_LED_ON);
}

DeviceBrick::DeviceBrick(unsigned typeId, uint8_t _funcGetVoltage, uint8_t _funcGetCurrent,
            uint8_t _funcSetCallbackVoltage, uint8_t _funcSetCallbackCurrent,
            uint8_t _funcCallbackVoltage, uint8_t _funcCallbackCurrent)
   : DeviceVoltageCurrent(_funcGetVoltage, _funcGetCurrent, _funcSetCallbackVoltage,
                          _funcSetCallbackCurrent, _funcCallbackVoltage, _funcCallbackCurrent)
   , deviceType(typeId)
   , extenstionType0(0)
   , extenstionType1(0)
{
    enableStatusLed(MASTER_FUNCTION_IS_STATUS_LED_ENABLED, MASTER_FUNCTION_ENABLE_STATUS_LED, MASTER_FUNCTION_DISABLE_STATUS_LED);
}

/**
 * Consume the status LED command
 */
bool DeviceBrick::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    if (deviceType == MASTER_DEVICE_IDENTIFIER)
    {
        // functions only valid for master brick
        switch (p.header.function_id) {
        case MASTER_FUNCTION_REFRESH_WIFI_STATUS:
        case MASTER_FUNCTION_SET_WIFI_CERTIFICATE:
        case FUNCTION_WRITE_BRICKLET_PLUGIN:
            return true;

        case MASTER_FUNCTION_GET_EXTENSION_TYPE:
            p.header.length = sizeof(p.header) + 4;
            p.uint32Value = p.group[0] == 0 ? extenstionType0 : extenstionType1;
            utils::Log() << "Return extension type " << p.uint32Value << " for position " << (p.group[0] == 0 ? '0' : '1');
            return true;

        case MASTER_FUNCTION_GET_CONNECTION_TYPE:
            p.header.length = sizeof(p.header) + 1;
            p.uint8Value = MASTER_CONNECTION_TYPE_USB;
            return true;

        case MASTER_FUNCTION_SET_EXTENSION_TYPE:
            if (p.group[0] == 0)
                extenstionType0 = p.group[1];
            else
                extenstionType1 = p.group[1];
            utils::Log() << "Set extension type " << (int) p.group[1] << " for position " << (p.group[0] == 0 ? '0' : '1');
            return true;

        case MASTER_FUNCTION_GET_WIFI_STATUS:
            bzero(p.fullData.payload, sizeof(p.fullData.payload));
            p.header.length = sizeof(p.header) + 35;
            return true;

        case MASTER_FUNCTION_GET_WIFI_BUFFER_INFO:
            bzero(p.fullData.payload, sizeof(p.fullData.payload));
            p.header.length = sizeof(p.header) + 8;
            return true;

        case MASTER_FUNCTION_IS_CHIBI_PRESENT:
        case MASTER_FUNCTION_IS_WIFI_PRESENT:
        case MASTER_FUNCTION_IS_WIFI2_PRESENT:
        case MASTER_FUNCTION_IS_ETHERNET_PRESENT:
        case MASTER_FUNCTION_IS_RS485_PRESENT:
            p.boolValue = false;
            p.header.length = sizeof(p.header) + 1;
            return true;
        }
    }

    return DeviceVoltageCurrent::consumeCommand(relativeTimeMs, p, visualizationClient);
}


DeviceBrick::DeviceBrick(unsigned typeId, DeviceFunctions* _other)
    : DeviceVoltageCurrent(_other, 0, 0)
    , deviceType(typeId)
    , extenstionType0(0)
    , extenstionType1(0)
{
}

DeviceHatBrick::DeviceHatBrick(unsigned typeId, DeviceFunctions* _other)
  : DeviceBrick(typeId, _other)
  , powerOffDelay(0)
  , powerOffDuration(0)
  , raspberryPiOff(false)
  , brickletsOff(false)
  , enableSleepIndicator(true)
  , brickletPower(true)
{
    isV2 = true;
}

/**
 * Consume the HAT commands
 */
bool DeviceHatBrick::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    p.header.length = sizeof(p.header);

    switch (p.header.function_id) {
    case HAT_FUNCTION_SET_SLEEP_MODE:
        // uint32_t power_off_delay, uint32_t power_off_duration, bool raspberry_pi_off, bool bricklets_off, bool enable_sleep_indicator
        powerOffDelay    = p.hatSleepMode.powerOffDelay;
        powerOffDuration = p.hatSleepMode.powerOffDuration;
        raspberryPiOff   = p.hatSleepMode.raspberryPiOff;
        brickletsOff     = p.hatSleepMode.brickletsOff;
        enableSleepIndicator = p.hatSleepMode.enableSleepIndicator;
        return true;

    case HAT_FUNCTION_GET_SLEEP_MODE:
        // uint32_t power_off_delay, uint32_t power_off_duration, bool raspberry_pi_off, bool bricklets_off, bool enable_sleep_indicator
        p.header.length += 11;
        p.hatSleepMode.powerOffDelay    = powerOffDelay;
        p.hatSleepMode.powerOffDuration = powerOffDuration;
        p.hatSleepMode.raspberryPiOff   = raspberryPiOff;
        p.hatSleepMode.brickletsOff     = brickletsOff;
        p.hatSleepMode.enableSleepIndicator = enableSleepIndicator;
        return true;

    case HAT_FUNCTION_SET_BRICKLET_POWER:
        // bool bricklet_power
        brickletPower = p.boolValue;
        return true;

    case HAT_FUNCTION_GET_BRICKLET_POWER:
        p.header.length += sizeof(bool);
        p.boolValue = brickletPower;
        return true;

    case HAT_FUNCTION_GET_VOLTAGES:
        // uint16_t *ret_voltage_usb, uint16_t *ret_voltage_dc
        p.header.length += 2*sizeof(uint16_t);
        p.shorts.value1 = 0;
        p.shorts.value2 = 5100;
        return true;

    case HAT_FUNCTION_SET_VOLTAGES_CALLBACK_CONFIGURATION:
        p.header.length = sizeof(p.header) + 4;
       // p.uint32Value = p.group[0] == 0 ? extenstionType0 : extenstionType1;
        return true;

    case HAT_FUNCTION_GET_VOLTAGES_CALLBACK_CONFIGURATION:
        p.header.length = sizeof(p.header) + 4;
       // p.uint32Value = p.group[0] == 0 ? extenstionType0 : extenstionType1;
        return true;

    }

    return DeviceVoltageCurrent::consumeCommand(relativeTimeMs, p, visualizationClient);
}

} /* namespace stubserver */
