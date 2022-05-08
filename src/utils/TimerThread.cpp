/*
 * TimerThread.cpp
 *
 * Copyright (C) 2013-2021 Holger Grosenick
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
#include <iomanip>
#include <chrono>

#include "Log.h"
#include "utils.h"
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
TimerThread::~TimerThread() noexcept
{
    TimerThread::stop(2*delay+100);

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

        if (c->getRemainingCalls() * (c->getDelayInMs() / 1000) > 300) {
            // Log if a timer command with a duration longer than 5 minutes / 300 seconds is started
            Log() << "Started the timer '" << c->getName() << "' with duration " << (c->getRemainingCalls() * (c->getDelayInMs() / 1000));
        }
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
TimerCommand* TimerThread::getCommand(const char *name) const noexcept
{
    MutexLock guard(taskMutex);

    for (auto it = allCmds.cbegin(); it != allCmds.cend(); ++it)
    {
        TimerCommand* c = *it;
        if (c->hasName(name)) {
            return c;
        }
    }
    return nullptr;
}
/**
 * Returns true if the command list is empty.
 */
bool TimerThread::idle() const
{
    MutexLock guard(taskMutex);
    return allCmds.empty();
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
//                std::cout << "execute " <<  c->getName() << " at " << serializeTimePoint(absTime) <<
//                             " vs. " << serializeTimePoint(std::chrono::system_clock::now()) << '\n';
                c->execute(absTime);
            }
            catch (const std::exception &e) {
                Log::error(c->getName(), e);
            }

            // throw away commands that are not used any more
            if (c->getRemainingCalls() == 0) {
//                std::cout << "Callcount == 0 -> remove a command from queue\n";
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

/**
 * Helper to print a timepoint into a string, optional with milliseconds if the format is NOT given.
 * Without format, a special internal trace format is used: Y-M-D H:M:S.ms
 */
std::string TimerThread::serializeTimePoint( const std::chrono::system_clock::time_point &time, const std::string *format)
{
    std::time_t tt = std::chrono::system_clock::to_time_t(time);
    std::tm tm = *std::localtime(&tt); //Locale time-zone, usually UTC by default.
    std::stringstream ss;
    ss << std::put_time( &tm, format ? format->c_str() : "%Y-%m-%d %H:%M:%S" );
    if (!format) {
        // add milliseconds
        auto value = std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch());
        long ms = value.count() % 1000;
        ss << '.' << ms;
    }
    return ss.str();
}

} /* namespace utils */
