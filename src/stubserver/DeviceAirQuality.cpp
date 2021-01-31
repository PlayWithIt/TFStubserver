/*
 * DeviceAirQuality.cpp
 *
 * Copyright (C) 2019 Holger Grosenick
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

#include <bricklet_air_quality.h>

#include "DeviceAirQuality.h"
#include "BrickStack.h"

namespace stubserver {

/**
 * The ValueProvider needs to provide data for all channels
 * @param _vpT ValueProvider for temperature
 * @param _vpH ValueProvider for humidity
 * @param _vpP ValueProvider for air pressure
 * @param _vpI ValueProvider for IAQ index
 */
DeviceAirQuality::DeviceAirQuality(ValueProvider *_vpT, ValueProvider *_vpH, ValueProvider *_vpP, ValueProvider *_vpI)
    : V2Device(NULL, this, true)
    , SensorState(-1000, 1000)  // -100 .. +100°C
    , vpT(_vpT)
    , vpH(_vpH)
    , vpP(_vpP)
    , vpI(_vpI)
    , cbT(4)
    , cbH(4)
    , cbP(4)
    , cbI(4)
    , tempOffset(0)
    , cbAPeriod(0)
    , cbAStartTime(0)
    , cbAHasToChange(false)
{
    // Min / Max from VizualizationClient: as we have 4 values for min / max:
    // set to 0 so that the UI ignores this and uses its own values.
    this->min = 0;
    this->max = 0;
    this->setStatusLedConfig(STATUS_LED_HEARTBEAT);

    if (vpT == NULL)
        throw utils::Exception("DeviceAirQuality::vpT must be non-null");
    if (vpH == NULL)
        throw utils::Exception("DeviceAirQuality::vpH must be non-null");
    if (vpP == NULL)
        throw utils::Exception("DeviceAirQuality::vpP must be non-null");
    if (vpI == NULL)
        throw utils::Exception("DeviceAirQuality::vpI must be non-null");

    cbT.setFunctions(AIR_QUALITY_FUNCTION_SET_TEMPERATURE_CALLBACK_CONFIGURATION,
                     AIR_QUALITY_FUNCTION_GET_TEMPERATURE_CALLBACK_CONFIGURATION,
                     0, 0, AIR_QUALITY_CALLBACK_TEMPERATURE);
    cbH.setFunctions(AIR_QUALITY_FUNCTION_SET_HUMIDITY_CALLBACK_CONFIGURATION,
                     AIR_QUALITY_FUNCTION_GET_HUMIDITY_CALLBACK_CONFIGURATION,
                     0, 0, AIR_QUALITY_CALLBACK_HUMIDITY);
    cbP.setFunctions(AIR_QUALITY_FUNCTION_SET_AIR_PRESSURE_CALLBACK_CONFIGURATION,
                     AIR_QUALITY_FUNCTION_GET_AIR_PRESSURE_CALLBACK_CONFIGURATION,
                     0, 0, AIR_QUALITY_CALLBACK_AIR_PRESSURE);
    cbI.setFunctions(AIR_QUALITY_FUNCTION_SET_IAQ_INDEX_CALLBACK_CONFIGURATION,
                     AIR_QUALITY_FUNCTION_GET_IAQ_INDEX_CALLBACK_CONFIGURATION,
                     0, 0, AIR_QUALITY_CALLBACK_IAQ_INDEX);
}

DeviceAirQuality::~DeviceAirQuality()
{
    delete vpT;
    delete vpH;
    delete vpP;
    delete vpI;
}


bool DeviceAirQuality::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    // set default dummy response size: header only
    p.header.length = sizeof(p.header);

    // check function to perform
    switch (p.header.function_id) {
    case AIR_QUALITY_FUNCTION_GET_ALL_VALUES:
        // air_quality_get_all_values(AirQuality *air_quality, int32_t *ret_iaq_index, uint8_t *ret_iaq_index_accuracy, int32_t *ret_temperature, int32_t *ret_humidity, int32_t *ret_air_pressure)
        p.airQualityData.air_pressure = vpP->getValue(relativeTimeMs);
        p.airQualityData.temperature  = vpT->getValue(relativeTimeMs);
        p.airQualityData.humidity     = vpH->getValue(relativeTimeMs);
        p.airQualityData.iaq_index    = vpI->getValue(relativeTimeMs);
        p.airQualityData.iaq_index_accuracy = 3;
        p.header.length += sizeof(p.airQualityData);
        return true;

    case AIR_QUALITY_FUNCTION_GET_IAQ_INDEX:
        p.uint32Value = sensorValue1;
        p.airQualityData.iaq_index_accuracy = 3;
        p.header.length += 5;
        return true;

    case AIR_QUALITY_FUNCTION_GET_TEMPERATURE:
        p.uint32Value = sensorValue2 + tempOffset;
        p.header.length += sizeof(p.uint32Value);
        return true;

    case AIR_QUALITY_FUNCTION_GET_HUMIDITY:
        p.uint32Value = sensorValue3;
        p.header.length += sizeof(p.uint32Value);
        return true;

    case AIR_QUALITY_FUNCTION_GET_AIR_PRESSURE:
        p.uint32Value = sensorValue4;
        p.header.length += sizeof(p.uint32Value);
        return true;

    case AIR_QUALITY_FUNCTION_SET_TEMPERATURE_OFFSET:
        tempOffset = p.int32Value;
        return true;

    case AIR_QUALITY_FUNCTION_GET_TEMPERATURE_OFFSET:
        p.int32Value = tempOffset;
        p.header.length += sizeof(p.uint32Value);
        return true;

    case AIR_QUALITY_FUNCTION_SET_ALL_VALUES_CALLBACK_CONFIGURATION:
        cbAPeriod = p.int32Value;
        cbAStartTime = relativeTimeMs;
        cbAHasToChange = p.fullData.payload[0] != 0;
        return true;

    case AIR_QUALITY_FUNCTION_GET_ALL_VALUES_CALLBACK_CONFIGURATION:
        p.int32Value = cbAPeriod;
        p.fullData.payload[0] = cbAHasToChange;
        p.header.length += 5;
        return true;

    default:
        if (cbT.consumeGetSetConfig(p))
            return true;
        if (cbH.consumeGetSetConfig(p))
            return true;
        if (cbP.consumeGetSetConfig(p))
            return true;
        if (cbI.consumeGetSetConfig(p))
            return true;

        return V2Device::consumeCommand(relativeTimeMs, p, visualizationClient);
    }
}


void DeviceAirQuality::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &visualizationClient)
{
    int newValue;
    bool anyChange = false;

    //--------------- IAQ index callback ------------------------------------------
    if (visualizationClient.useAsInputSource(AIR_QUALITY_IAQ))
        newValue = visualizationClient.getInputState(AIR_QUALITY_IAQ);
    else
        newValue = vpI->getValue(relativeTimeMs);

    if (newValue != sensorValue1)
    {
        //printf("cbI change %lu (old=%d, new=%d)\n", relativeTimeMs, sensorValue1, newValue);
        anyChange = true;
        sensorValue = sensorValue1 = newValue;
        notify(AIR_QUALITY_IAQ, visualizationClient, VALUE_CHANGE);
    }
    if (newValue != cbI.param1 && cbI.mayExecute(relativeTimeMs))
    {
        IOPacket packet(uid, AIR_QUALITY_CALLBACK_IAQ_INDEX, 5);
        packet.int32Value = newValue;
        packet.fullData.payload[4] = 3;
        brickStack->dispatchCallback(packet);
        cbI.relativeStartTime = relativeTimeMs;
        cbI.param1 = newValue;
    }

    //--------------- temperature callback ------------------------------------------
    if (visualizationClient.useAsInputSource(AIR_QUALITY_TEMP))
        newValue = visualizationClient.getInputState(AIR_QUALITY_TEMP);
    else
        newValue = vpT->getValue(relativeTimeMs);

    if (newValue != sensorValue2)
    {
        // printf("cbT change %lu (old=%d, new=%d)\n", relativeTimeMs, sensorValue2, newValue);
        anyChange = true;
        sensorValue = sensorValue2 = newValue;
        notify(AIR_QUALITY_TEMP, visualizationClient, VALUE_CHANGE);
    }
    if (cbT.shouldTriggerRangeCallback(relativeTimeMs, newValue)) {
        triggerCallbackInt(relativeTimeMs, uid, brickStack, cbT, newValue);
    }

    //--------------- humidity callback ------------------------------------------
    if (visualizationClient.useAsInputSource(AIR_QUALITY_HUMIDITY))
        newValue = visualizationClient.getInputState(AIR_QUALITY_HUMIDITY);
    else
        newValue = vpH->getValue(relativeTimeMs);

    if (newValue != sensorValue3)
    {
        //printf("cbH change %lu\n", relativeTimeMs);
        anyChange = true;
        sensorValue = sensorValue3 = newValue;
        notify(AIR_QUALITY_HUMIDITY, visualizationClient, VALUE_CHANGE);
    }
    if (cbH.shouldTriggerRangeCallback(relativeTimeMs, newValue)) {
        triggerCallbackInt(relativeTimeMs, uid, brickStack, cbH, newValue);
    }

    //--------------- air pressure callback -----------------------------------------
    if (visualizationClient.useAsInputSource(AIR_QUALITY_PRESSURE))
        newValue = visualizationClient.getInputState(AIR_QUALITY_PRESSURE);
    else
        newValue = vpP->getValue(relativeTimeMs);

    if (newValue != sensorValue4)
    {
        //printf("cbP change %lu\n", relativeTimeMs);
        anyChange = true;
        sensorValue = sensorValue4 = newValue;
        notify(AIR_QUALITY_PRESSURE, visualizationClient, VALUE_CHANGE);
    }
    if (cbP.shouldTriggerRangeCallback(relativeTimeMs, newValue)) {
        triggerCallbackInt(relativeTimeMs, uid, brickStack, cbP, newValue);
    }

    //--------------- all values callback -----------------------------------------
    if (!anyChange || cbAPeriod == 0 || (cbAStartTime + cbAPeriod > relativeTimeMs))
        return;

    //----- trigger callback
    {
        IOPacket packet(uid, AIR_QUALITY_CALLBACK_ALL_VALUES, 4*4+1);
        packet.airQualityData.air_pressure = sensorValue4;
        packet.airQualityData.temperature  = sensorValue2;
        packet.airQualityData.humidity     = sensorValue3;
        packet.airQualityData.iaq_index    = sensorValue1;
        packet.airQualityData.iaq_index_accuracy = 3;
        brickStack->dispatchCallback(packet);
        cbAStartTime = relativeTimeMs;
    }
}

} /* namespace stubserver */
