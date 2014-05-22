/*! \file t_future.cpp
 * \brief Test futures.
 * \see http://channel9.msdn.com/Events/Lang-NEXT/Lang-NEXT-2012/-Not-Your-Father-s-C-
 */

#include <iostream>
#include <string>
#include <vector>
#include <future>
#include <algorithm>

using std::cout;
using std::endl;

std::string flip(std::string s) {
    std::reverse(begin(s), end(s));
    return s;
}

void test_future_async() {
    std::vector<std::future<std::string> > v;
    v.push_back(std::async([]{ return flip("abc"); }));
    v.push_back(std::async([]{ return flip("xyz"); }));
    v.push_back(std::async([]{ return flip("123"); }));
    for (auto& e : v) {
        cout << e.get() << endl;
    }
}

int main(int argc, const char * argv[], const char * envp[]) {
    test_future_async();
    return 0;
}
