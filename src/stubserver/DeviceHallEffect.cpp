/*
 * DeviceHallEffect.cpp
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

#include <bricklet_hall_effect.h>

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

DeviceHallEffect::~DeviceHallEffect()
{
    delete valueProvider;
}


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

} /* namespace stubserver */
