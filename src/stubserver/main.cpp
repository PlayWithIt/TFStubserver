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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "stubserver.h"

/**
 * Main control.
 */
int main(int argc, char *argv[])
{
    int port               = 4223;
    const char *deviceList = NULL;
    const char *logName    = NULL;
    bool logRequests       = false;

    for (int i = 1; i < argc; ++i)
    {
        const char *arg = argv[i];
        if (strcmp(arg, "-p") == 0 && i+1 < argc)
        {
            port = atol(argv[++i]);
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
            logName = argv[++i];
            continue;
        }

        if (strcmp(arg, "-?") != 0)
            printf("\nUnknown option '%s' !\n", arg);
        printf("\nUsage: stubserver {-p port} {-l logName} {-rq} -d devices.properties\n"
               "\nOptions:"
               "\n-p port     change the port which is 4223 by default"
               "\n-l logName  write log into this file instead of stdout"
               "\n-rq         log request data"
               "\n-d devs     properties config file with the TF devices to simulate"
               "\n");
        return 1;
    }

    int rc = stubserver::runServer(deviceList, port, logName, logRequests);
    return rc;
}
