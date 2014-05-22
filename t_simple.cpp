// ============================================================================

#include <iostream>
#include <sstream>

#include <utility>
#include <iterator>

// Standard Containers:
#include <complex>              // complex
#include <vector>               // vector, bit_vector
#include <list>                 // list
#include <queue>                // queue, priority_queue
#include <deque>
#include <map>                  // map, multimap
#include <set>                  // set, multiset
#include <stack>

#include <memory>               // auto_ptr
#include <algorithm>

// C++ class wrapper for GMP types
#include <gmpxx.h>

typedef mpz_class MpZ;
typedef mpq_class MpQ;
typedef mpf_class MpF;

// ============================================================================

namespace Tests
{
    using namespace std;

    template<typename T>
    std::ostream & operator << (std::ostream & os,
                                const std::vector<T> & in)
    {
        os << '[';
        if (in.size() > 0)
            os << in[0];
        for (size_t j = 1; j < in.size(); j++)
            os << ' ' << in[j];
        os << ']';
        return os;
    }

    void test_vector(int n = 10)
    {
        vector<double> v(n);
        v[0] = 1;
        v[n - 1] = 1;
        cout << v << endl;
    }

    void test_complex()
    {
        complex<double> c(3.0, 4.0);
        cout << c << " has the abs " << abs(c) << endl;
    }

    template<typename T1, class T2>
    std::ostream & operator << (std::ostream & os,
                                const std::pair<T1, T2> & in)
    {
        return os << '{' << in.first << ' ' << in.second << '}';
    }

    void test_pair()
    {
        pair<int, double> p(11, 13);
        cout << p << endl;
    }

    void test_list()
    {

        list<string> names, other_names;

        names.push_back(string("Alfa"));
        names.push_back(string("Bravo"));
        names.push_back(string("Golf"));
        names.push_back(string("Quebec"));

        other_names.push_back(string("Oscar"));
        other_names.push_back(string("Mike"));
        other_names.push_back(string("November"));
        other_names.push_back(string("Zulu"));

        names.merge(other_names);

        cout << "Initially we have: " << endl;

        list<string>::iterator from;

        for (from = begin(names); from != end(names); ++from)
            cout << *from << ' ';
        cout << endl;

        names.sort();

        cout << "After sort() we have: " << endl;
        for (from = begin(names); from != end(names); ++from)
            cout << *from << ' ';
        cout << endl;

        // Makes sure that each element will occur only once
        names.unique();

        cout << "After unique() we have: " << endl;
        for (from = begin(names); from != end(names); ++from)
            cout << *from << ' ';
        cout << endl;

    }

    void test_stack()
    {
        stack<complex<double> >
            cstack;

        cstack.push(complex<double>(3.14, 2.71));
        cstack.push(complex<double>(-3.14, -2.71));

        while (cstack.size())
        {
            cout << cstack.top().real() << ", " <<
                cstack.top().imag() << "i" << endl;
            cstack.pop();
        }
    }

    class CaseString {
    public:
        bool operator()(string const & first, string const & second) const {
            return (strcasecmp(first.c_str(), second.c_str()) < 0);
        }
    };

    void test_next_permutation()
    {
        string saints[] = {"Oh", "when", "the", "saints"};

        cout << "All permutations of 'Oh when the saints':\n";
        cout << "Sequences:\n";
        do {
            copy(saints, saints + 4, ostream_iterator<string>(cout, " "));
            cout << endl;
        } while (next_permutation(saints, saints + 4, CaseString()));

        cout << "After first sorting the sequence:\n";

        sort(saints, saints + 4, CaseString());

        cout << "Sequences:\n";
        do {
            copy(saints, saints + 4, ostream_iterator<string>(cout, " "));
            cout << endl;
        } while (next_permutation(saints, saints + 4, CaseString()));
    }

    void test_gmp()
    {
        mpz_class a, b, c;

        a = 1234;
        b = "-5678";
        c = a + b;
        cout << "sum is " << c << "\n";
//          cout << "absolute value is " << abs(c) << "\n";
    }

}

// ============================================================================

int main(int argc, char * argv[])
{
//      Tests::test_next_permutation();
//      Tests::test_gmp();
    Tests::test_stack();

    return 0;
}
