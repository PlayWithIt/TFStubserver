/*
 * DeviceInOut.h
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
#ifndef DEVICEINOUT_H_
#define DEVICEINOUT_H_

#include "DeviceRelay.h"

namespace stubserver {

/**
 * The digital input with a given number of input switches.
 */
class DeviceDigitalIn : public DeviceRelay
{
public:
    struct EdgeCount
    {
        unsigned count;
        unsigned debouncePeriod;
        uint64_t relativeTimeMs;
        bool     active;
        uint8_t  directions;

        EdgeCount();
    };

protected:
    utils::ValueProvider *valueProvider;
    BasicCallback         interruptCallback;
    unsigned              interruptMask;
    unsigned              currentValue;
    EdgeCount             edgeCount[8];

public:
    DeviceDigitalIn(utils::ValueProvider *vp, unsigned numPins = 4);

    /**
     * Release old value provider and use the new one.
     */
    void changeValueProvider(utils::ValueProvider *newValueProvider);

    DECLARE_OWN_DEVICE_CALLBACKS
};



/**
 * Input/Output 4 bricklet (IO4): the devices has a variable
 * number of pins, because it is reused for the IO16.
 */
class DeviceInOut : public DeviceDigitalIn
{
    // TODO if true, this behaves similar as a relay
    // bool    outputMode;
    uint8_t directionMask;
    uint8_t valueMask;

public:
    DeviceInOut(utils::ValueProvider *vp, unsigned numPins = 4);

    DECLARE_OWN_DEVICE_CALLBACKS
};



/**
 * Input/Output 16 bricklet (IO16).
 */
class DeviceInOut16 : public DeviceFunctions
{
    DeviceInOut portA;
    DeviceInOut portB;

public:
    DeviceInOut16(utils::ValueProvider *a, utils::ValueProvider *b);

    void changeValueProviderA(utils::ValueProvider *newValueProvider) {
        portA.changeValueProvider(newValueProvider);
    }
    void changeValueProviderB(utils::ValueProvider *newValueProvider) {
        portB.changeValueProvider(newValueProvider);
    }

    DECLARE_OWN_DEVICE_CALLBACKS
};

} /* namespace stubserver */

#endif /* DEVICEINOUT_H_ */
