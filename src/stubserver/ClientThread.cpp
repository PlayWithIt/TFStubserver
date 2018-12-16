/*
 * ClientThread.cpp
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

#include <netinet/tcp.h>
#include <netinet/in.h>
#include <unistd.h>
#include <memory.h>

#include <utils/Log.h>
#include <utils/utils.h>

#include "ClientThread.h"
#include "SimulatedDevice.h"

namespace stubserver {

using utils::Log;

/**
 *
 */
ClientThread::ClientThread(int _socketHandle, BrickStack &_brickStack, bool _logRequests)
    : QueueProducerTask("ClientThread")
    , socketHandle(_socketHandle)
    , brickStack(_brickStack)
    , packetsIn(0)
    , logRequests(_logRequests)
    , senderThread(NULL)
{
    int flag = 1;
    if (setsockopt(socketHandle, IPPROTO_TCP, TCP_NODELAY, (void *)&flag, sizeof(flag)) < 0) {
        Log::perror("setsockopt()");
        close(socketHandle);
        socketHandle = -1;
    }
    else
        brickStack.registerClient(this);
}

ClientThread::~ClientThread()
{
    cleanUpSenderThread();

    if (socketHandle > 0) {
        brickStack.deregisterClient(this);
        close(socketHandle);
    }
}

void ClientThread::cleanUpSenderThread()
{
    if (senderThread == NULL)
        return;
    queue.closeQueue();
    senderThread->signalToStop();
    senderThread->stop(50);
    delete senderThread;
    senderThread = NULL;
}

/**
 *
 */
void ClientThread::run()
{
    if (socketHandle <= 0)
        return;

    IOPacket packet;
    int headerSize = sizeof(packet.header);

    // create response thread
    senderThread = new SenderThread(socketHandle, queue);
    senderThread->start();

    do {
        uint8_t* dataPtr = (uint8_t*) &packet;
        bzero(&packet, sizeof(packet));

        int size = recv(socketHandle, dataPtr, headerSize, 0);
        if (size < 0) {
            Log::perror("ClientThread::run() - read() error");
            continue;
        }
        if (size == 0) {
            // Client closed socket -> finish
            cleanUpSenderThread();
            brickStack.deregisterClient(this);
            close(socketHandle);
            socketHandle = -1;
            break;
        }

        while (size < headerSize)
        {
            int n = recv(socketHandle, dataPtr + size, headerSize - size, 0);
            if (n < 0) {
                Log::perror("read()");
                break;
            }
            size += n;
        }
        while (size < packet.header.length)
        {
            int n = recv(socketHandle, dataPtr + size, packet.header.length - size, 0);
            if (n < 0) {
                Log::perror("ClientThread::run() - recv() error");
                break;
            }
            size += n;
        }

        // put request into queue
        ++packetsIn;


        // Just for testing ...
        if (logRequests)
        {
            char msg[1024];
            int len = sprintf(msg, "Request for uid=%-6s, func-id %3d, msg-size %2d -",
                              utils::base58Encode(packet.header.uid).c_str(),
                              packet.header.function_id, packet.header.length);
            for (unsigned i = 8; i < packet.header.length; ++i) {
                len += sprintf(msg + len, " %02X", packet.fullData.payload[i-8]);
            }
            Log::log(msg);
        }
        brickStack.enqueueRequest(this, packet);

    } while (!shouldFinish(100));

    queue.closeQueue();
    Log::log("ClientThread has stopped: packets in", packetsIn);
}

/**
 * Just enqueue the response or callback.
 */
bool ClientThread::sendResponse(const IOPacket& packet)
{
    queue.enqueue(packet);
    return true;
}

/**
 * Consume one data packet: send this back to the client.
 */
bool ClientThread::SenderThread::consume(IOPacket &packet)
{
    int size = send(socketHandle, &packet, packet.header.length, 0);
    if (size != packet.header.length) {
        Log::perror("ClientThread::SenderThread::consume /send()");
    }
    ++packetsOut;
    return true;
}

} /* namespace stubserver */
