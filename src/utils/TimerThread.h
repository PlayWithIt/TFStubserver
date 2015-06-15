/*
 * TimerThread.h
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

#ifndef TIMERTHREAD_H_
#define TIMERTHREAD_H_

#include <list>

#include "AsyncTask.h"
#include "TimerCommand.h"

namespace utils {

/**
 * Async Thread that can execute external functions periodically up to 10 times per second.
 * <p>
 * The external functions must implement the {@link TimerCommand} interface. The lifecycle
 * of a command is<br>
 * - client creates command and adds it to the timer<br>
 * - timer calls {@link TimerCommand.onAdd()}<br>
 * - timer checks when the command must be executed, calls {@link TimerCommand.execute()}<br>
 * - timer removes the command, calls {@link TimerCommand.onRemove()}<br>
 * - if {@link TimerCommand.destroyAfterRemove()} returns true, the command-object will get deleted
 */
class TimerThread : public AsyncTask
{
    unsigned delay;          // loop delay in milliseconds
    unsigned cycle;
    std::list<TimerCommand*> allCmds;
    std::list<TimerCommand*> newCmds;
    std::mutex newCmdsMutex;

    /**
     * Main thread loop (internally used).
     */
    virtual void run();

public:
    /**
     * Init with timer cycle 100ms.
     */
    TimerThread();

    /**
     * Init with user defined timer cycle in milliseconds.
     */
    explicit TimerThread(unsigned delayMs);

    // stop the thread loop and releases resources
    virtual ~TimerThread() NOEXCEPT;

    /**
     * Add another command to the list of commands. This command is used
     * within the loop afterwards and 'callOnCycle()' is called 4 times
     * per second: it this returns true, 'execute' is called.
     */
    void addCommand(TimerCommand *c);

    /**
     * Removes the given commands from the queue if it still exists.
     * Returns true if the command was found and stopped.
     */
    bool stopCommand(TimerCommand *c);

    /**
     * Removes the commands with the given name from the queue if it still exists.
     * Returns true if the command was found and stopped.
     */
    bool stopCommand(const char *name);

    /**
     * Return the first command of the queue with the given name or NULL.
     */
    TimerCommand* getCommand(const char *name) const NOEXCEPT;
};

} /* namespace utils */
#endif /* TIMERTHREAD_H_ */
