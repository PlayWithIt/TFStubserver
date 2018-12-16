/*
 * AsyncTask.h
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

#ifndef ASYNCTASK_H_
#define ASYNCTASK_H_

#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include <condition_variable>

#include "Mutexes.h"


namespace utils {

/**
 * The AsyncTask is an object that can start a separate thread and
 * execute some functionality in a well-defined way.
 * <P>
 * Derived classes must implement the {@link run()} method and put
 * the main functionality in it. If {@link run()} creates a 'never-ending'
 * loop, it should check {@link shouldFinish()} from time to time in order
 * to terminate the loop if {@link shouldFinish()} returns true.
 * <P>
 * An external user must explicitly call {@link start()} in order to start
 * the separate thread. The constructor only initializes things, but
 * does not yet start a thread.
 * <P>
 * It is also possible to wait on the eventCondition variable: an external
 * data provider can call {@link wakeUp()} in order to notify this thread
 * that new data has arrived.
 */
class AsyncTask : public Object
{
    std::string taskName;
    std::thread *th;
    std::atomic_bool active;    // thread is running
    std::atomic_bool finish;    // thread loop should finish

    std::condition_variable eventCondition;

    void callRunMethod();        // thread init

    // copy a thread is dangerous ...
    AsyncTask(const AsyncTask&) = delete;
    AsyncTask& operator=(const AsyncTask&) = delete;

protected:
    mutable std::mutex taskMutex;

    /**
     * Must be implemented in derived classes, a typical implementation
     * looks like this (if it periodically consumes data):
     * <pre>
     * do {
     *    doSomething();
     * } while (!shouldFinish(1000));
     * </pre>
     */
    virtual void run() = 0;

    /**
     * Change the active flag: a client can set this to true just before it
     * leaves the run() method and nothing else is necessary to clean up.
     * Otherwise this flag is automatically set to false if the run()
     * method terminates.
     */
    void setActive(bool a);

    /**
     * Wait until the 'eventCondition' variable gets notified via {@link wakeUp()}.
     * If the thread should not wait endlessly, use {@link shouldFinish(unsigned)}.
     */
    void waitForEvent();

    /**
     * Waits max 'waitUs' microseconds for the finish flag to be changed to 'true'.
     * If the flag is not set to true, the function returns false.
     * @param waitUs number of microseconds to wait for the thread to finish normally
     */
    bool shouldFinish(unsigned waitUs);

    /**
     * Waits until the given time is reached or the condition_variable was signaled.
     * @param absTime to wait until
     */
    bool shouldFinish(const std::chrono::system_clock::time_point &absTime);

public:
    AsyncTask(const std::string &name);
    AsyncTask(const char *name);
    virtual ~AsyncTask();

    /**
     * the {@link run()} method is still active?
     * The access is atomic, concurrent.
     */
    bool isActive() const {
        return active;
    }

    /**
     * Returns the logical task name of this thread.
     */
    const std::string& getTaskName() const {
        return taskName;
    }

    /**
     * Signal the thread which currently calls 'shouldFinish()' or waits for the condition
     * variable to get notified.
     */
    virtual void wakeUp();

    /**
     * Just return the value of the "finish" flag without waiting.
     */
    bool shouldFinish() const {
        return finish;
    }

    /**
     * Sets the "finish" flag to true and waits some time so that
     * the {@link run()} method has time to finish. The called should
     * check {@link isActive()} before calling this method.
     *
     * @return true if the thread ended in time, false if the thread is
     *        not running any more or didn't end.
     */
    virtual bool stop(int waitTimeMs = 0) noexcept;

    /**
     * Just sets the "finish" flag to true and returns. The running thread
     * might check this from time to time and then terminate, but it is not
     * guaranteed that the thread finishes within a certain amount of time.
     * But derived threads might use this.
     *
     * @return false if the thread is not running any more, true otherwise.
     */
    virtual bool signalToStop() noexcept;

    /**
     * Sets active to true, starts {@link run()} in a separate thread and returns.
     * Returns false if the task is still active from a previous call to {@link start()}.
     */
    bool start();
};

} /* namespace utils */
#endif /* ASYNCTASK_H_ */
