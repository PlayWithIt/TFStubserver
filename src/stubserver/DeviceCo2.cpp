/*
 * DeviceCo2.cpp
 *
 * Copyright (C) 2020 Holger Grosenick
 */

#include <bricklet_co2_v2.h>

#include "BrickStack.h"
#include "DeviceCo2.h"


namespace stubserver {


DeviceCo2::DeviceCo2(utils::ValueProvider *vp, utils::ValueProvider *vpT, utils::ValueProvider *vpH)
   : V2Device(NULL, this, true)
   , SensorState(0, 40000)  // 0 .. 40000 ppm
   , multiValueProvider(nullptr)
   , vpCo2(vp)
   , vpTemp(vpT)
   , vpHum(vpH)
   , tempOffset(0)
{
    if (!vp)
        throw utils::Exception("DeviceCo2::vp must be non-null");
    else {
        multiValueProvider = dynamic_cast<utils::CSVValueProvider*>(vp);
        if (multiValueProvider) {
            vpCo2  = nullptr;
            vpTemp = nullptr;
            vpHum  = nullptr;
        }
        else {
            if (!vpTemp)
                throw utils::Exception("DeviceCo2::vpTemp must be non-null");
            if (!vpHum)
                throw utils::Exception("DeviceCo2::vpHum must be non-null");
        }
    }

    cbAll.setFunctions(CO2_V2_FUNCTION_SET_ALL_VALUES_CALLBACK_CONFIGURATION,
                       CO2_V2_FUNCTION_GET_ALL_VALUES_CALLBACK_CONFIGURATION,
                       0, 0, CO2_V2_CALLBACK_ALL_VALUES);
    cbHum.setFunctions(CO2_V2_FUNCTION_SET_HUMIDITY_CALLBACK_CONFIGURATION,
                       CO2_V2_FUNCTION_GET_HUMIDITY_CALLBACK_CONFIGURATION,
                       0, 0, CO2_V2_CALLBACK_HUMIDITY);
    cbTemp.setFunctions(CO2_V2_FUNCTION_SET_TEMPERATURE_CALLBACK_CONFIGURATION,
                        CO2_V2_FUNCTION_GET_TEMPERATURE_CALLBACK_CONFIGURATION,
                        0, 0, CO2_V2_CALLBACK_TEMPERATURE);
    cbCo2.setFunctions(CO2_V2_FUNCTION_SET_CO2_CONCENTRATION_CALLBACK_CONFIGURATION,
                       CO2_V2_FUNCTION_GET_CO2_CONCENTRATION_CALLBACK_CONFIGURATION,
                       0, 0, CO2_V2_CALLBACK_CO2_CONCENTRATION);
}

DeviceCo2::~DeviceCo2() {
    delete multiValueProvider;
}


bool DeviceCo2::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    // set default dummy response size: header only
    p.header.length = sizeof(p.header);

    // check function to perform
    switch (p.header.function_id) {
    case CO2_V2_FUNCTION_GET_AIR_PRESSURE:
        p.uint16Value = sensorValue3;
        p.header.length += sizeof(p.uint16Value);
        return true;

    case CO2_V2_FUNCTION_SET_AIR_PRESSURE:
        sensorValue3 = p.uint16Value;
        return true;

    case CO2_V2_FUNCTION_GET_ALL_VALUES:
        p.ushorts.value1    = sensorValue;   // CO2
        p.shorts.value2     = sensorValue1;  // temp
        p.ushorts.values[0] = sensorValue2;  // hum
        p.header.length += 6;
        return true;

    case CO2_V2_FUNCTION_GET_CO2_CONCENTRATION:
        p.uint16Value = sensorValue;
        p.header.length += sizeof(p.uint16Value);
        return true;

    case CO2_V2_FUNCTION_GET_HUMIDITY:
        p.uint16Value = sensorValue2;
        p.header.length += sizeof(p.uint16Value);
        return true;

    case CO2_V2_FUNCTION_GET_TEMPERATURE:
        p.int16Value = sensorValue1;
        p.header.length += sizeof(p.int16Value);
        return true;

    case CO2_V2_FUNCTION_GET_TEMPERATURE_OFFSET:
        p.uint16Value = tempOffset;
        p.header.length += sizeof(p.uint16Value);
        return true;

    case CO2_V2_FUNCTION_SET_TEMPERATURE_OFFSET:
        tempOffset = p.uint16Value;
        return true;

    default:
        if (cbAll.consumeGetSetConfig(p))
            return true;
        if (cbHum.consumeGetSetConfig(p))
            return true;
        if (cbTemp.consumeGetSetConfig(p))
            return true;
        if (cbCo2.consumeGetSetConfig(p))
            return true;

        return V2Device::consumeCommand(relativeTimeMs, p, visualizationClient);
    }
}

void DeviceCo2::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &visualizationClient)
{
    int newCo2, newTemp, newHum;

    if (multiValueProvider) {
        std::vector<int> newValues;
        multiValueProvider->getValues(relativeTimeMs, newValues);
        newCo2  = newValues[0];
        newTemp = newValues[1];
        newHum  = newValues[2];
    }
    else {
        newCo2  = vpCo2->getValue(relativeTimeMs);
        newTemp = vpTemp->getValue(relativeTimeMs);
        newHum  = vpHum->getValue(relativeTimeMs);
    }

    if (visualizationClient.useAsInputSource())
        newCo2 = visualizationClient.getInputState();

    if (newCo2 != sensorValue || newTemp != sensorValue1 || newHum != sensorValue2) {
        if (cbAll.active) {
            IOPacket packet(uid, CO2_V2_CALLBACK_ALL_VALUES, 6);
            packet.ushorts.value1    = newCo2;
            packet.shorts.value2     = newTemp;
            packet.ushorts.values[2] = newHum;
            brickStack->dispatchCallback(packet);
        }
    }

    if (newCo2 != sensorValue) {
        sensorValue = newCo2;
        if (cbCo2.active) {
            IOPacket packet(uid, CO2_V2_CALLBACK_CO2_CONCENTRATION, 2);
            packet.uint16Value = newCo2;
            brickStack->dispatchCallback(packet);
        }

        // sensorValue must hold the correct value before 'notify' is called
        notify(visualizationClient);
    }

    if (newTemp != sensorValue) {
        sensorValue1 = newTemp;
        if (cbTemp.active) {
            IOPacket packet(uid, CO2_V2_CALLBACK_TEMPERATURE, 2);
            packet.int16Value = newTemp;
            brickStack->dispatchCallback(packet);
        }
    }

    if (newHum != sensorValue2) {
        sensorValue2 = newHum;
        if (cbHum.active) {
            IOPacket packet(uid, CO2_V2_CALLBACK_HUMIDITY, 2);
            packet.uint16Value = newHum;
            brickStack->dispatchCallback(packet);
        }
    }
}

} /* namespace stubserver */
