/*
 * BlockingQueueTasks.h
 *
 * Copyright (C) 2014 Holger Grosenick
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
#ifndef BLOCKINGQUEUETASKS_H_
#define BLOCKINGQUEUETASKS_H_

#include "AsyncTask.h"
#include "BlockingQueue.h"
#include "Log.h"

namespace utils {


/**
 * This template implements thread which holds a data-queue that can be consumed by one or
 * more {@link QueueConsumerTask}s. If the producing thread is stopped, it automatically
 * stops the consuming threads by "closing" the queue.
 */
template <typename _Tp> class QueueProducerTask : public AsyncTask
{
public:
    typedef _Tp value_type;

protected:
    utils::BlockingQueue<value_type> queue;

public:
    /**
     * Init with a thread name.
     */
    QueueProducerTask(const char *taskName) : AsyncTask(taskName), queue()
    { }

    /**
     * Stop the thread: first close the queue and then announce to finish.
     * It is quite important to release the waiting clients here first!
     */
    virtual bool signalToStop() noexcept
    {
        queue.closeQueue();
        return utils::AsyncTask::signalToStop();
    }
};


/**
 * This template implements a thread that consumes a data queue (FIFO). The queue is passed
 * in as argument and is filled by another thread, e.g. {@link QueueProducerTask}.
 * <p>
 * Derived classes need to define a type for '_Tp' and implement the method {@link consume()}.
 * This method makes the main work for each data packet.
 * <p>
 * Multiple instances of this class might be listing on one queue.
 */
template <typename _Tp> class QueueConsumerTask : public AsyncTask
{
public:
    typedef _Tp value_type;

private:
    utils::BlockingQueue<value_type> &queue;

    /**
     * Main loop that consumes the data.
     */
    virtual void run()
    {
        value_type data;
        while (true)
        {
            if (!queue.waitFor(data))
            {
                if (shouldFinish() || queue.closed()) {
                    // wakeUp without data -> terminate
                    Log::log(getTaskName() + " done");
                    return;
                }

                // see condition_variable api: the variable might be triggered without condition
                Log::log(getTaskName() + " wakeup without data??");
                continue;
            }

            // let the implementation do its work
            if (!consume(data))
                return;
        }
    }

protected:
    /**
     * Must be implemented by the derived class: do something with the data.
     * @return true: OK continue, false: <b>terminate the thread</b> (be careful here ...)
     */
    virtual bool consume(value_type &data) = 0;

public:
    /**
     * Init with a thread name and the queue that should be processed.
     */
    QueueConsumerTask(const char *taskName, utils::BlockingQueue<value_type> &_queue) : AsyncTask(taskName), queue(_queue)
    { }
};

} // namespace

#endif /* BLOCKINGQUEUETASKS_H_ */
