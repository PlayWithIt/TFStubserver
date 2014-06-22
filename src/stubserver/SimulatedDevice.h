/*
 * SimulatedDevice.h
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

#ifndef SIMULATEDDEVICE_H_
#define SIMULATEDDEVICE_H_

#include <string>
#include <vector>
#include <mutex>
#include <utils/Properties.h>

#include "DeviceFunctions.h"

namespace stubserver {


/**
 * The main class which is used in the {@link BrickStack} for all software
 * simulated devices. Internally this class constructs the devices out
 * of {@link DeviceFunctions} or specific device implementations.
 */
class SimulatedDevice
{
    BrickStack *brickStack;
    utils::Properties *properties;
    DeviceFunctions *functions;

    // child devices e.g. of a MasterBrick: children are in the stack or at a port a..d
    std::vector<SimulatedDevice*> children;
    std::mutex deviceMutex;

    unsigned    uid;                   // device uid as number
    unsigned    typeId;
    char        position;              // port a..d or stack position 0..6
    bool        isBrick;
    bool        visibleStateChange;
    uint8_t     hardwareVersion[3];
    uint8_t     firmwareVersion[3];
    std::string uidStr;                // device uid as string
    std::string connectedUidStr;       // 'parent' device where this device is connected to

    DeviceFunctions *setupFunctions();

    /**
     * Read a property from the properties: first with UID prefix and if
     * this doesn't exist without prefix. Check for min expected length.
     */
    const char *getProperty(const std::string &key, int minLength = 0);
    const char *getProperty(const std::string &key, const char *defaultValue);
    utils::ValueProvider* createValueProvider(const char *options);

public:
    SimulatedDevice(BrickStack *_brickStack, const char *_uidStr, unsigned int _typeId);
    SimulatedDevice(BrickStack *_brickStack, const char *_uidStr, const utils::Properties &props);
    virtual ~SimulatedDevice();

    /**
     * Consumes a packet with a given function code. This might modify
     * the packet contents, if the result is 'true': in this case, the
     * packet should be returned to the client where the packet came
     * from. If the result is false, the packet was consumed. There is never
     * a direct response to the client: this must be handled by the called
     * since the called knows the responseExpected flag.
     * <P>
     * In case of error, an exception is thrown.
     */
    bool consumePacket(IOPacket &p, bool responseExpected);

    /**
     * Check if any of the callbacks should trigger a response. This method is
     * called by the BrickStack periodically.
     */
    void checkCallbacks();

    /**
     * Connect a child with this device: the child is located at a
     * specific port (A-D) or position in the stack (0..7) if the child
     * is another brick.
     */
    void connect(SimulatedDevice* child);

    /**
     * Returns the device type identifier for Masterbrick, Servo etc.
     */
    unsigned int getTypeId() const {
        return typeId;
    }

    /**
     * Returns the device uid.
     */
    unsigned int getUid() const {
        return uid;
    }

    /**
     * Returns the base58 encoded uid as string.
     */
    const std::string& getUidStr() const {
        return uidStr;
    }
};

} /* namespace stubserver */

#endif /* SIMULATEDDEVICE_H_ */
