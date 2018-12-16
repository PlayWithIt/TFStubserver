/*
 * BlockingQueue.h
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
#ifndef BLOCKINGQUEUE_H_
#define BLOCKINGQUEUE_H_

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <stdexcept>

#include "Compatability.h"

namespace utils {

/**
 * Thrown in cases where queue items are accessed, but the queue is empty.
 */
class EmptyQueueException : public std::exception
{
    std::string msg;

public:
    EmptyQueueException() : msg("Queue is empty, call to front() or pop() is invalid!") { }
    explicit EmptyQueueException(const std::string &s) : msg(s) { }
    explicit EmptyQueueException(const char *s) : msg(s) { }

    virtual const char* what() const noexcept override {
        return msg.c_str();
    }
};


/**
 * This class implements a simple BlockingQueue to use with a producer and consumer threads.
 * The access to the queue is synchronized. Producers should use {@link enqueue()} to add
 * new items (which are copied into the queue) and consumers can use {@link waitFor()}
 * to retrieve items or just check the length.
 * <P>
 * It depends on the number of consuming threads which way is the simplest one to retrieve
 * elements.
 * <P>
 * When there will be no more new items in the queue (program is about to finish), the
 * producer should call {@link closeQueue()} or {@link closeImmediate()} in order to
 * unblocking waiting client threads.
 */
template <typename _Tp> class BlockingQueue
{
    typedef _Tp value_type;

    // data
    std::deque<value_type> items;
    std::atomic_uint       size;
    std::atomic_bool       closing;

    // for synchronization
    std::mutex              queueMutex;
    std::condition_variable queueEvent;

    /**
     * Remove the front element (not synchronized).
     */
    void getAndRemoveFront(value_type &result) {
        result = items.front();
        items.pop_front();
        --size;
    }

public:
    BlockingQueue() : size(0), closing() {};
    virtual ~BlockingQueue() {
        closeImmediate();
    }

    /**
     * Returns true if the queue is empty.
     */
    bool empty() const {
        return size == 0;
    }

    /**
     * Returns true if the queue is in closing state, no new items will be added.
     */
    bool closed() const {
        return closing;
    }

    /**
     * Returns true if the client should continue to call any of the {@link #waitFor()} methods.
     * This is the case as long as there are items in the queue or the queue is not closed.
     */
    bool consumeable() const {
        return size != 0 || !closing;
    }

    /**
     * Returns the number of elements in the queue.
     */
    unsigned length() const {
        return size;
    }

    /**
     * Put an element into the queue and notify threads which are waiting for elements.
     * If the queue is already in closing state: ignore this call!
     * @return true if the item was added, false if the queue is shutdown
     */
    bool enqueue(const value_type& item) {
        if (closing)
            return false;
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            items.push_back(item);
            ++size;
        }
        queueEvent.notify_one();
        return true;
    }

    /**
     * Announce that the queue is about to be closed. All waiting clients
     * should wake up, consume the rest of the queue an then terminate themselves.
     */
    void closeQueue()
    {
        if (closing)
            return;

        unsigned s = size;
        unsigned loops = s + 1000;
        while (s > 0)
        {
            {
                // get a lock to wait until all items are consumed and then notify to wake up
                std::unique_lock<std::mutex> lock(queueMutex);
                queueEvent.notify_one();
            }
            s = size;
            if (s <= 1)
                closing = true;

            // TODO: don't wait forever, this might be done via calculating max time for consuming
            if (--loops == 0)
                break;
        }
        closing = true;
        queueEvent.notify_all();
    }

    /**
     * Remove all existing items from the queue (even if not processed) and then
     * give a single signal to all waiting clients which then should terminate.
     */
    void closeImmediate()
    {
        if (closing)
            return;

        {
            std::lock_guard<std::mutex> lock(queueMutex);
            closing = true;
            size = 0;
            items.clear();
        }
        queueEvent.notify_all();
    }

    /**
     * Read the first element from the queue, but don't remove it from the queue
     * @throws EmptyQueueException if the queue is empty
     */
    value_type& front() {
        std::lock_guard<std::mutex> lock(queueMutex);
        if (size == 0)
            throw EmptyQueueException();
        return items.front();
    }

    /**
     * Remove the first element from the queue.
     * @throws EmptyQueueException if the queue is empty
     */
    void pop() {
        std::lock_guard<std::mutex> lock(queueMutex);
        if (size == 0)
            throw EmptyQueueException();
        items.pop_front();
        --size;
    }

    /**
     * Waits until an item is in the queue and the returns the first element and removes
     * the first elements from the queue.
     * <p>
     * If the queue already has an items, if the method is called, the method immediately
     * returns the item. If there is no element in the queue, the method waits for a
     * 'new item' event.
     * <p>
     * If the method receives a 'new item' event, but the queue is empty, the queue is
     * about to be closed (and then destroyed) which means that consuming clients should
     * also terminate. <b>In this case the method return false</b>.
     *
     * @param result - the first element of the queue will be copied into this result
     * @return true if a item was retrieved, false if not (-> queue closed)
     */
    bool waitFor(value_type& result)
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        if (size > 0) {
            getAndRemoveFront(result);
            return true;
        }
        if (closing)
            return false;

        // nothing in -> wait
        queueEvent.wait(lock);
        if (size == 0) {
            return false;
        }
        getAndRemoveFront(result);
        return true;
    }

    /**
     * Waits until an item is in the queue or until the given amount of time expires. If an
     * item is in the queue, the method returns the first element and removes it from the queue.
     * <p>
     * If the queue already has an items, if the method is called, the method immediately
     * returns the item. If there is no element in the queue, the method waits for a
     * 'new item' event.
     * <p>
     * If the method receives a 'new item' event, but the queue is empty, the queue is
     * about to be closed (and then destroyed) which means that consuming clients should
     * also terminate. <b>In this case the method return false</b>.
     *
     * @param result - the first element of the queue will be copied into this result
     * @param rel_time - maximum time to wait until this method returns. If this time has
     *              passed and no item is in the queue, false is returned
     * @return true if a item was retrieved, false if not (-> queue closed or timeout)
     */
    bool waitFor(value_type& result, const std::chrono::milliseconds& rel_time)
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        if (size > 0) {
            getAndRemoveFront(result);
            return true;
        }
        if (closing)
            return false;

        // nothing in -> wait
        if (queueEvent.wait_for(lock, rel_time) == std::cv_status::timeout)
            return false;
        if (size == 0) {
            return false;
        }
        getAndRemoveFront(result);
        return true;
    }
};

} /* namespace utils */

#endif /* BLOCKINGQUEUE_H_ */
