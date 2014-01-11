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
#include <errno.h>
#include <memory.h>
#include <sstream>

#include <utils/Log.h>
#include <utils/utils.h>

#include "ClientThread.h"
#include "SimulatedDevice.h"

using utils::Log;

/**
 *
 */
ClientThread::ClientThread(int _socketHandle, BrickStack &_brickStack)
    : AsyncTask("ClientThread")
    , socketHandle(_socketHandle)
    , brickStack(_brickStack)
    , packetsIn(0)
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
    senderThread->signalToStop();
    senderThread->notify();
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
    senderThread = new SenderThread(socketHandle);
    senderThread->start();

    do {
        uint8_t* dataPtr = (uint8_t*) &packet;
        bzero(&packet, sizeof(packet));

        int size = recv(socketHandle, dataPtr, headerSize, 0);
        if (size < 0) {
            Log::perror("read()");
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
                Log::perror("read()");
                break;
            }
            size += n;
        }

        // put request into queue
        ++packetsIn;
        brickStack.enqueueRequest(this, packet);

    } while (!shouldFinish(1));

    Log::log("ClientThread has stopped: packets in", packetsIn);
}

/**
 * Just enqueue the response or callback.
 */
bool ClientThread::sendResponse(const IOPacket& packet)
{
    if (senderThread) {
        senderThread->enqueue(packet);
        return true;
    }
    return false;
}

/**
 * Main loop to consume responses for one client.
 */
void ClientThread::SenderThread::run()
{
    while (true)
    {
        std::unique_lock<std::mutex> l(queueMutex);
        queueEvent.wait(l);

        if (sendQueue.empty())
        {
            if (shouldFinish()) {
                // wakeUp without data -> terminate
                Log::log("SenderThread has stopped, packets out", packetsOut);
                return;
            }
            else {
                // see condition_variable api: the variable might be triggered without condition
                Log::log("SenderThread wakeup without data??");
                continue;
            }
        }

        do {
            auto packet = sendQueue.front();
            int size = send(socketHandle, &packet, packet.header.length, 0);
            if (size != packet.header.length) {
                Log::perror("send()");
            }
            ++packetsOut;
            sendQueue.pop();
        } while (!sendQueue.empty());
    }
}

void ClientThread::SenderThread::enqueue(const IOPacket& packet)
{
    {
        MutexLock lock(queueMutex);
        sendQueue.push(packet);
    }
    notify();
}

/**
 * Wake up the thread which consumes the queue.
 */
void ClientThread::SenderThread::notify()
{
    queueEvent.notify_one();
}
