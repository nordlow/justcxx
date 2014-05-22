/*! \file timed.hpp
 * \brief Benchmark Functions on Auto-Randomized Arguments.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 * \see http://en.cppreference.com/w/cpp/thread/async
 * \see http://stackoverflow.com/questions/10059239/gccs-behaviour-with-stdasyncstdlaunchasync-vs-clangs-behaviour
 * \see http://stackoverflow.com/questions/8895350/quickcheck-like-template-function-benching-in-c
 * \see http://functionalcpp.wordpress.com/2013/08/03/generalized-function-evaluation/
 * \see http://bryanstamour.com/2012/05/13/timing-functions-with-chrono.html
 * \see https://limcore.wordpress.com/2008/04/13/8/
 * \see http://www.randombit.net/bitbashing/programming/parallel_function_invocation_using_std_async.html
 * \todo Use const boost::posix_time::time_duration & dur = boost::posix_time::seconds(1)
 */

#pragma once
#include <utility>
#include <vector>
#include <future>
#include <chrono>
#include <thread>
#include <type_traits>
#include "rand.hpp"
#include "type_traits_x.hpp"

namespace pnw
{

struct timed {
    typedef std::chrono::high_resolution_clock hrc;
    typedef std::chrono::milliseconds millis;
    typedef std::chrono::microseconds micros;
public:
    timed(size_t max_length = 1024*256,
          millis total_duration_max = millis(1000),
          millis call_duration_max = millis(0),
          uint n_tries = 1)
        : m_max_length(max_length),
          m_tot_dur(total_duration_max),
          m_call_dur(call_duration_max),
          m_n_tries(n_tries)
    {}

    /*! Benchmark Function/Lambda/Functor \p f with non-void return value.
     * \see http://bryanstamour.com/2012/05/13/timing-functions-with-chrono.html
     */
    template<class Func>
    auto call(Func&& f)
        -> typename std::enable_if<! std::is_void<decltype(f())>::value, // if non-void return
                                   std::pair<decltype(f()),
                                             typename hrc::duration> >::type {
        decltype(f()) result;
        auto tA = hrc::now();
        for (uint i = 0; i < m_n_tries; i++) { result = f(); } // call it \c m_n_tries times
        auto span = hrc::now() - tA;
        return std::make_pair(result, span);
    }

    /*! Benchmark Function/Lambda/Functor \p f with \c void return value.
     * \see http://bryanstamour.com/2012/05/13/timing-functions-with-chrono.html
     */
    template<class Func>
    auto call(Func&& f)
        -> typename std::enable_if<std::is_void<decltype(f())>::value, // if void return
                                   typename hrc::duration>::type {
        auto tA = hrc::now();
        for (uint i = 0; i < m_n_tries; i++) { f(); } // call it \c m_n_tries times
        return hrc::now() - tA; // reverse order?
    }

#if 0
    /*! Call Function \p f after \p us microseconds.
     * \see http://stackoverflow.com/questions/10804683/delayed-function-call
     */
    template<class Func, class... Args>
    inline std::future<typename std::result_of<Func(Args...)>::type>
    delayed_async(Func&& f, Args&&... args, micros us) {
        return std::async(std::launch::async, [f, us, args...] () {
                std::this_thread::sleep_for(us);
                return f(args...);            // call
            } );
    }
#endif

    /*! Benchmark the \em Single Argument Function \p f using Randomized Instances of type \p Cont.
     * \tparam C Container Type.
     * \tparam B benchS (Times) Container.
     * \param[in] f Function.
     * \return vector of pairs of \c C-instance size and execute-time.
     * \todo Estimate Time Complexity Model: O(c), O(n), ..., O(n^p), O(log(n)) O(exp(n))
     *       and predict cost of next iteration to halt within time-limit.
     * \see http://stackoverflow.com/questions/7245235/syntax-of-c-templates-with-function-type-parameters
     * \see http://en.cppreference.com/w/cpp/chrono/steady_clock
     */
    template<class C, class B = std::vector<std::pair<size_t,
                                                      double> > >
    typename std::enable_if<is_container<C>::value, B>::type
    call_s(void (*f)(C&)) {
        const auto deadline = hrc::now() + m_tot_dur;
        B bm;                    // benchmarks
        C c;                     // test data
        for (size_t n = 1; n <= m_max_length; n *= 2) {
            // efficient way of adding more random values to a vector
            auto c2 = rand_n<C>(n); c.insert(begin(c), begin(c2), end(c2)); // \see http://stackoverflow.com/a/2551785/683710
            auto tA = hrc::now(); // log start
            for (uint i = 0; i < m_n_tries; i++) { f(c); } // call it \c m_n_tries times
            auto span = hrc::now() - tA;
            if (m_call_dur.count() != 0 and
                span >= m_call_dur) { break; } // if function call took too long
            auto count = std::chrono::duration_cast<std::chrono::microseconds>(span).count();
            bm.emplace_back(n, static_cast<double>(count)/1e6); // store
            if (hrc::now() + span >= deadline) { // if no time for yet another measure taking at least \c span
                break;
            }
        }
        return bm;
    }

    /*! Benchmark the \em Single Argument Function \p f using Randomized Instances of type \p Cont.
     * \see Previous overload of \c call_s
     */
    template<class C, class B = std::vector<std::pair<size_t, double> > >
    typename std::enable_if<is_container<C>::value, B>::type
    call_s(void (*f)(const C&)) {
        return call_s(reinterpret_cast<void (*)(C&)>(f));
    }

    /*! Benchmark Function \p f using Randomized Instances of type \p Cont.
     * \tparam C Container.
     * \tparam B benchS (Times) Container.
     * \param[in] f Function.
     * \return vector of pairs of \c C-instance size and execute-time.
     */
    template<class C, class B = std::vector<std::pair<size_t,
                                                      double> > >
    typename std::enable_if<is_container<C>::value, B>::type
    call(void (*f)(typename C::iterator,
                   typename C::iterator))
    {
        const auto deadline = hrc::now() + m_tot_dur;
        B bm;                    // benchmarks
        C c;                     // test data
        for (size_t n = 1; n <= m_max_length; n *= 2) {
            // efficient way of adding more random values to a vector
            auto c2 = rand_n<C>(n); c.insert(begin(c), begin(c2), end(c2)); // \see http://stackoverflow.com/a/2551785/683710
            auto tA = hrc::now(); // log start
            for (uint i = 0; i < m_n_tries; i++) { f(c.begin(), c.end()); } // call it \c m_n_tries times
            auto span = hrc::now() - tA;
            if (m_call_dur.count() != 0 and
                span >= m_call_dur) { break; } // if function call took too long
            auto count = std::chrono::duration_cast<std::chrono::microseconds>(span).count();
            bm.emplace_back(n, static_cast<double>(count)/1e6); // store
            if (hrc::now() + span >= deadline) { // if there's no time for more measures
                break;
            }
        }
        return bm;
    }

public:
    size_t m_max_length = 1024*256; ///< Maximum container instance length of \c T.
    millis m_tot_dur = millis(1000); ///< Maximum time to wait for whole benchmarking to complete.
    millis m_call_dur = millis(0); ///< Maximum time to wait for a call \p f to complete.
    uint m_n_tries = 1;    ///< Number of tries per size of randomizeda instance.
    std::launch m_launch_policy = std::launch::async | std::launch::deferred;    ///< Thread launch policy.
};

}
