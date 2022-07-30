/*
 * DeviceDualAnalogIn.h
 *
 * Copyright (C) 2015-2022 Holger Grosenick
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

#ifndef STUBSERVER_DEVICEDUALANALOGIN_H_
#define STUBSERVER_DEVICEDUALANALOGIN_H_

#include <utils/MinMax.h>

#include "DeviceFunctions.h"
#include "VisualizationClient.h"

namespace stubserver {

/**
 * DeviceDualAnalogIn: a simulated device which supports multiple independent sensors values
 * (two in this case) and some LED features per sensor channel.
 * <P>
 * The major difference to the simple {@link DeviceDualSensor} is that the sensor number is
 * part of the call arguments, not part of the function code.
 */
class DeviceDualAnalogIn : public V2Device, public SensorState
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
        int            channelStatusMin, channelStatusMax;
        uint8_t        channelLedConfig;

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
        SensorData(ValueProvider *v, uint8_t cbCode);
        ~SensorData();
    };

    std::vector<SensorData*> sensors;
    uint8_t                  getValueFunc;
    uint32_t                 allVoltagesPeriod;
    bool                     allVoltagesHasToChange;

    /**
     * Check sensor index and throws an exception if invalid.
     */
    SensorData* checkIndex(uint8_t index);

public:
    DeviceDualAnalogIn(ValueProvider *v1, ValueProvider *v2, bool isV2);
    ~DeviceDualAnalogIn();

    DECLARE_OWN_DEVICE_CALLBACKS
};

} /* namespace stubserver */

#endif /* STUBSERVER_DEVICEDUALANALOGIN_H_ */
