/*
 * DeviceMotionDetector.h
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

#ifndef STUBSERVER_DEVICEMOTIONDETECTOR_H_
#define STUBSERVER_DEVICEMOTIONDETECTOR_H_

#include "DeviceFunctions.h"
#include "VisualisationClient.h"

namespace stubserver {

/**
 * Simple MotionDetector which has only two callbacks.
 */
class DeviceMotionDetector : public DeviceFunctions, public SensorState
{
    ValueProvider    *valueProvider;
    uint64_t         motionEnd;

public:
    /**
     * The ValueProvider just needs to provide 1 (on) and 0 (off).
     */
    DeviceMotionDetector(ValueProvider *vp);
    ~DeviceMotionDetector();

    DECLARE_OWN_DEVICE_CALLBACKS
};

} /* namespace stubserver */

#endif /* STUBSERVER_DEVICEMOTIONDETECTOR_H_ */
