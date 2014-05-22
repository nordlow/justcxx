/*!
 * \file t_ngram.cpp
 * \brief Test N-gram.
 */

#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <boost/spirit/include/support_istream_iterator.hpp>
#include <boost/spirit/include/qi_binary.hpp>

#include "rand.hpp"
#include "ngram.hpp"
#include "show_ngram.hpp"
#include "show_vector.hpp"
#include "algorithm_x.hpp"

/*! Test N-Gram. */
template<class V> void test_ngram(size_t num, size_t nlevels = 3, bool show_flag = false)
{
    using std::cout;
    using std::endl;
    typedef std::vector<V> C;   // container
    C seq = rand_n<C>(num);
    cout << "minmax: " << std::minmax_element_value(seq) << endl;

    pnw::histogram::ngram::tree<V> ngram(begin(seq), end(seq), nlevels);
    if (show_flag) {
        dshowln(ngram);
    }
    // pnw::histogram::dense<V> dh(begin(seq), end(seq));
    // pnw::histogram::sparse<V,C> sh(begin(seq), end(seq));
}

void test_ngram_all(size_t num = 1e2,
                    size_t nlevels = 3) {
    test_ngram<uint8_t>(num, nlevels, true);
    test_ngram<uint16_t>(num, nlevels, true);
}

void bench_ngram_all(size_t num = 1e6,
                     size_t nlevels = 3) {
    test_ngram<uint8_t>(num, nlevels, false);
    test_ngram<uint16_t>(num, nlevels, false);
}

int main(int argc, const char * argv[], const char * envp[])
{
    std::ifstream in("some_data_file"); // open file

    in.unsetf(std::ios::skipws);        // disable whitespace skipping

    boost::spirit::istream_iterator begin(in);// wrap istream into iterator
    boost::spirit::istream_iterator end;

    // parse file data
    // boost::<spirit2::qi::parse(begin, end, <...your grammar here...>);

    test_ngram_all();
    bench_ngram_all();

    return 0;
}
