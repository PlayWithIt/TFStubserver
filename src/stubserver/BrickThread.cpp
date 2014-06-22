/*
 * BrickThread.cpp
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

#include "BrickThread.h"

#include <utils/Log.h>

namespace stubserver {

using utils::Log;


BrickThread::BrickThread(const char *filename)
  : AsyncTask("BrickThread"), BrickStack(filename)
{
}

/**
 * Quite simple what the thread does ...
 */
void BrickThread::run()
{
    Log::log("BrickThread is running ...");

    auto now = std::chrono::system_clock::now();
    std::chrono::milliseconds waitTime(1);

    do {
        incrementTime(now);
        consumeRequestQueue();
        checkCallbacks();

        // sleep until the next millisecond is reached
        now += waitTime;
    } while (!shouldFinish(now));

    Log::log("BrickThread has stopped ...");
}

} /* namespace stubserver */
