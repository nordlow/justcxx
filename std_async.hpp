/*! \file std_async.hpp
 * \date 2009-11-27 12:34
 */

#pragma once
#include <future>
#include <thread>

template<typename F>
auto std_async(F f) -> std::unique_future<decltype(f())>
{
    typedef decltype(f()) result_type;
    std::packaged_task<result_type ()> task(std::move(f));
    std::unique_future<result_type> future = task.get_future();
    std::thread thread(std::move(task));
    thread.detach();
    return future;
}
