#ifdef _MSC_VER
#  define _SECURE_SCL 0
#  define _CRT_SECURE_NO_DEPRECATE 1
#  define WIN32_LEAN_AND_MEAN
#  define VC_EXTRALEAN
#  define NOMINMAX
#endif

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include "cycle.h"

using std::cout;
using std::endl;

static const int N = 100000;
static const size_t R = 7;

void PrintStats(std::vector<double> timings) {
    cout << std::fixed << std::setprecision(2);
    cout << "[";
    for (size_t i = 1 ; i<timings.size()-1 ; ++i) {
        cout << timings[i] << ",";
    }
    cout << timings.back();
    cout << "]";

    double sum = 0.0;
    for (size_t i = 1 ; i<timings.size() ; ++i) {
        sum += timings[i];
    }
    double avg = sum / double(timings.size()-1);

    sum = 0.0;
    for (size_t i = 1 ; i<timings.size() ; ++i) {
        timings[i] = pow(timings[i]-avg, 2);
        sum += timings[i];
    }
    double var = sum/(timings.size()-2);
    double sdv = sqrt(var);

    cout << " with average " << avg << ", stddev " << sdv;
}

int naive(const char *p) {
    int x = 0;
    bool neg = false;
    if (*p == '-') {
        neg = true;
        ++p;
    }
    while (*p >= '0' && *p <= '9') {
        x = (x*10) + (*p - '0');
        ++p;
    }
    if (neg) {
        x = -x;
    }
    return x;
}

int main(int argc, char *argv[])
    std::vector<std::string> nums;
    nums.reserve(N);
    for (int i=0-(N/2) ; i<N/2 ; ++i) {
        std::string y = boost::lexical_cast<std::string>(i);
        nums.push_back(y);
    }

    {
        int tsum = 0;
        std::vector<double> timings;
        timings.reserve(R);
        for (size_t r=0 ; r<R ; ++r) {
            ticks start = getticks();
            for (size_t i=0 ; i<nums.size() ; ++i) {
                int x = boost::lexical_cast<int>(nums[i]);
                tsum += x;
            }
            ticks end = getticks();
            double timed = elapsed(end, start);
            timings.push_back(timed);
        }

        cout << "lexical_cast: ";
        PrintStats(timings);
        cout << endl;
        cout << tsum << endl;
    }

    {
        int tsum = 0;
        std::vector<double> timings;
        timings.reserve(R);
        for (size_t r=0 ; r<R ; ++r) {
            ticks start = getticks();
            for (size_t i=0 ; i<nums.size() ; ++i) {
                std::stringstream ss(nums[i]);
                int x = 0;
                ss >> x;
                tsum += x;
            }
            ticks end = getticks();
            double timed = elapsed(end, start);
            timings.push_back(timed);
        }

        cout << "stringstream: ";
        PrintStats(timings);
        cout << endl;
        cout << tsum << endl;
    }

    {
        int tsum = 0;
        std::vector<double> timings;
        timings.reserve(R);
        for (size_t r=0 ; r<R ; ++r) {
            ticks start = getticks();
            std::stringstream ss;
            for (size_t i=0 ; i<nums.size() ; ++i) {
                ss.str(nums[i]);
                ss.clear();
                int x = 0;
                ss >> x;
                tsum += x;
            }
            ticks end = getticks();
            double timed = elapsed(end, start);
            timings.push_back(timed);
        }

        cout << "stringstream reused: ";
        PrintStats(timings);
        cout << endl;
        cout << tsum << endl;
    }

    {
        int tsum = 0;
        std::vector<double> timings;
        timings.reserve(R);
        for (size_t r=0 ; r<R ; ++r) {
            ticks start = getticks();
            for (size_t i=0 ; i<nums.size() ; ++i) {
                int x = atoi(nums[i].c_str());
                tsum += x;
            }
            ticks end = getticks();
            double timed = elapsed(end, start);
            timings.push_back(timed);
        }

        cout << "atoi(): ";
        PrintStats(timings);
        cout << endl;
        cout << tsum << endl;
    }

    {
        int tsum = 0;
        std::vector<double> timings;
        timings.reserve(R);
        for (size_t r=0 ; r<R ; ++r) {
            ticks start = getticks();
            for (size_t i=0 ; i<nums.size() ; ++i) {
                int x = atol(nums[i].c_str());
                tsum += x;
            }
            ticks end = getticks();
            double timed = elapsed(end, start);
            timings.push_back(timed);
        }

        cout << "atol(): ";
        PrintStats(timings);
        cout << endl;
        cout << tsum << endl;
    }

    {
        int tsum = 0;
        std::vector<double> timings;
        timings.reserve(R);
        for (size_t r=0 ; r<R ; ++r) {
            ticks start = getticks();
            for (size_t i=0 ; i<nums.size() ; ++i) {
                int x = strtol(nums[i].c_str(), 0, 10);
                tsum += x;
            }
            ticks end = getticks();
            double timed = elapsed(end, start);
            timings.push_back(timed);
        }

        cout << "strtol(): ";
        PrintStats(timings);
        cout << endl;
        cout << tsum << endl;
    }

    {
        int tsum = 0;
        std::vector<double> timings;
        timings.reserve(R);
        for (size_t r=0 ; r<R ; ++r) {
            ticks start = getticks();
            for (size_t i=0 ; i<nums.size() ; ++i) {
                int x = naive(nums[i].c_str());
                tsum += x;
            }
            ticks end = getticks();
            double timed = elapsed(end, start);
            timings.push_back(timed);
        }

        cout << "naive: ";
        PrintStats(timings);
        cout << endl;
        cout << tsum << endl;
    }
}
