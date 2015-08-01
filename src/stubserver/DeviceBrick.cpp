/*
 * DeviceBrick.cpp
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

#include <utils/Log.h>
#include <bindings/brick_master.h>

#include "DeviceBrick.h"


namespace stubserver {

DeviceBrick::DeviceBrick(unsigned type, DeviceFunctions* _other, uint8_t _funcGetVoltage, uint8_t _funcGetCurrent)
   : DeviceVoltageCurrent(_other, _funcGetVoltage, _funcGetCurrent)
   , deviceType(type)
{
    setStatusLedOn(true);
}

DeviceBrick::DeviceBrick(unsigned type, uint8_t _funcGetVoltage, uint8_t _funcGetCurrent,
            uint8_t _funcSetCallbackVoltage, uint8_t _funcSetCallbackCurrent,
            uint8_t _funcCallbackVoltage, uint8_t _funcCallbackCurrent)
   : DeviceVoltageCurrent(_funcGetVoltage, _funcGetCurrent, _funcSetCallbackVoltage,
                          _funcSetCallbackCurrent, _funcCallbackVoltage, _funcCallbackCurrent)
   , deviceType(type)
{
    setStatusLedOn(true);
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
        case MASTER_FUNCTION_IS_ETHERNET_PRESENT:
        case MASTER_FUNCTION_IS_RS485_PRESENT:
            p.boolValue = false;
            p.header.length = sizeof(p.header) + 1;
            return true;
        }
    }

    // functions valid for all types of brick
    switch (p.header.function_id) {
    case MASTER_FUNCTION_ENABLE_STATUS_LED:
        setStatusLedOn(true);
        notify(visualizationClient, LED_CHANGE);
        utils::Log::log("Toggle status led ON");
        return true;

    case MASTER_FUNCTION_DISABLE_STATUS_LED:
        setStatusLedOn(false);
        utils::Log::log("Toggle status led OFF");
        notify(visualizationClient, LED_CHANGE);
        return true;

    case MASTER_FUNCTION_IS_STATUS_LED_ENABLED:
        p.boolValue = isStatusLedOn();
        p.header.length = sizeof(p.header) + 1;
        return true;

    case MASTER_FUNCTION_REFRESH_WIFI_STATUS:
        return true;

    default:
        return DeviceVoltageCurrent::consumeCommand(relativeTimeMs, p, visualizationClient);
    }
}

} /* namespace stubserver */
