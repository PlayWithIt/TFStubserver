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
    BrickStack          *brickStack;
    utils::Properties   *properties;
    DeviceFunctions     *functions;
    VisualizationClient *visualizationClient;   // is always set, maybe a dummy instance

    // child devices e.g. of a MasterBrick: children are in the stack or at a port a..d
    std::vector<SimulatedDevice*> children;
    std::mutex deviceMutex;

    std::string deviceTypeName;        // device type e.g. MASTER, LCD as string
    std::string uidStr;                // device uid as string
    std::string connectedUidStr;       // 'parent' device where this device is connected to
    std::string label;                 // device specific label for the optional GUI

    unsigned    uid;                   // device uid as number
    unsigned    deviceTypeId;
    char        position;              // port a..d or stack position 0..6
    bool        isBrick;
    uint8_t     hardwareVersion[3];
    uint8_t     firmwareVersion[3];

    DeviceFunctions *setupFunctions();

    // release resources
    void cleanup();

    /**
     * Read a property from the properties: first with UID prefix and if
     * this doesn't exist without prefix. Check for min expected length.
     */
    const char *getProperty(const std::string &key, int minLength = 0);
    const char *getProperty(const std::string &key, const char *defaultValue);
    int getIntProperty(const std::string &key);

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
     * called by the BrickStack periodically after {@link #consumePacket()}.
     */
    void checkCallbacks();

    /**
     * Connect a child with this device: the child is located at a
     * specific port (A-D) or position in the stack (0..7) if the child
     * is another brick.
     */
    void connect(SimulatedDevice* child);

    /**
     * Returns the device type identifier for Masterbrick, Servo etc, the values are
     * those from the TF header files, like MASTER_DEVICE_IDENTIFIER, AMBIENT_LIGHT_DEVICE_IDENTIFIER etc.
     */
    unsigned getDeviceTypeId() const {
        return deviceTypeId;
    }

    /**
     * Returns a type string.
     */
    const std::string& getDeviceTypeName() const {
        return deviceTypeName;
    }

    /**
     * Returns the position in the stack, this is either a port code (a..d)
     * for a bricklet connected to a brick or a number char (0..6) which
     * indicates that the device is a brick.
     */
    char getPosition() const {
        return position;
    }

    const std::string& getConnectedUidStr() const {
        return connectedUidStr;
    }

    /**
     * Returns the device uid.
     */
    unsigned int getUid() const {
        return uid;
    }

    /**
     * Set one VisualizationClient which must exist as long as it is registered here!
     */
    void setVisualizationClient(VisualizationClient &client) const;

    /**
     * Clear a VisualizationClient set before using {@link #setVisualizationClient()}.
     */
    void clearVisualizationClient() const;

    /**
     * Is this device connected to the given brick? This is true if the device is
     * the brick or the parent of a bricklet.
     */
    bool isConnectedTo(const std::string& brickId) const {
        return uidStr == brickId || connectedUidStr == brickId;
    }

    /**
     * Returns true if this brick is not connected to any other brick, so this brick
     * is at the bottom of the stack.
     */
    bool isMainBrick() const {
        return connectedUidStr.compare("0") == 0;
    }

    /**
     * Returns true if this bricklet is in the stack where the given brickUid is a
     * parent of this bricklet. This methid also returns true if the given uid
     * actually matches the UID of the current object.
     */
    bool isChildOf(const std::string &brickUid) const;

    /**
     * Returns the base58 encoded uid as string.
     */
    const std::string& getUidStr() const {
        return uidStr;
    }

    /**
     * Returns the optional label string.
     */
    const std::string& getLabel() const {
        return label;
    }
};

} /* namespace stubserver */

#endif /* SIMULATEDDEVICE_H_ */
