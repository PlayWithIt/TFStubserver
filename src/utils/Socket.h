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
    int      handle;
    unsigned timeoutRead;
    unsigned timeoutWrite;
    bool     closeAll;

public:
    /**
     * Create a socket and bind to 0.0.0.0:&lt;port&gt;.
     * Throws an exception in case of error.
     * @param port the port number to use
     * @param reuse the socket SO_REUSEADDR option ?
     */
    Socket(int port, bool reuse = true);

    /**
     * Close the socket, even if there is still a client listening.
     */
    virtual ~Socket();

    /**
     * Set a read-timeout in milliseconds on all new client connections which are created by
     * {@link waitForClient()}. If the value is 0, no timeout will be set and <code>recv()</code>
     * is a blocking call.
     * An exception is thrown if this operation fails.
     */
    void setReadTimeout(unsigned ms) {
        timeoutRead = ms;
    }

    /**
     * Set a write-timeout in milliseconds on all new client connections which are created by
     * {@link waitForClient()}. If the value is 0, no timeout will be set and <code>send()</code>
     * is a blocking call.
     * An exception is thrown if this operation fails.
     */
    void setWriteTimeout(unsigned ms) {
        timeoutWrite = ms;
    }

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
