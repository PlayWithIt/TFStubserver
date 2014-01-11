/*
 * ClientThread.h
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

#ifndef CLIENTTHREAD_H_
#define CLIENTTHREAD_H_

#include <utils/AsyncTask.h>

#include "BrickClient.h"
#include "BrickStack.h"

/**
 * A separate thread which consumes incoming requests from the network
 * socket and sends them to the stack of bricks. The stack will then
 * asynchronously respond if necessary. This objects creates a separate
 * thread for sending outgoing requests back.
 */
class ClientThread : public utils::AsyncTask, BrickClient
{
    int socketHandle;
    BrickStack &brickStack;
    uint64_t packetsIn;

    /**
     * Separate thread that handles the responses. The overall throughput
     * is the same as if the call is directly in sendResponse().
     */
    class SenderThread : public utils::AsyncTask
    {
        int socketHandle;
        uint64_t packetsOut;
        std::mutex queueMutex;
        std::queue<IOPacket> sendQueue;
        std::condition_variable queueEvent;

    public:
        SenderThread(int _socketHandle)
         : AsyncTask("SenderThread"), socketHandle(_socketHandle), packetsOut(0), queueMutex(), sendQueue(), queueEvent()
        { }

        /**
         * Put a new packet into the outgoing queue.
         */
        void enqueue(const IOPacket& packet);

        virtual void run();

        /**
         * Wake up the thread which consumes the queue.
         */
        void notify();
    };

    // created if necessary to send responses
    SenderThread *senderThread;

    // terminate thread and release memory
    void cleanUpSenderThread();

public:
    ClientThread(int socketHandle, BrickStack &_brickStack);
    virtual ~ClientThread();

    virtual void run();
    virtual bool sendResponse(const IOPacket& packet);
};


#endif /* CLIENTTHREAD_H_ */
