/*
 * DeviceHallEffect.h
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

#ifndef STUBSERVER_DEVICEHALLEFFECT_H_
#define STUBSERVER_DEVICEHALLEFFECT_H_

#include "DeviceFunctions.h"
#include "VisualizationClient.h"

namespace stubserver {

/**
 * Hall effect bricklet.
 */
class DeviceHallEffect : public DeviceFunctions, public SensorState
{
    ValueProvider *valueProvider;
    uint64_t       latestCallback;
    uint32_t       callbackPeriod;
    uint32_t       latestInterrupt;
    uint32_t       edgeInterrupt;
    uint8_t        edgeConfig;
    uint8_t        debounce;

public:
    /**
     * The ValueProvider just needs to provide value 0,1 or 2.
     */
    DeviceHallEffect(ValueProvider *vp);
    ~DeviceHallEffect();

    DECLARE_OWN_DEVICE_CALLBACKS
};

} /* namespace stubserver */

#endif /* STUBSERVER_DEVICEHALLEFFECT_H_ */
