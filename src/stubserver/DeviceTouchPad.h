/*
 * DeviceButtons.h
 *
 * Copyright (C) 2013 Holger Grosenick
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


#ifndef DEVICEBUTTONS_H_
#define DEVICEBUTTONS_H_

#include "DeviceFunctions.h"

namespace stubserver {

/**
 * A touch pad / multi touch panel.
 */
class DeviceTouchPad : public V2Device
{
    unsigned numButtons;
    unsigned sensitivity;
    unsigned currentState;
    unsigned enabledBits;
    ValueProvider *values;

public:
    DeviceTouchPad(unsigned _numButtons, ValueProvider *vp, bool isV2);
    ~DeviceTouchPad();

    unsigned getNumButtons() const {
        return numButtons;
    }

    DECLARE_OWN_DEVICE_CALLBACKS
};

} /* namespace stubserver */

#endif /* DEVICEBUTTONS_H_ */
