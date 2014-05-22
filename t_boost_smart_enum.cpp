//
// Test program to verify smart_enum<> and friends.
//
// Copyright (c) 2002-2007 Peter Simons <simons@cryp.to>
//
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/smart_enum.hpp>
#include <iostream>
#include "raw.hpp"

#if 0
#include <boost/static_assert.hpp>
#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>
#include <iomanip>
BOOST_AUTO_TEST_CASE( test_state_enum )
{
    enum myEnum { state1 = 5, state2, state3, state4 };
    typedef boost::bounded_smart_enum<myEnum, state1, state4> my_enum_t;
    my_enum_t e(state1);
    // Guarantee the boundaries.
    BOOST_STATIC_ASSERT((myEnum(my_enum_t::min) == state1));
    BOOST_STATIC_ASSERT((myEnum(my_enum_t::max) == state4));
    // Verify upper bound.
    BOOST_REQUIRE_EQUAL(e, state1);
    BOOST_REQUIRE_EQUAL(++e, state2);
    BOOST_REQUIRE_EQUAL(++e, state3);
    BOOST_REQUIRE_EQUAL(++e, state4);
    BOOST_REQUIRE_EQUAL(++e, state4);
    // Verify lower bound.
    BOOST_REQUIRE_EQUAL(--e, state3);
    BOOST_REQUIRE_EQUAL(--e, state2);
    BOOST_REQUIRE_EQUAL(--e, state1);
    BOOST_REQUIRE_EQUAL(--e, state1);
    // Verify assignment.
    BOOST_REQUIRE_EQUAL((e = 50), state4);
    BOOST_REQUIRE_EQUAL((e = -50), state1);
}
#endif

using namespace std;

template<class E>
void test_smart_enum_iteration_safe()
{
    E x = E::min;
    while (true) {
        cout << x << ",";
        if (x == E::max) { break; }
        x++;
    };
    cout << endl;
}

template<class E>
void test_smart_enum_iteration_for()
{
    for (E i = E::min; i != i+1; i++) {
        cout << i << ",";
    }
    cout << endl;
}

template<class E>
void test_smart_enum_iteration_unsafe()
{
    E x = E::min;
    for (int i = 0; i < (E::max - E::min+2); i++, x++) {
        cout << x << ",";
    }
    cout << endl;
}

int main(int argc, char * argv[], char * envp[])
{
    enum E { state1 = 5, state2, state3, state4 };

    cout << "bounded_smart_enum: ";
    test_smart_enum_iteration_safe<boost::bounded_smart_enum<E, state1, state4>>();
    cout << "wrapped_smart_enum: ";
    test_smart_enum_iteration_safe<boost::wrapped_smart_enum<E, state1, state4>>();
    cout << "sequential_smart_enum: ";
    test_smart_enum_iteration_safe<boost::sequential_smart_enum<E, state1, state4>>();

    cout << "bounded_smart_enum: ";
    test_smart_enum_iteration_for<boost::bounded_smart_enum<E, state1, state4>>();
    cout << "wrapped_smart_enum: ";
    test_smart_enum_iteration_for<boost::wrapped_smart_enum<E, state1, state4>>();
    cout << "sequential_smart_enum: ";
    test_smart_enum_iteration_for<boost::sequential_smart_enum<E, state1, state4>>();

    cout << "bounded_smart_enum: ";
    test_smart_enum_iteration_unsafe<boost::bounded_smart_enum<E, state1, state4>>();
    cout << "wrapped_smart_enum: ";
    test_smart_enum_iteration_unsafe<boost::wrapped_smart_enum<E, state1, state4>>();
    cout << "sequential_smart_enum: ";
    test_smart_enum_iteration_unsafe<boost::sequential_smart_enum<E, state1, state4>>();

    return 0;
}
