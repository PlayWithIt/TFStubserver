/*
 * TimerCommand.h
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

#ifndef TIMERCOMMAND_H_
#define TIMERCOMMAND_H_

#include <chrono>
#include <string>
#include "Compatability.h"

namespace utils {

/**
 * Used in combination with the {@link TimerThread} which calls the commands with a
 * specific delay and optionally only a given number of times.
 * <P>
 * By default an object instance of this class is destroyed when the command should
 * not be executed any more.
 */
class TimerCommand
{
    std::string name;

protected:
    unsigned currentMs;
    unsigned delayInMs;
    int remainingCalls;

public:
    /**
     * Create a command that is called in cycles of 'delayInMs', if the 'calls'
     * value is larger than 0, the command is called only this number of times.
     * If the 'calls' value if below 0, the command is executed endlessly.
     */
    TimerCommand(const char *name, unsigned delayInMs, int calls = -1);
    virtual ~TimerCommand();

    /**
     * Called with in each cycle and if this returns true, the 'execute' method
     * is called (only in this case).
     */
    virtual bool callOnCycle(unsigned int cycleCount, unsigned int delaySinceLastCall,
                             const std::chrono::system_clock::time_point &now) NOEXCEPT;

    /**
     * This method is called by the controlling thread if 'callOnCycle' returns true.
     */
    virtual void execute(const std::chrono::system_clock::time_point &now) = 0;

    /**
     * Return the cycle time in ms in which this command will do something
     */
    int getDelayInMs() const {
        return delayInMs;
    }

    /**
     * If this return 0, the timer thread will remove and destroy this object.
     */
    int getRemainingCalls() const {
        return remainingCalls;
    }

    /**
     * Name as additional information
     */
    const std::string& getName() const {
        return name;
    }

    /**
     * Has the given command this specific name?
     */
    bool hasName(const char *s) const {
        return name.compare(s) == 0;
    }

    /**
     * Called after the command is put into a queue.
     */
    virtual void onAdd();

    /**
     * Called before the command is removed from the queue and just before it
     * is normally destroyed (deleted).
     * @see #destroyAfterRemove()
     */
    virtual void onRemove();

    /**
     * Destroy (delete) this object, after {@link onRemove()} was called? If the command
     * must live longer, it should implement this method and return 'false' here.
     *
     * @return the default implementation returns true.
     * @see #onRemove()
     */
    virtual bool destroyAfterRemove() const;
};

} /* namespace utils */
#endif /* TIMERCOMMAND_H_ */
