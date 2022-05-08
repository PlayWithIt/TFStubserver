/*
 * DeviceAirQuality.h
 *
 * Copyright (C) 2019-2022 Holger Grosenick
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

#ifndef STUBSERVER_DEVICEAIRQUALITY_H_
#define STUBSERVER_DEVICEAIRQUALITY_H_

#include "DeviceFunctions.h"
#include "VisualizationClient.h"

namespace stubserver {

/**
 * AirQualitySensor with 5 independent values.
 */
class DeviceAirQuality : public V2Device, public SensorState
{
    ValueProvider *vpT;
    ValueProvider *vpH;
    ValueProvider *vpP;
    ValueProvider *vpI;

    RangeCallback cbT;
    RangeCallback cbH;
    RangeCallback cbP;
    RangeCallback cbI;

    int32_t  tempOffset;
    uint32_t cbAPeriod;        // all values callback period
    uint64_t cbAStartTime;
    bool     cbAHasToChange;   // all values values has to change?

public:
    /**
     * The ValueProvider needs to provide data for all channels
     * @param vpT ValueProvider for temperature
     * @param vpH ValueProvider for humidity
     * @param vpP ValueProvider for air pressure
     * @param vpI ValueProvider for IAQ index
     */
    DeviceAirQuality(ValueProvider *vpT, ValueProvider *vpH, ValueProvider *vpP, ValueProvider *vpI);
    virtual ~DeviceAirQuality();

    DECLARE_OWN_DEVICE_CALLBACKS
};

} /* namespace stubserver */

#endif /* STUBSERVER_DEVICEAIRQUALITY_H_ */
