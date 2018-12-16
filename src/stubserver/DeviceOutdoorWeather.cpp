/*
 * DeviceOutdoorWeather.cpp
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

#include <bricklet_outdoor_weather.h>

#include "DeviceOutdoorWeather.h"
#include "BrickStack.h"

namespace stubserver {

DeviceOutdoorWeather::SensorData::SensorData()
{
    bzero(this, sizeof(SensorData));
}

DeviceOutdoorWeather::DeviceOutdoorWeather(ValueProvider *vp, uint8_t id)
    : V2Device(NULL, this)
    , SensorState(-1000, 1000)  // -100 .. +100°C
    , numSensors(1)
    , sensorCallback(false)
    , stationCallback(false)
{
    if (vp == NULL)
        throw utils::Exception("DeviceOutdoorWeather::valueProvider must be non-null");

    sensors[0].valueProvider = vp;
    sensors[0].id = id;
}

DeviceOutdoorWeather::~DeviceOutdoorWeather() {
}

/**
 * Add another sensor during construction phase.
 */
void DeviceOutdoorWeather::addSensor(ValueProvider *vp, uint8_t id)
{
    // ignore additions calls
    if (numSensors+1 >= MAX_SENSORS)
        return;

    if (vp == NULL)
        throw utils::Exception("DeviceOutdoorWeather::addSensor(): valueProvider must be non-null");

    sensors[numSensors].valueProvider = vp;
    sensors[numSensors].id = id;
    ++numSensors;
}

/**
 * Find Sensor with given ID in array or return NULL.
 */
DeviceOutdoorWeather::SensorData* DeviceOutdoorWeather::getSensor(uint8_t id)
{
    for (unsigned i = 0; i < numSensors; ++i) {
        if (sensors[i].id == id)
            return &(sensors[i]);
    }
    return NULL;
}


bool DeviceOutdoorWeather::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    SensorData* sensor;

    // set default dummy response size: header only
    p.header.length = sizeof(p.header);

    // check function to perform
    switch (p.header.function_id) {
    case OUTDOOR_WEATHER_FUNCTION_GET_SENSOR_DATA:
    	// outdoor_weather_get_sensor_data(OutdoorWeather *dev, uint8_t id, int16_t *ret_temperature, uint8_t *ret_humidity, uint16_t *ret_last_change);
        sensor = getSensor(p.uint8Value);
        if (sensor) {
            p.outdoorData.temperature = sensor->temperature;
            p.outdoorData.humidity    = sensor->humidity;
            p.outdoorData.last_change = (relativeTimeMs - sensor->lastChange) / 1000;
            p.header.length += sizeof(p.outdoorData);
        }
        else
            p.setErrorCode(IOPacket::INVALID_PARAMETER);
        return true;

    case OUTDOOR_WEATHER_FUNCTION_GET_SENSOR_IDENTIFIERS_LOW_LEVEL:
    	// outdoor_weather_get_sensor_identifiers_low_level(OutdoorWeather *dev, uint16_t *ret_identifiers_length, uint16_t *ret_identifiers_chunk_offset, uint8_t ret_identifiers_chunk_data[60]);
        p.ushorts.value1 = numSensors;
        p.ushorts.value2 = 0;
        for (unsigned i = 0; i < numSensors; ++i)
            p.fullData.payload[i + 4] = sensors[i].id;
        p.header.length += 64;
        return true;

    case OUTDOOR_WEATHER_FUNCTION_GET_STATION_IDENTIFIERS_LOW_LEVEL:
    	// outdoor_weather_get_station_identifiers_low_level(OutdoorWeather *dev, uint16_t *ret_identifiers_length, uint16_t *ret_identifiers_chunk_offset, uint8_t ret_identifiers_chunk_data[60]);
        p.ushorts.value1 = 0;
        p.ushorts.value2 = 0;
        p.header.length += 64;
        return true;

    case OUTDOOR_WEATHER_FUNCTION_SET_SENSOR_CALLBACK_CONFIGURATION:
        sensorCallback = p.boolValue;
        return true;

    case OUTDOOR_WEATHER_FUNCTION_GET_SENSOR_CALLBACK_CONFIGURATION:
        p.boolValue = sensorCallback;
        p.header.length += 1;
        return true;

    case OUTDOOR_WEATHER_FUNCTION_SET_STATION_CALLBACK_CONFIGURATION:
        stationCallback = p.boolValue;
        return true;

    case OUTDOOR_WEATHER_FUNCTION_GET_STATION_CALLBACK_CONFIGURATION:
        p.boolValue = stationCallback;
        p.header.length += 1;
        return true;

    default:
        return V2Device::consumeCommand(relativeTimeMs, p, visualizationClient);
    }
}

void DeviceOutdoorWeather::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &visualizationClient)
{
    if (visualizationClient.useAsInputSource())
    {
        int newValue = visualizationClient.getInputState();
        if (newValue != sensorValue)
        {
            sensorValue = newValue;

            SensorData* sensor = &(sensors[0]);
            sensor->temperature = newValue;
            sensor->humidity    = 40 + ((relativeTimeMs / 100) % 12);
            sensor->lastChange  = relativeTimeMs;

            IOPacket packet(uid, OUTDOOR_WEATHER_CALLBACK_SENSOR_DATA, 4);
            packet.outdoorCallback.id          = sensor->id;
            packet.outdoorCallback.temperature = newValue;
            packet.outdoorCallback.humidity    = sensor->humidity;

            brickStack->dispatchCallback(packet);
        }
        return;
    }

    // for each sensor that has changed: trigger a callback
    for (unsigned i = 0; i < numSensors; ++i)
    {
        SensorData* sensor = &(sensors[i]);
        int nw = sensor->valueProvider->getValue(relativeTimeMs);
        if (sensor->temperature != nw)
        {
            sensor->temperature = nw;
            sensor->humidity    = 40 + ((relativeTimeMs / 100) % 12);
            sensor->lastChange  = relativeTimeMs;

            IOPacket packet(uid, OUTDOOR_WEATHER_CALLBACK_SENSOR_DATA, 4);
            packet.outdoorCallback.id          = sensor->id;
            packet.outdoorCallback.temperature = nw;
            packet.outdoorCallback.humidity    = sensor->humidity;

            brickStack->dispatchCallback(packet);

            // TFStubViz only displays two sensors!
            if (i < 2) {
                this->setInternalSensorNo(i);
                this->sensorValue = nw;
                notify(visualizationClient);
            }
        }
    }
}

} /* namespace stubserver */
