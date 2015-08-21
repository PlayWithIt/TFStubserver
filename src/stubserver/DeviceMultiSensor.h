/*
 * DeviceDualSensor.h
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

#ifndef STUBSERVER_DEVICEMULTISENSOR_H_
#define STUBSERVER_DEVICEMULTISENSOR_H_

#include <utils/MinMax.h>

#include "DeviceFunctions.h"
#include "VisualizationClient.h"

namespace stubserver {

/**
 * A simulated device which supports multiple independent sensors values.
 * <P>
 * The major difference to the {@link DeviceSensor} is that the sensor number is
 * part of the call arguments, not part of the function code.
 */
class DeviceMultiSensor : public DeviceFunctions
{
protected:
    /**
     * Per sensor data.
     */
    class SensorData : public SensorState
    {
    public:
        BasicCallback   changedCb;
        RangeCallback   rangeCallback;
        ValueProvider  *values;

        void setSensorValue(int v) {
            sensorValue = v;
        }

        void setMinMax(int _min, int _max) {
            min = _min;
            max = _max;
        }

        /**
         * Init value provider and BasicCallback (RangeCallback is optional).
         */
        SensorData(ValueProvider *v, uint8_t getFuncCode, uint8_t setFuncCode, uint8_t cbCode);
        ~SensorData();
    };

    std::vector<SensorData*> sensors;

    unsigned   valueSize;      // size in bytes: 1,2 or 4 bytes
    uint8_t    getValueFunc;

    /**
     * Check sensor index and return NULL if invalid.
     */
    SensorData* checkIndex(uint8_t index);

public:
    DeviceMultiSensor(unsigned _valueSize, uint8_t _getValueFunc);
    ~DeviceMultiSensor();

    DECLARE_OWN_DEVICE_CALLBACKS
};

/**
 * Industrial dual analog in.
 */
class DeviceDualAnalogIn : public DeviceMultiSensor
{
public:
    DeviceDualAnalogIn(ValueProvider *v1, ValueProvider *v2);
};

} /* namespace stubserver */

#endif /* STUBSERVER_DEVICEMULTISENSOR_H_ */
