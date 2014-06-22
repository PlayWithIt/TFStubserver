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

#include <list>
#include <queue>
#include <mutex>
#include <tuple>
#include <chrono>

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

    // current time as relative time in milliseconds
    system_clock::time_point startTime;
    uint64_t relativeTimeMs;
    uint64_t packetsIn;
    uint64_t packetsOut;
    uint64_t callbackCycles;

    void enumerate();

public:
    /**
     * Init the stack from a configuration file. If the filename is NULL,
     * some hard-coded dummy objects are created.
     */
    BrickStack(const char *filename = NULL);
    virtual ~BrickStack();

    /**
     * Put one client request into the queue:
     * - enumerate will be returned to all clients
     * - get-value is returned to a single client
     */
    void enqueueRequest(BrickClient *cln, const IOPacket& packet);
    void dispatchCallback(const IOPacket& packet);

    /**
     * Sends a notification to specific clients which registered for state
     * changed notifications.
     * @param uid id of the device that has changed.
     */
    void dispatchStateChanged(uint32_t uid);

    void consumeRequestQueue();
    void checkCallbacks();

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
     * Registers a client that will be triggered with callbacks.
     */
    void registerClient  (BrickClient *cln);
    void deregisterClient(BrickClient *cln);
};

} /* namespace stubserver */

#endif /* SIMULATEDSTACK_H_ */
