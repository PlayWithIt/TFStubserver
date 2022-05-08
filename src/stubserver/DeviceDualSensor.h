/*
 * DeviceDualSensor.h
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

#ifndef STUBSERVER_DEVICEDUALSENSOR_H_
#define STUBSERVER_DEVICEDUALSENSOR_H_

#include "DeviceFunctions.h"
#include "VisualizationClient.h"


namespace stubserver {

/**
 * Device with two independent sensor values that can have callbacks and getter functions.
 */
class DeviceDualSensor : public V2Device, public SensorState
{
    ValueProvider *vp1;
    ValueProvider *vp2;
    unsigned      valueSize;

protected:
    RangeCallback rangeCallback1;
    RangeCallback rangeCallback2;

    uint8_t getValue1Func;
    uint8_t getValue2Func;

public:
    /**
     * The ValueProvider needs to provide data per sensor. Other attributes
     * like the callback and function codes needs to be set in derived classes.
     * <p>
     * Humidity V2: vp1 = humidity,  vp2 = temperature
     */
    DeviceDualSensor(ValueProvider *vp1, ValueProvider *vp2, unsigned valueSize, uint8_t getValue1Func, uint8_t getValue2Func);
    virtual ~DeviceDualSensor();

    DECLARE_OWN_DEVICE_CALLBACKS
};

/**
 * Humidity V2 bricklet:<br>
 * sensorValue1 = humidity,  sensorValue2 = temperature
 */
class DeviceHumidityV2 : public DeviceDualSensor
{
public:
    /**
     * Bricklet values:
     * vp1 / sensorValue1 = humidity,  vp2 / sensorValue2 = temperature
     */
    DeviceHumidityV2(ValueProvider *vp1, ValueProvider *vp2);

};

} /* namespace stubserver */

#endif /* STUBSERVER_DEVICEDUALSENSOR_H_ */
