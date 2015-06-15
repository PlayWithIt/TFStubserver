/*
 * TimerThread.cpp
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

#include <stdexcept>
#include <utils/Log.h>
#include <utils/utils.h>
#include "TimerThread.h"

using utils::Log;
using utils::MutexLock;


namespace utils {

/**
 * Default init all members.
 */
TimerThread::TimerThread()
   : AsyncTask("TimerThead")
   , delay(100)
   , cycle(0)
{
}

/**
 * Init with customer delay.
 */
TimerThread::TimerThread(unsigned delayMs)
   : AsyncTask("TimerThead")
   , delay(delayMs)
   , cycle(0)
{
    if (delayMs < 2)
        throw std::invalid_argument("Delay must be at least 2ms!");
}

/**
 * Clean up timer.
 */
TimerThread::~TimerThread() NOEXCEPT
{
    stop(2*delay+100);

    // free the remaining commands
    taskMutex.lock();
    for (std::list<TimerCommand*>::iterator it = allCmds.begin(); it != allCmds.end(); ++it)
    {
        TimerCommand *c = *it;
        if (c->destroyAfterRemove())
            delete c;
    }
    taskMutex.unlock();
}

/**
 * Add another command to the list of command. If this is called out of
 * and execute() method a lock might be active: use other list in this case.
 */
void TimerThread::addCommand(TimerCommand *c)
{
    if (taskMutex.try_lock())
    {
        allCmds.push_front(c);
        c->onAdd();
        taskMutex.unlock();
    }
    else {
        // put into a temp list.
        MutexLock guard(newCmdsMutex);
        newCmds.push_front(c);
    }
}

/**
 * Return the first command of the queue with the given name or NULL.
 */
TimerCommand* TimerThread::getCommand(const char *name) const NOEXCEPT
{
    MutexLock guard(taskMutex);

    for (auto it = allCmds.cbegin(); it != allCmds.cend(); ++it)
    {
        TimerCommand* c = *it;
        if (c->hasName(name)) {
            return c;
        }
    }
    return NULL;
}

/**
 * Main thread loop.
 */
void TimerThread::run()
{
    std::list<TimerCommand*> copyCmds;

    // to handle the time
    std::chrono::system_clock::time_point absTime = std::chrono::system_clock::now();
    std::chrono::milliseconds waitTime(delay);

    // first tiny wait
    absTime += waitTime;

    // wait until the absolute time value is reached (this is quite precise)
    while (!shouldFinish(absTime))
    {
        // count loops
        if (cycle > 2000000000)
            cycle = 1;
        else
            ++cycle;

        // make copies in order to always have only one mutex locked at a time.
        if (!newCmds.empty())
        {
            {
                MutexLock guard(newCmdsMutex);
                copyCmds = std::move(newCmds);
            }
            {
                for (auto it : copyCmds) {
                    // remove existing command: must be unique
                    stopCommand(it->getName().c_str());

                    // now add
                    {
                        MutexLock guard(taskMutex);
                        allCmds.push_front(it);
                    }
                    it->onAdd();
                }
                copyCmds.clear();
            }
        }

        // nothing to do
        if (allCmds.empty()) {
            absTime += waitTime;
            continue;
        }

        // repeat those commands which are still in the list of repeating commands
        taskMutex.lock();
        for (std::list<TimerCommand*>::iterator it = allCmds.begin(); it != allCmds.end(); )
        {
            TimerCommand *c = *it;
            if (!c->callOnCycle(cycle, delay, absTime)) {
                ++it;
                continue;
            }

            try {
                c->execute(absTime);
            }
            catch (const std::exception &e) {
                Log::error(c->getName(), e);
            }

            // throw away commands that are not used any more
            if (c->getRemainingCalls() == 0) {
                // printf("Callcount == 0 -> remove a command from queue\n");
                c->onRemove();
                if (c->destroyAfterRemove())
                    delete c;
                it = allCmds.erase(it);
            }
            else {
                ++it;
            }
        }
        taskMutex.unlock();

        absTime += waitTime;
    }
}

/**
 * Removes a specific command from the queue if it is still in.
 * Returns true if the command was found and stopped.
 */
bool TimerThread::stopCommand(TimerCommand *toRemove)
{
    if (!toRemove)
        return false;

    MutexLock guard(taskMutex);

    for (auto it = allCmds.begin(); it != allCmds.end(); ++it)
    {
        TimerCommand* c = *it;
        if (c == toRemove) {
            c->onRemove();
            if (c->destroyAfterRemove())
                delete c;
            allCmds.erase(it);
            return true;
        }
    }
    return false;
}


/**
 * Removes the given command from the queue if it exists.
 */
bool TimerThread::stopCommand(const char *name)
{
    if (!name)
        return false;
    return stopCommand(getCommand(name));
}

} /* namespace utils */
