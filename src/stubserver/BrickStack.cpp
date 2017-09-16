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

namespace stubserver {


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
  , startTime(system_clock::now())
  , relativeTimeMs(0)
  , packetsIn(0)
  , packetsOut(0)
  , callbackCycles(0)
  , doReconnect(false)
  , doEnumerateWithType(-1)
  , reconnectCount(0)
{
    if (objectCount > 0)
        throw utils::Exception("ERROR: there maybe only one BrickStack at a time!");

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

    try {
        for (std::string &it : utils::strings::split(str, ' ', uids))
        {
            SimulatedDevice *dev = new SimulatedDevice(this, it.c_str(), p);
            devices.push_back(dev);
        }

        // make some consistency checks
        if (bricks[0].length() == 0)
            throw utils::Exception("There is no brick on position '0' of the stack - this is invalid!");
    }
    catch (const std::exception &e) {
        // if an exception happens here, the destructor is not called!
        for (auto it : devices)
            delete it;
        throw;
    }

    // increment only if finally done
    ++objectCount;
}


BrickStack::~BrickStack()
{
    for (auto it : devices)
        delete it;
    --objectCount;

    system_clock::time_point endTimeMs = system_clock::now();
    uint64_t delta = std::chrono::duration_cast<std::chrono::milliseconds>(endTimeMs - startTime).count();

    Log() << "UpTime: " << delta << "ms, "
          << callbackCycles << " callback cycles ("
          << static_cast<double>(callbackCycles) / (delta / 1000.0) << " cyles/sec), "
          << packetsIn << " packets in, " << packetsOut << " packets out";
}

/**
 * Mark a stack position as used, it is allowed to use a position twice: there maybe
 * multiple stacks connected separately via USB -> multiple times position '0'
 * is used. But gaps are not allowed.
 *
 * @param position - position char '0'..'9'
 * @param uid - brick's uid
 */
void BrickStack::addBrick(char position, const std::string &uid)
{
    if (position > '9' || position < '0') {
        char msg[256];
        sprintf(msg, "ERROR: invalid position char '%c' (%d)", position, position);
        throw utils::Exception(msg);
    }
    unsigned index = position - '0';
    bricks[index] = uid;
}

/**
 * Recalculate the relative time
 */
void BrickStack::incrementTime(const system_clock::time_point &now)
{
    relativeTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();
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
    if (doReconnect)
    {
        int count = reconnectCount++;
        std::string uid;
        int brickNo;
        uint8_t type;

        if (count % 2 == 0)
        {
            brickNo = count / 2;
            type = IPCON_ENUMERATION_TYPE_DISCONNECTED;
        }
        else {
            brickNo = (count - 1) / 2;
            doReconnect = false;
            type = IPCON_ENUMERATION_TYPE_CONNECTED;
        }

        int act = 0;
        for (auto it : devices)
        {
            if (it->isMainBrick())
            {
                uid = it->getUidStr();
                if (act++ == brickNo)
                    break;
            }
        }
        if (brickNo > act)
            reconnectCount = 0;

        Log::log(type == IPCON_ENUMERATION_TYPE_CONNECTED ? "Reconnect brick" : "Disconnect brick", uid.c_str());
        enumerate(type, uid);
        return;
    }

    if (doEnumerateWithType >= 0 && doEnumerateWithType <= 2)
    {
        uint8_t t = doEnumerateWithType;
        doEnumerateWithType = -1;
        enumerate(t, "");
        return;
    }

    //--- start the lock if necessary
    {
        MutexLock lock(clientMutex);
        ++callbackCycles;
        for (auto it : devices)
        {
            it->checkCallbacks();
        }
    }
}

/**
 *
 */
void BrickStack::consumeRequestQueue()
{
    MutexLock lock(queueMutex);

    // consume data
    while (false == packetQueue.empty())
    {
        auto item = packetQueue.front();
        IOPacket& packet = std::get<1>(item);
        unsigned uid = packet.header.uid;

        // numeric 0 -> UID=1 as a base58 encoded literal
        if (uid == 0)
        {
            // stack commands, returned to all clients
            if (packet.header.function_id == FUNCTION_ENUMERATE)
                enumerate(IPCON_ENUMERATION_TYPE_AVAILABLE, "");

            // DISCONNECT_PROBE also goes into this branch
        }
        else {
            // brick / bricklet command
            SimulatedDevice* dev = getDevice(uid);
            if (dev == NULL) {
                std::string uidStr = utils::base58Encode(uid);
                Log(Log::ERROR) << "cannot find device with uid " << uidStr << " (" << std::hex << uid << ')';
            }
            else {
                try {
                    bool responseExpected = packet.packet_header_get_response_expected();
                    bool done = dev->consumePacket(packet, responseExpected);

                    if (!done)
                    {
                        // not yet implemented
                        Log(Log::ERROR) << "function " << (int) packet.header.function_id
                                << " (len=" << (int) packet.header.length << ") for device "
                                << dev->getDeviceTypeName() << " with id "
                                << dev->getUidStr() << '(' << packet.header.uid << ") NOT CONSUMED!";
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
                    Log::error("ERROR in consumeRequestQueue()", ex);
                }
            }
        }

        packetQueue.pop();
    }
}

/**
 * Send the information for all devices to all clients via enumerate callback message.
 */
void BrickStack::enumerate(uint8_t enumType, const std::string &uid)
{
    MutexLock lock(clientMutex);

    IOPacket          packet;
    EnumerateCallback response;
    int               enumerated = 0;

    // if uid is 0 -> enumerate all devices
    bool all = uid.length() == 0;

    for (auto it : devices)
    {
        if (!all && !it->isChildOf(uid))
            continue;

        bzero(&packet, sizeof(packet));
        bzero(&response, sizeof(response));

        packet.header.function_id = FUNCTION_GET_IDENTITY;  // get identity
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
        response.enumeration_type = enumType; // IPCON_ENUMERATION_TYPE_AVAILABLE;

        // put command into queue
        for (auto cln : clients)
            cln->sendResponse((IOPacket&) response);
        ++enumerated;
    }

    Log() << "Enumerated " << enumerated << " devices, enum-type was " << (int) enumType;
}

/**
 * A method that should simulate a disconnect event of a brick (like USB unplug and plug again).
 */
void BrickStack::reconnectBricks()
{
    doReconnect = true;
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

} /* namespace stubserver */
