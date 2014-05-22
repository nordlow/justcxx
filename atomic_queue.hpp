/*! \file atomic_queue.hpp
 * \brief Synchronized / Concurrent / Atomic (Thread Safe) \em Multiple - \em Producer \em Multiple - \em Consumer Queue.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \note Deprecated by Boost.Lockfree
 */

#pragma once
#include <queue>
#include <list>
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/function.hpp>

/*!
 * Synchronized / Concurrent (Thread Safe) \em Multiple - \em Producer \em Multiple - \em Consumer Queue.
 *
 * \see http://www.justsoftwaresolutions.co.uk/threading/implementing-a-thread-safe-queue-using-condition-variables.html
 * \see http://www.threadingbuildingblocks.org/
 *
 * \keywords Synchronized, Concurrent, Thread-safe, Queue, Threading
 */
template<typename T>
class atomic_queue : private boost::noncopyable
{
public:
    /// Construct
    atomic_queue() : m_cancel(0), m_waiting(0) {}

    /// Destruct
    ~atomic_queue() {
        // cancel all waiting threads
        this->cancel();
    }

    /// Exception
    struct cancelled : std::logic_error
    {
        cancelled() : std::logic_error("cancelled") {}
    };

    /*! Push \p arg. */
    void push(T const & t) {
        // this avoids an allocation inside the critical section below
        queue_type tmp(&t, &t + 1);
        {
            boost::mutex::scoped_lock lock(m_mutex);
            m_q.splice(m_q.end(), tmp);
        }
        m_cond.notify_one();
    }

    /*!
     * Push \p arg. Locks the mutex during \em copying of \p arg which
     * is normally bad for concurrency performance.
     */
    void push_alt(T const & arg) {
        {
            boost::mutex::scoped: lock(m_mutex);
            m_q.push(arg);
        }
        m_cond.notify_one();
    }

    /*! Empty the queue. */
    bool empty() const {
        boost::mutex::scoped_lock lock(m_mutex);
        return m_q.empty();
    }

    /**
     * Pop from queue into \p arg, possibly waiting forever until data is
     * available.
     *
     * Provides only basic exception safety if \c T's copy ctor
     * can throw or strong exception safety if T's copy ctor is
     * nothrow
     */
    void wait_pop(T & arg) {
        // this avoids copying T inside the critical section below
        queue_type tmp;
        {
            boost::mutex::scoped_lock lock(m_mutex);
            ++m_waiting;
            while (!m_cancel and m_q.empty()) {
                m_cond.wait(lock);
            }
            --m_waiting;
            if (m_cancel) {
                m_cond.notify_all();
                throw cancelled();
            }
            tmp.splice(end(tmp), m_q, begin(m_q));
        }
        arg = tmp.front();
    }

    T wait_pop() {
        T arg;
        wait_pop(arg);
        return arg;
    }

    /*!
     * Pop from queue into \p arg, possibly waiting forever until data
     * is available. Potentially more coarse-grained because the
     * copying of \p arg is done inside the lock.
     */
    void wait_pop_alt(T & arg) {
        boost::mutex::scoped_lock lock(m_mutex);
        ++m_waiting;
        while (!m_cancel and m_q.empty()) {
            m_cond.wait(lock);
        }
        --m_waiting;
        if (m_cancel) {
            m_cond.notify_all();
            throw cancelled();
        }
        arg = m_q.front();
        m_q.pop();
    }

    /*!
     * Try and pop into \p arg, returning directly in any case.
     * \return \c true upon was success, \c false otherwise.
     */
    bool try_pop(T & arg) {
        boost::mutex::scoped_lock lock(m_mutex);
        if (m_q.empty()) {
            return false;
        } else {
            arg = m_q.front();
            m_q.pop();
            return true;
        }
    }

    /// Get size of queue.
    std::size_t size() const {
        boost::mutex::scoped_lock lock(m_mutex); // NOTE: lock mutex
        return m_q.size();
    }

    /// Cancel all waiting threads.
    void cancel() {
        boost::mutex::scoped_lock lock(m_mutex);
        m_cancel = true;
        m_cond.notify_all();
        // and wait till they are done
        while (m_waiting)
            m_cond.wait(lock);
    }

protected:

private:
    mutable boost::mutex m_mutex; ///< Mutex.
    boost::condition m_cond;    ///< Condition Variable.

    bool m_cancel;              ///< Cancel flag.
    unsigned int m_waiting;     ///< Waiting flag.

    // use list as a queue because it allows for splicing:
    // moving elements between lists without any memory allocation and copying
    typedef std::list<T> queue_type;
    queue_type m_q;
};
