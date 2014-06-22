/*
 * Mutexes.h
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

#ifndef MUTEXES_H_
#define MUTEXES_H_

#include <mutex>

#include "Object.h"


namespace utils {

typedef std::lock_guard<std::mutex> MutexLock;
typedef std::lock_guard<std::recursive_mutex> RecursiveLock;

/**
 * A class that just holds a lock which can be used in derived classes,
 * the lock is non-recursive.
 */
class SyncedObject : public Object
{
protected:
    mutable std::mutex myMutex;

    SyncedObject();
    virtual ~SyncedObject() noexcept;
};

/**
 * A class that just holds a lock which can be used in derived classes,
 * the lock is recursive.
 */
class RecursiveSyncedObject : public Object
{
protected:
    mutable std::recursive_mutex myMutex;

    RecursiveSyncedObject();
    virtual ~RecursiveSyncedObject() noexcept;
};


}

#endif /* MUTEXES_H_ */
