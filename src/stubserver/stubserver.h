/*
 * stubserver.h
 *
 * Copyright (C) 2015 Holger Grosenick
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

#ifndef STUBSERVER_STUBSERVER_H_
#define STUBSERVER_STUBSERVER_H_

#include <list>


namespace stubserver {

class SimulatedDevice;

/**
 * Retrieve the list of configured devices, this can be empty if
 * the server is not running.
 */
std::list<const SimulatedDevice*> getDevices();

/**
 * Start the stubserver with the given configuration which points to the
 * device properties. This function return when the server gets stopped.
 */
int runServer(const char *deviceList, int port = 4223, const char *logName = NULL, bool logRequests = false);

/**
 * Try to stop the server by closing the socket.
 *
 * @return if the server was active and the socket was closed, false if not
 */
bool stopServer();


}  // namespace

#endif /* STUBSERVER_STUBSERVER_H_ */
