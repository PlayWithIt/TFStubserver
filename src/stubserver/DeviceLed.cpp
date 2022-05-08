/*
 * DeviceLed.cpp
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

#include "DeviceLed.h"

namespace stubserver {

DeviceLed::DeviceLed(uint8_t _functionGet, uint8_t _functionSet)
    : V2Device(NULL, this, true)
    , functionGet(_functionGet)
    , functionSet(_functionSet)
{
}

DeviceLed::~DeviceLed()
{
}

/**
 * Consume get/set commands (there are no callbacks ...)
 */
bool DeviceLed::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    // set default dummy response size: header only
    p.header.length = sizeof(p.header);

    // check function to perform
    if (p.header.function_id == functionSet) {
        red   = p.fullData.payload[0];
        green = p.fullData.payload[1];
        blue  = p.fullData.payload[2];
        notify(visualizationClient);
        return true;
    }
    if (p.header.function_id == functionGet) {
        p.fullData.payload[0] = red;
        p.fullData.payload[1] = green;
        p.fullData.payload[2] = blue;
        p.header.length += 3;
        return true;
    }

    return V2Device::consumeCommand(relativeTimeMs, p, visualizationClient);
}

} /* namespace stubserver */
