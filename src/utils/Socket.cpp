/*
 * Socket.cpp
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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <memory.h>
#include <strings.h>
#include <stdexcept>

#include "Log.h"
#include "Socket.h"
#include "StringUtil.h"


namespace utils {

/**
 * Open the socket.
 */
Socket::Socket(int port, bool reuse)
  : handle(-1)
  , timeoutRead(0)
  , timeoutWrite(0)
  , closeAll(false)
{
    char msg[512];

    handle = socket(AF_INET, SOCK_STREAM, 0);
    if (handle < 0)
    {
        sprintf(msg, "Create socket(AF_INET) on port %d failed: %s", port, strings::strerror(errno).c_str());
        throw std::runtime_error(msg);
    }

    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    int on = reuse ? 1 : 0;
    if (setsockopt(handle, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
    {
        Log::perror("Failed to set socket option SO_REUSEADDR", errno);
    }

    Log::log("Bind a socket on port", port);
    if (bind(handle, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        ::close(handle);
        sprintf(msg, "Bind socket %d failed: %s", port, strings::strerror(errno).c_str());
        throw std::runtime_error(msg);
    }
    if (listen(handle, 5) < 0)
    {
        ::close(handle);
        sprintf(msg, "Socket listen %d failed: %s", port, strings::strerror(errno).c_str());
        throw std::runtime_error(msg);
    }
    closeAll = false;
}

/**
 * Close all ...
 */
Socket::~Socket()
{
    close();
}

/**
 * Close all ...
 */
void Socket::close()
{
    closeAll = true;
    if (handle > 0)
    {
        Log::log("Socket::close()");
        shutdown(handle, SHUT_RDWR);
        ::close(handle);
        handle = -1;
    }
}

/**
 * Wait for a new client connection.
 */
int Socket::waitForClient()
{
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);

    int newsockfd;
    while ( (newsockfd = accept4(handle, (struct sockaddr *) &cli_addr, &clilen, SOCK_CLOEXEC)) < 0)
    {
        if (closeAll)
            return -1;

        // interrupted call: repeat, otherwise terminate
        if (Log::perror("Socket::waitForClient - accept()") != EINTR)
            return -4;
    }

    // TCP_NODELAY: disable Nagle alg. (200ms wait until packet gets transferred
    int flag = 1;
    if (setsockopt(newsockfd, IPPROTO_TCP, TCP_NODELAY, (void *)&flag, sizeof(flag)) < 0) {
        Log::perror("Socket::waitForClient - setsockopt(TCP_NODELAY)");
        ::close(newsockfd);
        return -4;
    }

    if (timeoutRead > 0)
    {
        struct timeval tv;
        tv.tv_sec = timeoutRead / 1000;
        tv.tv_usec = timeoutRead * 1000;

        if (setsockopt(newsockfd, IPPROTO_TCP, SO_RCVTIMEO, (void *)&tv, sizeof(tv)) < 0) {
            Log::perror("Socket::waitForClient - setsockopt(SO_RCVTIMEO)");
        }
    }

    if (timeoutWrite > 0)
    {
        struct timeval tv;
        tv.tv_sec = timeoutWrite / 1000;
        tv.tv_usec = timeoutWrite * 1000;

        if (setsockopt(newsockfd, IPPROTO_TCP, SO_SNDTIMEO, (void *)&tv, sizeof(tv)) < 0) {
            Log::perror("Socket::waitForClient - setsockopt(SO_SNDTIMEO)");
        }
    }
    return newsockfd;
}

} /* namespace utils */
