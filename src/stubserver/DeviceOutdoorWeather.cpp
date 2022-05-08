/*
 * DeviceOutdoorWeather.cpp
 *
 * Copyright (C) 2018-2021 Holger Grosenick
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

// ID 0..255 are simple sensors, ID 256 .. 511 are outdoor stations
#define STATION_ID_OFFSET 256

namespace stubserver {

DeviceOutdoorWeather::SensorData::SensorData(utils::PValueProvider vp, unsigned id)
   : BaseData(id)
   , valueProvider(vp)
   , lastChange(0)
{
}

DeviceOutdoorWeather::DeviceOutdoorWeather(ValueProvider *vp, unsigned id)
    : V2Device(NULL, this, true)
    , OutdoorWeatherState()
    , sensorCallback(false)
    , stationCallback(false)
{
    addSensor(vp, id);
}

DeviceOutdoorWeather::~DeviceOutdoorWeather()
{
    for (auto it : sensors) {
        SensorData *p = static_cast<SensorData*>(it);
        delete p->valueProvider;
    }
}

/**
 * Add another sensor during construction phase.
 */
void DeviceOutdoorWeather::addSensor(ValueProvider *vp, unsigned id)
{
    // ignore additions calls
    if (sensors.size() >= MAX_SENSORS)
        return;

    if (! vp)
        throw utils::Exception("DeviceOutdoorWeather::addSensor(): valueProvider must be non-null");

    sensors.push_back(new SensorData(vp, id));
    // printf("Add sensor %u\n", id);
}

/**
 * Find Sensor with given ID in array or return NULL.
 */
DeviceOutdoorWeather::SensorData* DeviceOutdoorWeather::getSensor(uint8_t id, bool isStation)
{
    unsigned toFind = id;
    if (isStation)
        toFind += STATION_ID_OFFSET;

    for (auto it : sensors) {
        if (it->getId() == toFind)
            return static_cast<DeviceOutdoorWeather::SensorData*>(it);
    }
    return nullptr;
}


bool DeviceOutdoorWeather::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    SensorData* sensor;
    unsigned num;

    // set default dummy response size: header only
    p.header.length = sizeof(p.header);

    // check function to perform
    switch (p.header.function_id) {
    case OUTDOOR_WEATHER_FUNCTION_GET_SENSOR_DATA:
    	// outdoor_weather_get_sensor_data(OutdoorWeather *dev, uint8_t id, int16_t *ret_temperature, uint8_t *ret_humidity, uint16_t *ret_last_change);
        sensor = getSensor(p.uint8Value, false);
        if (sensor) {
            p.outdoorData.temperature = sensor->getTemperatoure();
            p.outdoorData.humidity    = sensor->getHumidity();
            p.outdoorData.last_change = (relativeTimeMs - sensor->lastChange) / 1000;
            p.header.length += sizeof(p.outdoorData);
        }
        else
            p.setErrorCode(IOPacket::INVALID_PARAMETER);
        return true;

    case OUTDOOR_WEATHER_FUNCTION_GET_STATION_DATA:
        // outdoor_weather_get_sensor_data(OutdoorWeather *dev, uint8_t id, int16_t *ret_temperature, uint8_t *ret_humidity, uint16_t *ret_last_change);
        sensor = getSensor(p.uint8Value, true);
        if (sensor) {
            p.outdoorStationData.temperature = sensor->getTemperatoure();
            p.outdoorStationData.humidity    = sensor->getHumidity();
            p.outdoorStationData.wind_speed  = 20;
            p.outdoorStationData.gust_speed  = 10;
            p.outdoorStationData.rain        = 5;
            p.outdoorStationData.wind_direction = 1;
            p.outdoorStationData.battery_low = false;
            p.outdoorStationData.last_change = (relativeTimeMs - sensor->lastChange) / 1000;

            p.header.length += sizeof(p.outdoorStationData);
        }
        else
            p.setErrorCode(IOPacket::INVALID_PARAMETER);
        return true;

    case OUTDOOR_WEATHER_FUNCTION_GET_SENSOR_IDENTIFIERS_LOW_LEVEL:
    	// outdoor_weather_get_sensor_identifiers_low_level(OutdoorWeather *dev, uint16_t *ret_identifiers_length, uint16_t *ret_identifiers_chunk_offset, uint8_t ret_identifiers_chunk_data[60]);
        num = 0;

        for (auto it : sensors) {
            if (it->getId() < STATION_ID_OFFSET) {
                p.fullData.payload[num + 4] = it->getId();
                ++num;
            }
        }
        p.ushorts.value1 = num;
        p.ushorts.value2 = 0;
        p.header.length += 64;
        return true;

    case OUTDOOR_WEATHER_FUNCTION_GET_STATION_IDENTIFIERS_LOW_LEVEL:
        num = 0;
        for (auto it : sensors) {
            if (it->getId() >= STATION_ID_OFFSET) {
                p.fullData.payload[num + 4] = it->getId() - STATION_ID_OFFSET;
                ++num;
            }
        }
        p.ushorts.value1 = num;
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
    // for each sensor that has changed: trigger a callback
    unsigned numSensors = sensors.size();
    for (unsigned i = 0; i < numSensors; ++i)
    {
        SensorData* sensor = static_cast<SensorData*>(sensors[i]);

        if (visualizationClient.useAsInputSource(i))
        {
            union NewState {
                int64_t t;
                struct {
                    int temp;
                    unsigned hum;
                };
            };

            NewState state;
            state.t = visualizationClient.getInputState(i);

            if (state.temp != sensor->getTemperatoure() || state.hum != sensor->getHumidity())
            {
                sensor->setTemperature(state.temp);
                sensor->setHumidity(state.hum);
                sensor->lastChange = relativeTimeMs;

                if (sensor->getId() < STATION_ID_OFFSET) {
                    IOPacket packet(uid, OUTDOOR_WEATHER_CALLBACK_SENSOR_DATA, 4);
                    packet.outdoorCallback.id          = sensor->getId();
                    packet.outdoorCallback.temperature = sensor->getTemperatoure();
                    packet.outdoorCallback.humidity    = sensor->getHumidity();

                    brickStack->dispatchCallback(packet);
                }
                else {
                    // weather station sensor
                    IOPacket packet(uid, OUTDOOR_WEATHER_CALLBACK_STATION_DATA, sizeof(packet.outdoorStationCallback));
                    packet.outdoorStationCallback.id          = sensor->getId() - STATION_ID_OFFSET;
                    packet.outdoorStationCallback.temperature = sensor->getTemperatoure();
                    packet.outdoorStationCallback.humidity    = sensor->getHumidity();
                    packet.outdoorStationCallback.wind_speed  = 20;
                    packet.outdoorStationCallback.gust_speed  = 10;
                    packet.outdoorStationCallback.wind_direction = 1;
                    packet.outdoorStationCallback.rain        = 1580;   // == 158.0 mm
                    packet.outdoorStationCallback.battery_low = false;

                    brickStack->dispatchCallback(packet);
                }
            }
        }
        else {
            int nw = sensor->valueProvider->getValue(relativeTimeMs);
            if (sensor->getTemperatoure() != nw)
            {
                sensor->setTemperature(nw);
                sensor->setHumidity(40 + ((relativeTimeMs / 100) % 12));
                sensor->lastChange  = relativeTimeMs;

                if (sensor->getId() < STATION_ID_OFFSET) {
                    // simple sensor
                    IOPacket packet(uid, OUTDOOR_WEATHER_CALLBACK_SENSOR_DATA, 4);
                    packet.outdoorCallback.id          = sensor->getId();
                    packet.outdoorCallback.temperature = nw;
                    packet.outdoorCallback.humidity    = sensor->getHumidity();

                    brickStack->dispatchCallback(packet);

                    // TFStubViz only displays 4 sensors!
                    this->setInternalSensorNo(i);
                    notify(visualizationClient);
                }
                else {
                    // weather station sensor
                    IOPacket packet(uid, OUTDOOR_WEATHER_CALLBACK_STATION_DATA, sizeof(packet.outdoorStationCallback));
                    packet.outdoorStationCallback.id          = sensor->getId() - STATION_ID_OFFSET;
                    packet.outdoorStationCallback.temperature = nw;
                    packet.outdoorStationCallback.humidity    = sensor->getHumidity();
                    packet.outdoorStationCallback.wind_speed  = 20;
                    packet.outdoorStationCallback.gust_speed  = 10;
                    packet.outdoorStationCallback.wind_direction = 1;
                    packet.outdoorStationCallback.rain        = 1580;   // == 158.0 mm
                    packet.outdoorStationCallback.battery_low = false;

                    brickStack->dispatchCallback(packet);

                    // at least show the temp, the rain value is not supported in UI
                    this->setInternalSensorNo(i);
                    notify(visualizationClient);
                }
            }
        }
    }
}

} /* namespace stubserver */
