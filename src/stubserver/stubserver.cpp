/*
 * main.cpp
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

#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <fstream>

#include <utils/Log.h>
#include <utils/Socket.h>
#include <utils/SoundPlayback.h>

#include "ClientThread.h"
#include "BrickThread.h"
#include "stubserver.h"

using utils::Log;
using utils::Socket;


namespace stubserver {


static std::list<ClientThread*> clients;
static BrickThread *brickThread = NULL;
static Socket *gSocket;
static std::mutex socketMutex;


// signal handler to terminate the loop
static void sigTermHandler(int sig, siginfo_t *, void *)
{
    stopServer();
}

// signal handler to trigger a brick reset
static void sigHupHandler(int sig)
{
    if (brickThread)
        brickThread->reconnectBricks();
}

/**
 * Install the signal handler to terminate the process.
 */
static void initSignalHandlers()
{
    struct sigaction act;
    memset(&act, 0, sizeof(act));

    // TERM + INT: terminate process
    act.sa_sigaction = sigTermHandler;
    act.sa_flags     = SA_SIGINFO;
    sigaction(SIGTERM, &act, NULL);
    sigaction(SIGINT,  &act, NULL);

    // HUP: disconnect and reconnect a brick (simulate)
    memset(&act, 0, sizeof(act));
    act.sa_handler = sigHupHandler;
    sigaction(SIGHUP, &act, NULL);
}

/**
 * Try to stop the server by closing the socket.
 *
 * @return if the server was active and the socket was closed, false if not
 */
bool stopServer()
{
    if (gSocket)
    {
        utils::MutexLock lock(socketMutex);
        delete gSocket;
        gSocket = NULL;
        return true;
    }
    return false;
}


/**
 * Retrieve the list of configured devices, this can be empty if
 * the server is not running.
 */
std::list<const SimulatedDevice*> getDevices()
{
    if (brickThread)
        return brickThread->getDevices();
    return std::list<const SimulatedDevice*>();  // empty
}

/**
 * Start the stubserver with the given configuration which points to the
 * device properties. This function return when the server gets stopped.
 */
int runServer(const char *deviceList, int port, const char *logName, bool logRequests)
{
    std::ofstream os;

    if (logName != NULL && *logName != 0)
    {
        // create a log-stream
        os.open(logName, std::ofstream::out | std::ofstream::app);
        if (!os.is_open()) {
            Log::perror(logName);
            return 2;
        }
        Log::setStream(os);
    }

    if (deviceList == NULL)
    {
        Log::error("ERROR: device list not defined, option -d is mandatory!");
        return 1;
    }

    if (port < 1024 || port > 32000)
    {
        Log::error("'port' must be between 1024 and 32000, but is", port);
        return 1;
    }

    // first read the config file before creating the socket
    try {
        brickThread = new BrickThread(deviceList);
    }
    catch (const std::exception &e) {
        delete brickThread;
        brickThread = NULL;
        throw;
    }

    brickThread->start();

    // then create the socket
    try {
        gSocket = new Socket(port);
    }
    catch (const std::exception &e) {
        Log::log(std::string("Opening a socket failed: ") + e.what());
        brickThread->stop();
        delete brickThread;
        brickThread = NULL;
        return 4;
    }
    initSignalHandlers();

    do {
        int newsockfd = gSocket->waitForClient();
        if (newsockfd < 0) {
            if (newsockfd == -1)
                break;
            Log::perror("accept()");
        }
        else {
            Log::log("Create a new client, fd =", newsockfd);
            ClientThread *client = new ClientThread(newsockfd, *brickThread, logRequests);
            client->start();

            // cleanup clients that have terminated meanwhile
            for (std::list<ClientThread*>::iterator it = clients.begin(); it != clients.end(); ) {
                if (!(*it)->isActive()) {
                    delete (*it);
                    it = clients.erase(it);
                }
                else
                    ++it;
            }

            clients.push_back(client);
        }
    } while (true);

    // should disconnect clients
    stopServer();

    // cleanup all the rest
    brickThread->stop();
    for (auto it : clients)
        delete it;
    clients.clear();

    delete brickThread;
    brickThread = NULL;

    // if a piezo speaker was used: release global data
    utils::SoundPlayback::releaseAll();

    return 0;
}

}
