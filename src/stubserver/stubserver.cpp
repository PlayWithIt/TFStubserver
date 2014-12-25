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
#include <string.h>
#include <fstream>

#include <list>
#include <utils/Log.h>
#include <utils/Socket.h>
#include <utils/SoundPlayback.h>

#include "ClientThread.h"
#include "BrickThread.h"

using utils::Log;
using utils::Socket;

using stubserver::ClientThread;
using stubserver::BrickThread;


static std::list<ClientThread*> clients;
static BrickThread *brickThread = NULL;
static Socket *gSocket;
static std::mutex socketMutex;


// signal handler to terminate the loop
static void sigTermHandler(int sig, siginfo_t *, void *)
{
    if (gSocket) {
        utils::MutexLock lock(socketMutex);
        delete gSocket;
        gSocket = NULL;
    }
}

// signal handler to terminate the loop
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
 * Main control.
 */
int main(int argc, char *argv[])
{
    int port = 4223;
    const char *deviceList = NULL;

    std::ofstream os;
    bool logRequests = false;

    for (int i = 1; i < argc; ++i)
    {
        const char *arg = argv[i];
        if (strcmp(arg, "-p") == 0 && i+1 < argc)
        {
            int p = atol(argv[++i]);
            if (p < 1024 || p > 32000) {
                fprintf(stderr, "'port' must be between 1024 and 32000, but is %d!\n", p);
                return 1;
            }
            port = p;
            continue;
        }
        if (strcmp(arg, "-d") == 0 && i+1 < argc)
        {
            deviceList = argv[++i];
            continue;
        }
        if (strcmp(arg, "-rq") == 0)
        {
            logRequests = true;
            continue;
        }
        if (strcmp(arg, "-l") == 0 && i+1 < argc)
        {
            os.open(argv[++i], std::ofstream::out | std::ofstream::app);
            if (!os.is_open()) {
                fprintf(stderr, "open(%s): %s\n", argv[i], strerror(errno));
                return 1;
            }
            Log::setStream(os);
            continue;
        }

        if (strcmp(arg, "-?") != 0)
            printf("\nUnknown option '%s' !\n", arg);
        printf("\nUsage: stubserver {-p port} {-l logName} {-rq} -d devices.properties\n"
               "\n");
        return 1;
    }

    if (deviceList == NULL)
    {
        puts("\nERROR: option -d is mandatory!\n");
        return 1;
    }

    // first read the config file before creating the socket
    brickThread = new BrickThread(deviceList);
    brickThread->start();

    // then create the socket
    try {
        gSocket = new Socket(port);
    }
    catch (const std::exception &e) {
        Log::log(std::string("Opening a socket failed: ") + e.what());
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
    {
        utils::MutexLock lock(socketMutex);
        delete gSocket;
        gSocket = NULL;
    }

    brickThread->stop(100);
    for (auto it : clients)
        delete it;
    delete brickThread;

    // if a piezo speaker was used: release global data
    utils::SoundPlayback::releaseAll();

    return 0;
}
