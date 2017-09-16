/*
 * DeviceBrick.h
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

#ifndef STUBSERVER_DEVICEBRICK_H_
#define STUBSERVER_DEVICEBRICK_H_

#include "DeviceVoltageCurrent.h"

namespace stubserver {

/**
 * Base class for all bricks: supports brick supply-voltage and current
 * and the status LED.
 */
class DeviceBrick : public DeviceVoltageCurrent
{
    unsigned deviceType;
    unsigned extenstionType0;   // first master extension
    unsigned extenstionType1;   // seconds master extension

public:
    DeviceBrick(unsigned type, DeviceFunctions* _other, uint8_t _funcGetVoltage, uint8_t _funcGetCurrent);
    DeviceBrick(unsigned type,
                uint8_t _funcGetVoltage, uint8_t _funcGetCurrent,
                uint8_t _funcSetCallbackVoltage, uint8_t _funcSetCallbackCurrent,
                uint8_t _funcCallbackVoltage, uint8_t _funcCallbackCurrent);

    bool consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient) override;
};

} /* namespace stubserver */

#endif /* STUBSERVER_DEVICEBRICK_H_ */
