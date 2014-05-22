#include <cstring>
#include <cstdlib>
#include <iostream>
#include <string>

// STL
// #ifdef __GNUC__
// #  include <ext/hash_map>
// #else
// #  include <hash_map>
// #endif
// namespace std
// {
// using namespace __gnu_cxx;
// }

// C++ Techniqual Report 1 (TR1)
#include <tr1/unordered_map>
// C++11
#include <unordered_map>
// Boost
#include <boost/unordered_map.hpp>

// Sparsehash
#include <sparsehash/sparse_hash_map>
#include <sparsehash/dense_hash_map>
#include <sparsehash/sparse_hash_set>
#include <sparsehash/dense_hash_set>

// #include <nedtries/nedtrie.h>

#include <boost/array.hpp>

#include "xhash.hpp"
#include "cycle.h"
#include "rand.h"

// using google::sparse_hash_map;
// using google::dense_hash_map;
// using google::sparse_hash_set;
// using google::dense_hash_set;

using std::cout;
using std::cerr;
using std::endl;

template<class T>
struct eq {
    bool operator() (const T& a, const T& b) const { return a == b; }
};

//typedef std::string Tk;     // key type
typedef uint64_t Tk;            // key type
typedef int Tv;                 // value type

/*! Needed by \c unordered_map for a custom hash function.
 *
 * \see http://alexkr.com/source-code/13/how-to-use-hash_map/
 */
template<class T>
struct HashConfiguration{
    enum {
        bucket_size = 4,  // 0 < bucket_size
        min_buckets = 8}; // min_buckets = 2 ^^ N, 0 < N

    size_t operator()(const T& key_value) const {
        // here is the computation of the hash function for any VALUE_KEY_CLASS object
        // ... add your code here and return size_t
        // ...
        // ... in my example I just convert to int value
        return (int)(key_value);

    }

    bool operator()(const T& left, const T& right) const {
        //here should be the code to compare two VALUE_KEY_CLASS objects
        //
        return (int)left < (int)right;
    }
};

template<typename T>
double test_insert_set(T& a, size_t n)
{
    auto t0 = getticks();
    for (size_t i = 0; i < n; i++) {
        //Tk k("sample string");
        a.insert(int64_rand());
    }
    auto t1 = getticks();
    return elapsed(t1, t0) * 1.0E-9;
}

template<typename T>
double test_insert_map(T& a, size_t n)
{
    auto t0 = getticks();
    for (size_t i = 0; i < n; i++) {
        //Tk s("sample string");
        auto k = int64_rand();
        a.insert(std::make_pair(k, i));
        //a.emplace(k, i);
    }
    auto t1 = getticks();
    return elapsed(t1, t0) * 1.0E-9;
}

template<typename T>
double test_insert_nedtrie(T& a, size_t n)
{
    auto t0 = getticks();
    for (size_t i = 0; i < n; i++) {
        auto k = int64_rand();
        a[k] = (int)i;
    }
    auto t1 = getticks();
    return elapsed(t1, t0) * 1.0E-9;
}

template<class Tk, class Tv>
int test_hash(size_t m, size_t n)
{
    {
        std::tr1::unordered_map<Tk, Tv, std::hash<Tk>, eq<Tk> > map;
        cout << "std::tr1::unordered_map: " << n << " inserts took: " << test_insert_map(map, n) << "s" << endl;
    }
    {
        std::unordered_map<Tk, Tv, std::hash<Tk>, eq<Tk> > map;
        cout << "std::unordered_map: " << n << " inserts took: " << test_insert_map(map, n) << "s" << endl;
    }
    {
        boost::unordered_map<Tk, Tv, std::hash<Tk>, eq<Tk> > map;
        cout << "boost::unordered_map: " << n << " inserts took: " << test_insert_map(map, n) << "s" << endl;
    }
    {
        google::sparse_hash_set<Tk, std::hash<Tk>, eq<Tk> > set;
        cout << "google::sparse_hash_set: " << n << " inserts took: " << test_insert_set(set, n) << "s" << endl;
    }
    {
        google::dense_hash_set<Tk, std::hash<Tk>, eq<Tk> > set;
        set.set_empty_key(0);
        cout << "google::dense_hash_set: " << n << " inserts took: " << test_insert_set(set, n) << "s" << endl;
    }
    {
        google::sparse_hash_map<Tk, Tv, std::hash<Tk>, eq<Tk> > map;
        cout << "google::sparse_hash_map: " << n << " inserts took: " << test_insert_map(map, n) << "s" << endl;
    }
    {
        google::dense_hash_map<Tk, Tv, std::hash<Tk>, eq<Tk> > map;
        map.set_empty_key(0);
        cout << "google::dense_hash_map: " << n << " inserts took: " << test_insert_map(map, n) << "s" << endl;
    }

    {
//        nedtries::trie_map<Tk, Tv> trie_map;
//        cout << "nedtries::trie_map<Tk, Tv>: " << n << " inserts took: " << test_insert_nedtrie(trie_map, m, n) << "s" << endl;
    }

    // xHash
    // xHash xhash;
    // cout << "xhash: inserts: " << test_insert_map(xhash, m, n) << endl;

    return 0;
}

int main(int argc, char * argv[])
{
    test_hash<Tk, Tv>(16, 1e6);
}
