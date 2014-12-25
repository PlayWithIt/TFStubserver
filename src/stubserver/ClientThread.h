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

#include <utils/BlockingQueueTasks.h>

#include "BrickClient.h"
#include "BrickStack.h"

namespace stubserver {

/**
 * A separate thread which consumes incoming requests from the network
 * socket and sends them to the stack of bricks. The stack will then
 * asynchronously respond if necessary. This objects creates a separate
 * thread for sending outgoing requests back to the client.
 */
class ClientThread : public utils::QueueProducerTask<IOPacket>, BrickClient
{
    int         socketHandle;
    BrickStack &brickStack;
    uint64_t    packetsIn;
    bool        logRequests;

    /**
     * Separate thread that handles the responses to the client. The overall throughput
     * is the same as if the call is directly in sendResponse().
     */
    class SenderThread : public utils::QueueConsumerTask<IOPacket>
    {
        int socketHandle;
        uint64_t packetsOut;

    public:
        SenderThread(int _socketHandle, utils::BlockingQueue<IOPacket> &queue)
         : QueueConsumerTask("SenderThread", queue), socketHandle(_socketHandle), packetsOut(0)
        { }

        virtual bool consume(IOPacket &data);
    };

    // created if necessary to send responses
    SenderThread *senderThread;

    // terminate thread and release memory
    void cleanUpSenderThread();

public:
    ClientThread(int socketHandle, BrickStack &_brickStack, bool logRequests = false);
    virtual ~ClientThread();

    virtual void run();
    virtual bool sendResponse(const IOPacket& packet);
};

} /* namespace stubserver */

#endif /* CLIENTTHREAD_H_ */
