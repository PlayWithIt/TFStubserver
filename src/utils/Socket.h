/*
 * Socket.h
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
#ifndef SOCKET_H_
#define SOCKET_H_

namespace utils {

/**
 * A simple network server socket implementation: the sockets opens
 * the given port and listens on 0.0.0.0:port.
 */
class Socket
{
    int  handle;
    bool closeAll;

public:
    /**
     * Create a socket and bind to 0.0.0.0:<port>.
     * Throws an exception in case of error.
     */
    Socket(int port);

    /**
     * Close the socket, even if there is still a client listening.
     */
    virtual ~Socket();

    /**
     * Waits on the socket for a new client.
     * @return -1 if the socket was closed
     *         -4 on error in accept()
     *         otherwise returns the new client socket fd.
     */
    int waitForClient();

    /**
     * Close the socket, even if there is still a client listening.
     */
    void close();
};

} /* namespace utils */

#endif /* SOCKET_H_ */
