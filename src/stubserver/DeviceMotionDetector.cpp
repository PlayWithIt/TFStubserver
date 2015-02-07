/*
 * DeviceMotionDetector.cpp
 *
 * Copyright (C) 2014 Holger Grosenick
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

#include "BrickStack.h"
#include "DeviceMotionDetector.h"

namespace stubserver {


DeviceMotionDetector::DeviceMotionDetector(ValueProvider *vp)
  : valueProvider(vp)
  , motionEnd(0)
  , inMotion(false)
{
}


bool DeviceMotionDetector::consumeCommand(uint64_t relativeTimeMs, IOPacket &p, VisualisationClient &visualisationClient)
{
    // set default dummy response size: header only
    p.header.length = sizeof(p.header);

    // check function to perform
    switch (p.header.function_id) {
    case MOTION_DETECTOR_FUNCTION_GET_MOTION_DETECTED:
        p.uint8Value = inMotion ? MOTION_DETECTOR_MOTION_DETECTED : MOTION_DETECTOR_MOTION_NOT_DETECTED;
        p.header.length += sizeof(p.uint8Value);
        return true;

    default:
        return false;
    }
}

void DeviceMotionDetector::checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, VisualisationClient &visualisationClient)
{
    if (inMotion) {
        if (relativeTimeMs > motionEnd) {
            IOPacket packet(uid, MOTION_DETECTOR_CALLBACK_DETECTION_CYCLE_ENDED);
            brickStack->dispatchCallback(packet);
            inMotion = false;
        }
    }
    // a new motion will be detected only with small break of 1,5 seconds from the last cycle
    else if (valueProvider->getValue(relativeTimeMs) > 0 && motionEnd +1500 <= relativeTimeMs)
    {
        IOPacket packet(uid, MOTION_DETECTOR_CALLBACK_MOTION_DETECTED);
        brickStack->dispatchCallback(packet);
        inMotion = true;
        motionEnd = relativeTimeMs + 5000;  // 5 seconds after start
    }
}

} /* namespace stubserver */
