/*
 * AsyncTask.cpp
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

#include "Log.h"
#include "utils.h"
#include "AsyncTask.h"


using utils::MutexLock;


namespace utils {

/**
 * Defaul init.
 */
AsyncTask::AsyncTask(const char* name)
   : taskName(name)
   , th(NULL)
   , active(false)
   , finish(false)
   , eventCondition()
   , myMutex()
{
}

AsyncTask::~AsyncTask() {
    stop();
}

/**
 * Start main processing.
 */
void AsyncTask::callRunMethod()
{
    run();
    setActive(false);
}

/**
 * Change the active flag.
 */
void AsyncTask::setActive(bool a)
{
    active = a;
}

/**
 * Return the value of the finish flag.
 */
bool AsyncTask::shouldFinish()
{
    return finish;
}

/**
 * Return the value of the finish flag.
 */
bool AsyncTask::shouldFinish(unsigned waitUs)
{
    std::unique_lock<std::mutex> l(myMutex);
    eventCondition.wait_for(l, std::chrono::microseconds(waitUs));
    return finish;
}

/**
 * Return the value of the finish flag.
 */
bool AsyncTask::shouldFinish(const std::chrono::system_clock::time_point &absTime)
{
    std::unique_lock<std::mutex> l(myMutex);
    eventCondition.wait_until(l, absTime);
    return finish;
}

/**
 * Start the thread and call the run method in the thread.
 */
bool AsyncTask::start()
{
    if (active || th)
        return false;

    MutexLock l(myMutex);
    finish = false;
    active = true;
    th = new std::thread(&AsyncTask::callRunMethod, this);
    return true;
}

bool AsyncTask::signalToStop() noexcept
{
    MutexLock l(myMutex);
    if (!th)
        return false;
    finish = true;
    if (active)
        eventCondition.notify_all();
    return true;
}

/**
 * Stop the thread loop.
 */
bool AsyncTask::stop(int wait) noexcept
{
    if (!signalToStop())
        return false;

    int ms = wait <= 0 ? 40 : wait;
    while (isActive() && ms >= 0) {
        utils::msleep(10);
        ms -= 10;
    }
    if (isActive())
        utils::Log::log(std::string("WARNING: async-task '") + taskName + "' didn't end in time!");
    th->join();
    delete th;
    th = NULL;
    setActive(false);
    finish = false;
    return (ms >= 0);
}


/**
 * Signal the thread which currently calls 'shouldFinish()'.
 */
void AsyncTask::wakeUp()
{
    if (active) {
        MutexLock l(myMutex);
        eventCondition.notify_all();
    }
}

} /* namespace utils */
