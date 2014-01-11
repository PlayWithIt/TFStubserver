/*
 * BrickStack.cpp
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

#include <sys/time.h>
#include <sstream>

#include <utils/Log.h>
#include <utils/utils.h>
#include <utils/StringUtil.h>
#include <utils/Exceptions.h>

#include <device_identifiers.h>
#include <ip_connection.h>

#include "BrickStack.h"
#include "BrickClient.h"


using utils::Log;
using utils::Properties;

#define ATTRIBUTE_PACKED __attribute__((packed))

typedef struct {
        PacketHeader header;
        char uid[8];
        char connected_uid[8];
        char position;
        uint8_t hardware_version[3];
        uint8_t firmware_version[3];
        uint16_t device_identifier;
        uint8_t enumeration_type;
} ATTRIBUTE_PACKED EnumerateCallback;


static int objectCount = 0;

/**
 * Init the stack: create devices from a config file ...
 */
BrickStack::BrickStack(const char *filename)
  : queueMutex()
  , deviceMutex()
  , clientMutex()
  , devices()
  , clients()
  , packetQueue()
  , startTimeMs(0)
  , relativeTimeMs(0)
  , packetsIn(0)
  , packetsOut(0)
  , callbackCycles(0)
{
    if (objectCount > 0)
        throw utils::Exception("ERROR: there maybe only one BrickStack at a time!");
    ++objectCount;

    // determine start time in ms
    struct timeval current;
    gettimeofday(&current, NULL);
    startTimeMs = (current.tv_sec * 1000) + (current.tv_usec / 1000);

    if (filename == NULL)
    {
        Log::log("BrickStack has NO devices!");
        return;
    }

    // load config from properties
    Properties p(filename);
    std::string str = p.get("UIDS", "");
    if (str.length() == 0)
        throw utils::Exception("Property 'UIDS' is not present!");

    std::vector<std::string> uids;
    for (std::string &it : utils::StringUtil::split(str, ' ', uids))
    {
        SimulatedDevice *dev = new SimulatedDevice(this, it.c_str(), p);
        devices.push_back(dev);
    }
}


BrickStack::~BrickStack()
{
    for (auto it : devices)
        delete it;
    --objectCount;

    struct timeval current;
    gettimeofday(&current, NULL);
    uint64_t endTimeMs = (current.tv_sec * 1000) + (current.tv_usec / 1000);

    std::ostringstream os;
    os << "UpTime: " << (endTimeMs - startTimeMs) << "ms, "
       << callbackCycles << " callback cycles ("
       << static_cast<double>(callbackCycles) / ((endTimeMs - startTimeMs) / 1000.0) << " cyles/sec), "
       << packetsIn << " packets in, " << packetsOut << " packets out";
    Log::log(os.str());
}

/**
 * Read realtime and recalculate the relative time
 */
void BrickStack::incrementTime()
{
    // determine start time in ms
    struct timeval current;
    gettimeofday(&current, NULL);
    uint64_t newTimeMs = (current.tv_sec * 1000) + (current.tv_usec / 1000);
    relativeTimeMs = newTimeMs - startTimeMs;
}

/**
 * Dispatches a callback back to known clients. This method is called out of
 * the {@link checkCallbacks()} via the simulated device. {@link checkCallbacks()}
 * already gets the lock for the client list.
 */
void BrickStack::dispatchCallback(const IOPacket &packet)
{
    for (auto cln : clients) {
        cln->sendResponse(packet);
        ++packetsOut;
    }
}

/**
 * Sends a notification to specific clients which registered for state
 * changed notifications.
 * @param uid id of the device that has changed.
 */
void BrickStack::dispatchStateChanged(uint32_t uid)
{
    printf("State changed for UID = %u\n", uid);
}

/**
 * Put one client request into the queue:
 * - enumerate will be returned to all clients
 * - get-value is returned to a single client
 */
void BrickStack::enqueueRequest(BrickClient *cln, const IOPacket& packet)
{
    MutexLock lock(queueMutex);
    packetQueue.push(std::make_tuple(cln, packet));
    ++packetsIn;
}

/**
 * Check all devices if callbacks must be executed.
 */
void BrickStack::checkCallbacks()
{
    MutexLock lock(clientMutex);
    ++callbackCycles;
    for (auto it : devices)
    {
        it->checkCallbacks();
    }
}

/**
 *
 */
void BrickStack::consumeRequestQueue()
{
    MutexLock lock(queueMutex);
    char msg[128];

    while (false == packetQueue.empty())
    {
        auto item = packetQueue.front();
        IOPacket& packet = std::get<1>(item);
        unsigned uid = packet.header.uid;

        if (uid == 0)
        {
            // stack commands, returned to all clients
            if (packet.header.function_id == 254)
                enumerate();
        }
        else {
            // brick / bricklet command
            SimulatedDevice* dev = getDevice(uid);
            if (dev == NULL) {
                std::string uidStr = utils::base58Encode(uid);
                sprintf(msg, "ERROR: Cannot find device with uid %s (%x)", uidStr.c_str(), uid);
                Log::log(msg);
            }
            else {
                try {
                    bool responseExpected = packet.packet_header_get_response_expected();
                    bool done = dev->consumePacket(packet, responseExpected);

                    if (!done)
                    {
                        // not yet implemented
                        sprintf(msg, "ERROR: function %d for device %s (%x) NOT CONSUMED!",
                                packet.header.function_id, dev->getUidStr().c_str(), packet.header.uid);
                        Log::log(msg);
                        packet.setErrorCode(IOPacket::ErrorCode::NOT_SUPPORTED);
                    }

                    if (responseExpected)
                    {
                        BrickClient* cln = std::get<0>(item);
                        if (cln == NULL) {
                            Log::log("Logic error in consumeRequestQueue(): client is NULL!");
                        }
                        else {
                            cln->sendResponse(packet);
                            ++packetsOut;
                        }
                    }
                }
                catch (const std::exception &ex) {
                    Log::log("ERROR in consumeRequestQueue()", ex);
                }
            }
        }

        packetQueue.pop();
    }
}

/**
 *
 */
void BrickStack::enumerate()
{
    MutexLock lock(clientMutex);

    IOPacket          packet;
    EnumerateCallback response;

    for (auto it : devices)
    {
        bzero(&packet, sizeof(packet));
        bzero(&response, sizeof(response));

        packet.header.function_id = 255;  // get identity
        bool ok = it->consumePacket(packet, true);
        if (!ok) {
            Log::log("Enumeration error !");
            break;
        }

        // copy manually since the memory layout is not identical!
        memcpy(response.uid, packet.identity.uid, sizeof(response.uid));
        memcpy(response.connected_uid, packet.identity.connected_uid, sizeof(response.connected_uid));
        memcpy(response.hardware_version, packet.identity.hardware_version, sizeof(response.hardware_version));
        memcpy(response.firmware_version, packet.identity.firmware_version, sizeof(response.firmware_version));
        response.device_identifier = packet.identity.device_identifier;
        response.position = packet.identity.position;

        response.header.length = sizeof(EnumerateCallback);
        response.header.function_id = IPCON_CALLBACK_ENUMERATE;
        response.enumeration_type = IPCON_ENUMERATION_TYPE_AVAILABLE;

        for (auto cln : clients)
            cln->sendResponse((IOPacket&) response);
    }

    Log::log("Enumerate #devices =", static_cast<int>(devices.size()));
}

/**
 * Find the device with the given ID in the list of devices, returns
 * NULL if the uid is not known.
 */
SimulatedDevice* BrickStack::getDevice(unsigned int uid)
{
    for (auto it : devices)
        if (it->getUid() == uid)
            return it;
    return NULL;
}

/**
 * Find the device with the given ID in the list of devices, returns
 * NULL if the uid is not known.
 */
SimulatedDevice* BrickStack::getDevice(const std::string &uid)
{
    for (auto it : devices)
        if (it->getUidStr() == uid)
            return it;
    return NULL;
}


void BrickStack::registerClient(BrickClient *cln)
{
    MutexLock lock(clientMutex);
    clients.push_back(cln);
}


void BrickStack::deregisterClient(BrickClient *cln)
{
    MutexLock lock(clientMutex);
    for (auto it = clients.begin(); it != clients.end(); ++it)
    {
        if ((*it) == cln) {
            clients.erase(it);
            return;
        }
    }
}
