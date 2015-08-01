/*
 * BrickStack.h
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

#ifndef SIMULATEDSTACK_H_
#define SIMULATEDSTACK_H_

#include <atomic>
#include <chrono>
#include <list>
#include <queue>
#include <mutex>
#include <tuple>

#include "SimulatedDevice.h"


typedef std::lock_guard<std::mutex> MutexLock;

namespace stubserver {

class BrickClient;

using std::chrono::system_clock;

/**
 * Simulates the stack of bricks and bricklets. This class offers methods
 * to check for callbacks and dispatch them to the clients. An external class
 * must call {@link consumeRequestQueue()} and {@link checkCallbacks()}
 * periodically in order to process callbacks and request / response data.
 */
class BrickStack
{
    typedef std::tuple<BrickClient*, IOPacket> TQueueItem;

    std::mutex queueMutex;
    std::mutex deviceMutex;
    std::mutex clientMutex;

    std::list<SimulatedDevice*> devices;
    std::list<BrickClient*> clients;
    std::queue<TQueueItem> packetQueue;

    // the UIDs of the brick, index 0 is position '0'
    std::string bricks[10];

    // current time as relative time in milliseconds
    system_clock::time_point startTime;
    uint64_t relativeTimeMs;
    uint64_t packetsIn;
    uint64_t packetsOut;
    uint64_t callbackCycles;

    std::atomic_bool doReconnect;
    std::atomic_int  doEnumerateWithType;
    std::atomic_uint reconnectCount;

    void enumerate(uint8_t enumType, const std::string &uid);

public:
    /**
     * Init the stack from a configuration file which has property format.
     */
    BrickStack(const char *filename);
    virtual ~BrickStack();

    /**
     * Put one client request into the queue:
     * - enumerate will be returned to all clients
     * - get-value is returned to a single client
     */
    void enqueueRequest(BrickClient *cln, const IOPacket& packet);
    void dispatchCallback(const IOPacket& packet);

    void consumeRequestQueue();
    void checkCallbacks();

    /**
     * Mark a stack position as used.
     *
     * @param position - position char '0'..'9'
     * @param uid - brick's uid
     */
    void addBrick(char position, const std::string &uid);

    /**
     * Trigger an enumeration.
     */
    void initEnumerate(uint8_t enumType) {
        doEnumerateWithType = enumType;
    }

    /**
     * A method that should simulate a disconnect event of a brick (like USB unplug and plug again).
     */
    void reconnectBricks();

    /**
     * Update the current relative time
     */
    void incrementTime(const system_clock::time_point &now);

    uint64_t getRelativeTimeMs() const {
        return relativeTimeMs;
    }

    /**
     * Find the device with the given ID in the list of devices
     * @return NULL if the uid is not known.
     */
    SimulatedDevice* getDevice(unsigned int uid);
    SimulatedDevice* getDevice(const std::string &uid);

    /**
     * Returns a copy of the list of currently configured devices.
     */
    std::list<const SimulatedDevice*> getDevices() const {
        std::list<const SimulatedDevice*> result;
        for (auto it : devices)
            result.push_back(it);
        return result;
    }

    /**
     * Registers a client that will be triggered with callbacks.
     */
    void registerClient  (BrickClient *cln);
    void deregisterClient(BrickClient *cln);
};

} /* namespace stubserver */

#endif /* SIMULATEDSTACK_H_ */
