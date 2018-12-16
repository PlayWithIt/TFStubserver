/*
 * DeviceOutdoorWeather.h
 *
 * Copyright (C) 2018 Holger Grosenick
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

#ifndef STUBSERVER_DEVICEOUTDOORWEATHER_H_
#define STUBSERVER_DEVICEOUTDOORWEATHER_H_

#include "DeviceFunctions.h"
#include "VisualizationClient.h"


namespace stubserver {

/**
 * Outdoor Weather with max 8 outdoor sensors, not more.
 */
class DeviceOutdoorWeather : public V2Device, public SensorState
{
public:
    static const int MAX_SENSORS = 8;

    /**
     * The ValueProvider needs to provide temperature data.
     */
    DeviceOutdoorWeather(ValueProvider *vp, uint8_t uid);
    virtual ~DeviceOutdoorWeather();

    /**
     * Add another sensor during construction phase.
     */
    void addSensor(ValueProvider *vp, uint8_t id);

    DECLARE_OWN_DEVICE_CALLBACKS

private:
    struct SensorData {
        utils::PValueProvider valueProvider;
        uint64_t              lastChange;
        int                   temperature;
        unsigned              humidity;
        uint8_t               id;

        SensorData();
    };

    /**
     * Find Sensor with given ID in array or return NULL.
     */
    SensorData* getSensor(uint8_t id);

    SensorData sensors[MAX_SENSORS];
    unsigned   numSensors;
    bool       sensorCallback;
    bool       stationCallback;
};


} /* namespace stubserver */

#endif /* STUBSERVER_DEVICEOUTDOORWEATHER_H_ */
