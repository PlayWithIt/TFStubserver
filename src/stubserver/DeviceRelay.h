/*
 * DeviceRelay.h
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


#ifndef DEVICERELAY_H_
#define DEVICERELAY_H_

#include "DeviceFunctions.h"

namespace stubserver {

/**
 * A set of functions serving dual or quad-relay or digital quad out.
 * This supports open/close of the relay and the monoflop functionality.
 * <p>
 * Not yet supported: grouping.
 */
class DeviceRelay : public DeviceFunctions
{
protected:
    unsigned numSwitches;
    bool     bitSwitches;

    enum {
        FUNC_SET_STATE = 0,
        FUNC_SET_SELECTED,
        FUNC_GET_STATE,
        FUNC_SET_MONOFLOP,
        FUNC_GET_MONOFLOP
    };
    bool    switchOn[16];          // one flag per switch, max 16 switches
    uint8_t functionCodes[256];

    // possible callback
    std::vector<BasicCallback> callbacks;

    // Find the callback where param1 has the given 'pin' value.
    typedef std::vector<BasicCallback>::iterator CallbackIterator;

    void initMonoflopCallbacks(uint8_t callbackCode);

protected:
    DeviceRelay(unsigned _numSwitches, bool _bitSwitches);

public:
    bool consumeCommand(uint64_t relativeTimeMs, IOPacket &p, bool &stateChanged);
    void checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, bool &stateChanged);
};


/**
 * The solid state relay
 */
class DeviceSolidStateRelay : public DeviceRelay
{
public:
    DeviceSolidStateRelay();

    virtual bool consumeCommand(uint64_t relativeTimeMs, IOPacket &p, bool &stateChanged) override;
    virtual void checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, bool &stateChanged) override;
};


/**
 * The dual relay
 */
class DeviceDualRelay : public DeviceRelay
{
public:
    DeviceDualRelay();
};


/**
 * The industrial quad relay
 */
class DeviceQuadRelay : public DeviceRelay
{
public:
    DeviceQuadRelay();
};


/**
 * The digital out 4 switch
 */
class DeviceDigitalOut4 : public DeviceRelay
{
public:
    DeviceDigitalOut4();
};


/**
 * A remote switch: this differs a little bit from the other relays, since it
 * has a 'busy' flag and callback: while the relay is switching, no other command
 * is allowed.
 */
class DeviceRemoteRelay : public DeviceFunctions
{
    // switching is active
    bool     busy;
    unsigned repeats;
    uint64_t switchDoneAtMs;

public:
    DeviceRemoteRelay();

    bool consumeCommand(uint64_t relativeTimeMs, IOPacket &p, bool &stateChanged);
    void checkCallbacks(uint64_t relativeTimeMs, unsigned int uid, BrickStack *brickStack, bool &stateChanged);
};

} /* namespace stubserver */

#endif /* DEVICERELAY_H_ */
