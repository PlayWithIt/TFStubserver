/*
 * DeviceMotionDetector.cpp
 *
 * Copyright (C) 2014-2018 Holger Grosenick
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

#include <bricklet_motion_detector.h>
#include <bricklet_motion_detector_v2.h>

#include "BrickStack.h"
#include "DeviceMotionDetector.h"

#if MOTION_DETECTOR_V2_FUNCTION_GET_MOTION_DETECTED != MOTION_DETECTOR_FUNCTION_GET_MOTION_DETECTED
#error "MOTION_DETECTOR_V2_FUNCTION_GET_MOTION_DETECTED != MOTION_DETECTOR_FUNCTION_GET_MOTION_DETECTED, but must be equal"
#endif


namespace stubserver {


DeviceMotionDetector::DeviceMotionDetector(ValueProvider *vp, bool v2)
  : V2Device(NULL, this, v2), SensorState(0, 1)
  , valueProvider(vp)
  , motionEnd(0)
  , sensitivity(50)
{
    if (v2)
        setStatusLedConfig(MOTION_DETECTOR_STATUS_LED_CONFIG_SHOW_STATUS);
    else
        setStatusLedConfig(MOTION_DETECTOR_V2_STATUS_LED_CONFIG_SHOW_STATUS);
}

DeviceMotionDetector::~DeviceMotionDetector()
{
    delete valueProvider;
}


bool DeviceMotionDetector::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualizationClient &visualizationClient)
{
    // set default dummy response size: header only
    p.header.length = sizeof(p.header);

    // check function to perform: see for #error which functions are equal in V1 + V2
    switch (p.header.function_id) {
    case MOTION_DETECTOR_FUNCTION_GET_MOTION_DETECTED:
        p.uint8Value = sensorValue ? MOTION_DETECTOR_MOTION_DETECTED : MOTION_DETECTOR_MOTION_NOT_DETECTED;
        p.header.length += sizeof(p.uint8Value);
        return true;

    case MOTION_DETECTOR_V2_FUNCTION_GET_SENSITIVITY:
        p.fullData.payload[0] = sensitivity;
        p.header.length += 1;
        return true;

    case MOTION_DETECTOR_V2_FUNCTION_SET_SENSITIVITY:
        sensitivity = p.fullData.payload[0];
        return true;

    case MOTION_DETECTOR_V2_FUNCTION_GET_INDICATOR:
        p.fullData.payload[0] = led1;
        p.fullData.payload[1] = led2;
        p.fullData.payload[2] = led3;
        p.header.length += 3;
        return true;

    case MOTION_DETECTOR_V2_FUNCTION_SET_INDICATOR:
        led1 = p.fullData.payload[0];
        led2 = p.fullData.payload[1];
        led3 = p.fullData.payload[2];
        visibleDeviceState->notify(visualizationClient, SensorState::LED_CHANGE);
        return true;

    default:
        return V2Device::consumeCommand(relativeTimeMs, p, visualizationClient);
    }
}

void DeviceMotionDetector::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualizationClient &visualizationClient)
{
    if (visualizationClient.useAsInputSource())
    {
        int newValue = visualizationClient.getInputState() != 0 ? 1 : 0;
        if (newValue == false && newValue != sensorValue)
        {
            IOPacket packet(uid, isV2 ? MOTION_DETECTOR_V2_CALLBACK_DETECTION_CYCLE_ENDED : MOTION_DETECTOR_CALLBACK_DETECTION_CYCLE_ENDED);
            brickStack->dispatchCallback(packet);
        }
        else if (newValue && newValue != sensorValue)
        {
            IOPacket packet(uid, isV2 ? MOTION_DETECTOR_V2_CALLBACK_MOTION_DETECTED : MOTION_DETECTOR_CALLBACK_MOTION_DETECTED);
            brickStack->dispatchCallback(packet);
        }
        sensorValue = newValue;
        return;
    }

    if (sensorValue) {
        if (relativeTimeMs > motionEnd) {
            IOPacket packet(uid, isV2 ? MOTION_DETECTOR_V2_CALLBACK_DETECTION_CYCLE_ENDED : MOTION_DETECTOR_CALLBACK_DETECTION_CYCLE_ENDED);
            brickStack->dispatchCallback(packet);
            sensorValue = false;
            notify(visualizationClient);
        }
    }
    // a new motion will be detected only with small break of 1,5 seconds from the last cycle
    else if (valueProvider->getValue(relativeTimeMs) > 0 && motionEnd +1500 <= relativeTimeMs)
    {
        IOPacket packet(uid, isV2 ? MOTION_DETECTOR_V2_CALLBACK_MOTION_DETECTED : MOTION_DETECTOR_CALLBACK_MOTION_DETECTED);
        brickStack->dispatchCallback(packet);
        sensorValue = true;
        motionEnd = relativeTimeMs + 5000;  // 5 seconds after start
        notify(visualizationClient);
    }
}

} /* namespace stubserver */
