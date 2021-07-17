/*
 * DeviceHallEffect.cpp
 *
 * Copyright (C) 2015-2021 Holger Grosenick
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

#include <bricklet_hall_effect.h>
#include <bricklet_hall_effect_v2.h>

#include "BrickStack.h"
#include "DeviceHallEffect.h"

namespace stubserver {


DeviceHallEffect::DeviceHallEffect(ValueProvider *vp)
  : SensorState(0, 1)
  , valueProvider(vp)
  , latestCallback(0)
  , callbackPeriod(0)
  , latestInterrupt(0)
  , edgeInterrupt(0)
  , edgeConfig(HALL_EFFECT_EDGE_TYPE_RISING)
  , debounce(10)
{
}

DeviceHallEffectV2::DeviceHallEffectV2(ValueProvider *vp)
  : V2Device(nullptr, this, true)
  , SensorState(-7000, 7000)
  , valueProvider(vp)
  , latestCallback(0)
  , callbackPeriod(0)
  , prevCounter(0)
  , debounce(10000)  // Entprellzeit
  , highThreshold(2000)
  , lowThreshold(-2000)
  , valueHasToChange(true)
{
    cbFlux.setFunctions(HALL_EFFECT_V2_FUNCTION_SET_MAGNETIC_FLUX_DENSITY_CALLBACK_CONFIGURATION,
                        HALL_EFFECT_V2_FUNCTION_GET_MAGNETIC_FLUX_DENSITY_CALLBACK_CONFIGURATION,
                        0, 0, HALL_EFFECT_V2_CALLBACK_MAGNETIC_FLUX_DENSITY);
}

DeviceHallEffect::~DeviceHallEffect()
{
    delete valueProvider;
}

DeviceHallEffectV2::~DeviceHallEffectV2()
{
    delete valueProvider;
}

/**
 * consume "old" hall effect bricklet
 */
bool DeviceHallEffect::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    // set default dummy response size: header only
    p.header.length = sizeof(p.header);

    // check function to perform
    switch (p.header.function_id) {
    case HALL_EFFECT_FUNCTION_GET_VALUE:
        p.uint8Value = sensorValue;
        p.header.length += sizeof(p.uint8Value);
        return true;

    case HALL_EFFECT_FUNCTION_GET_EDGE_COUNT:
        p.uint32Value = counter;
        p.header.length += sizeof(p.uint32Value);
        return true;

    case HALL_EFFECT_FUNCTION_GET_EDGE_INTERRUPT:
        p.uint32Value = edgeInterrupt;
        p.header.length += sizeof(p.uint32Value);
        return true;

    case HALL_EFFECT_FUNCTION_SET_EDGE_INTERRUPT:
        edgeInterrupt = p.uint32Value;
        latestInterrupt = counter;
        return true;

    case HALL_EFFECT_FUNCTION_GET_EDGE_COUNT_CALLBACK_PERIOD:
        p.uint32Value = callbackPeriod;
        p.header.length += sizeof(p.uint32Value);
        return true;

    case HALL_EFFECT_FUNCTION_SET_EDGE_COUNT_CALLBACK_PERIOD:
        callbackPeriod = p.uint32Value;
        return true;

    case HALL_EFFECT_FUNCTION_SET_EDGE_COUNT_CONFIG:
        counter = 0;
        latestInterrupt = 0;
        edgeConfig = p.fullData.payload[0];
        debounce = p.fullData.payload[1];
        return true;

    case HALL_EFFECT_FUNCTION_GET_EDGE_COUNT_CONFIG:
        p.fullData.payload[0] = edgeConfig;
        p.fullData.payload[1] = debounce;
        p.header.length += 2;
        return true;

    default:
        return false;
    }
}

void DeviceHallEffect::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &visualizationClient)
{
    int newValue;

    if (visualizationClient.useAsInputSource())
        newValue = visualizationClient.getInputState();
    else
        newValue = valueProvider->getValue(relativeTimeMs);

    if (newValue != sensorValue)
    {
        switch (edgeConfig) {
        case HALL_EFFECT_EDGE_TYPE_RISING:
            if (newValue > sensorValue)
                ++counter;
            break;
        case HALL_EFFECT_EDGE_TYPE_FALLING:
            if (newValue < sensorValue)
                ++counter;
            break;
        case HALL_EFFECT_EDGE_TYPE_BOTH:
            ++counter;
            break;
        }

        sensorValue = newValue;
        notify(visualizationClient);

        // edge interrupt
        if (edgeInterrupt > 0 && counter - latestInterrupt >= edgeInterrupt) {
            IOPacket packet(uid,  HALL_EFFECT_CALLBACK_EDGE_COUNT, 5);
            packet.uint32Value = counter;
            packet.fullData.payload[5] = sensorValue;
            brickStack->dispatchCallback(packet);
            latestInterrupt = counter;
            latestCallback = relativeTimeMs;
            return;
        }
    }

    // edge count callback
    if (callbackPeriod > 0 && relativeTimeMs - latestCallback >= callbackPeriod) {
        IOPacket packet(uid,  HALL_EFFECT_CALLBACK_EDGE_COUNT, 5);
        packet.uint32Value = counter;
        packet.fullData.payload[4] = sensorValue;
        brickStack->dispatchCallback(packet);
        latestCallback = relativeTimeMs;
    }
}
/**
 * consume "new" hall effect bricklet
 */
bool DeviceHallEffectV2::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    // set default dummy response size: header only
    p.header.length = sizeof(p.header);

    // check function to perform
    switch (p.header.function_id) {
    case HALL_EFFECT_V2_FUNCTION_GET_MAGNETIC_FLUX_DENSITY:
        // ret_magnetic_flux_density – Typ: int16_t, Einheit: 1 µT, Wertebereich: [-7000 bis 7000]
        p.int16Value = sensorValue;
        p.header.length += sizeof(p.int16Value);
        return true;

    case HALL_EFFECT_V2_FUNCTION_GET_COUNTER: {
        bool reset = p.boolValue;
        p.uint32Value = counter;
        p.header.length += sizeof(p.uint32Value);

        if (reset) {
            counter = 0;
            prevCounter = 0;
        }
        return true;
    }
    case HALL_EFFECT_V2_FUNCTION_SET_COUNTER_CONFIG:
        // high_threshold – Typ: int16_t, Einheit: 1 µT, Wertebereich: [-2^15 bis 2^15 - 1], Standardwert: 2000
        // low_threshold – Typ: int16_t, Einheit: 1 µT, Wertebereich: [-2^15 bis 2^15 - 1], Standardwert: -2000
        // debounce – Typ: uint32_t, Einheit: 1 µs, Wertebereich: [0 bis 1000000], Standardwert: 100000
        highThreshold = p.shorts.value1;
        lowThreshold = p.shorts.value2;
        debounce = p.ints.value2;
        counter = 0;
        prevCounter = 0;
        return true;

    case HALL_EFFECT_V2_FUNCTION_GET_COUNTER_CONFIG:
        p.header.length += 2 * sizeof(p.uint32Value);
        p.shorts.value1 = highThreshold;
        p.shorts.value2 = lowThreshold;
        p.ints.value2   = debounce;
        return true;

    case HALL_EFFECT_V2_FUNCTION_SET_COUNTER_CALLBACK_CONFIGURATION:
        // uint32_t period, bool value_has_to_change
        callbackPeriod = p.callbackConfigInt.period;
        valueHasToChange = p.callbackConfigInt.value_has_to_change;
        return true;

    case HALL_EFFECT_V2_FUNCTION_GET_COUNTER_CALLBACK_CONFIGURATION:
        p.header.length += sizeof(p.uint32Value) + 1;
        p.callbackConfigInt.period = callbackPeriod;
        p.callbackConfigInt.value_has_to_change = valueHasToChange;
        return true;

    default:
        if (cbFlux.consumeGetSetConfig(p))
            return true;
        return V2Device::consumeCommand(relativeTimeMs, p, visualizationClient);
    }
}

void DeviceHallEffectV2::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &visualizationClient)
{
    int newValue;

    if (visualizationClient.useAsInputSource())
        newValue = visualizationClient.getInputState();
    else
        newValue = valueProvider->getValue(relativeTimeMs);

    if (newValue != sensorValue)
    {
        if (newValue > highThreshold && sensorValue <= highThreshold)
            ++counter;
        else if (newValue < lowThreshold && sensorValue >= lowThreshold)
            ++counter;

        sensorValue = newValue;
        notify(visualizationClient);
    }

    if (cbFlux.shouldTriggerRangeCallback(relativeTimeMs, newValue)) {
        IOPacket packet(uid, HALL_EFFECT_V2_CALLBACK_MAGNETIC_FLUX_DENSITY, 2);
        packet.int16Value = newValue;
        brickStack->dispatchCallback(packet);
        cbFlux.relativeStartTime += cbFlux.period;
    }

    // edge count callback
    if (callbackPeriod > 0 && relativeTimeMs - latestCallback >= callbackPeriod &&
            (!valueHasToChange || prevCounter != counter)) {
        IOPacket packet(uid, HALL_EFFECT_V2_CALLBACK_COUNTER, 4);
        packet.uint32Value = counter;
        prevCounter = counter;
        brickStack->dispatchCallback(packet);
        latestCallback = relativeTimeMs;
    }
}

} /* namespace stubserver */
