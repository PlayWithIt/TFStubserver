/*
 * TimerCommand.cpp
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

#include "TimerCommand.h"

namespace utils {

TimerCommand::TimerCommand(const char *_name, unsigned _delayInMs, int calls)
    : name(_name)
    , currentMs(0)
    , delayInMs(_delayInMs)
    , remainingCalls(calls)
{
}

TimerCommand::~TimerCommand() {
}

/**
 * Check if the main function should be called.
 */
bool TimerCommand::callOnCycle(unsigned int cycleCount, unsigned int delaySinceLastCall,
                               const std::chrono::system_clock::time_point &now) NOEXCEPT
{
    // count reached => no more execution
    if (remainingCalls == 0)
        return false;

    currentMs += delaySinceLastCall;
    //printf("%s::callOnCycle() check ms %u against delay of %u, (last call %u)\n",
    //       name.c_str(), currentMs, delayInMs, delaySinceLastCall);

    if (currentMs >= delayInMs)
    {
        currentMs -= delayInMs;

        // count down
        if (remainingCalls > 0)
            --remainingCalls;
        return true;
    }
    return false;
}

/**
 * Called if the command is put into a queue.
 */
void TimerCommand::onAdd() {

}

/**
 * Called if the command is removed from the queue.
 */
void TimerCommand::onRemove() {

}

/**
 * Default impl: return true.
 */
bool TimerCommand::destroyAfterRemove() const {
    return true;
}

} /* namespace utils */
