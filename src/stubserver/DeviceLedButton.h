/*
 * DeviceLedButton.h
 *
 * Copyright (C) 2018 Holger Grosenick
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

#ifndef STUBSERVER_DEVICELEDBUTTON_H_
#define STUBSERVER_DEVICELEDBUTTON_H_

#include "DeviceFunctions.h"
#include "VisualizationClient.h"

namespace stubserver {

/**
 * Simulates the Led button bricklet.
 */
class DeviceLedButton: public DeviceFunctions, public DualButtonState
{
    ValueProvider *valueProvider;

    // flags how status LED should behave, the LED ON/OFF state is kept in DualButtonState
    uint8_t ledConfig;

public:
    /**
     * The ValueProvider must provide <br>
     * 0 -> both buttons off<br>
     * 1 -> left button down<br>
     * 2 -> right button down<br>
     * 3 -> both buttons down<br>
     */
    DeviceLedButton(ValueProvider *vp);
    virtual ~DeviceLedButton();

    DECLARE_OWN_DEVICE_CALLBACKS
};

} /* namespace stubserver */

#endif /* STUBSERVER_DEVICELEDBUTTON_H_ */
