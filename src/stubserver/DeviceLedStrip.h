/*
 * DeviceLedStrip.h
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

#ifndef STUBSERVER_DEVICELEDSTRIP_H_
#define STUBSERVER_DEVICELEDSTRIP_H_

#include "DeviceFunctions.h"
#include "VisualizationClient.h"

namespace stubserver {

/**
 * A stub-device for the LED strip bricklet. This makes not very much, because it
 * just consumes the data, but it calls the callbacks. It assumes that the minimum
 * frame duration is equals to the number of LEDs:<br>
 * if there are 320 LEDs => min frame duration 320ms<br>
 * if there are 100 LEDs => min frame duration 100ms<br>
 */
class DeviceLedStrip : public DeviceFunctions, public LedStripState
{
    unsigned chipType;
    unsigned frequency;
    BasicCallback frameRenderedCb;

public:
    DeviceLedStrip(const char* numLeds);

    DECLARE_OWN_DEVICE_CALLBACKS
};

} /* namespace stubserver */

#endif /* STUBSERVER_DEVICELEDSTRIP_H_ */
