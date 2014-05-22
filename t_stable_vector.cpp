/* Stable vector test suite.
 *
 * Copyright 2008 Joaquín M López Muñoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

#define STABLE_VECTOR_ENABLE_INVARIANT_CHECKING

#include <boost/detail/lightweight_test.hpp>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/iterator/iterator_adaptor.hpp>
#include "stable_vector.hpp"

struct input_counting_iterator:
    boost::iterator_adaptor<
    input_counting_iterator,
    boost::counting_iterator<int>,
    boost::use_default,
    std::input_iterator_tag
    >
{
    input_counting_iterator(int x):
        input_counting_iterator::iterator_adaptor_(x){}
};

int main(int argc, char *argv[])
{
    stable_vector<int> v1;
    BOOST_TEST(v1.size()==0&&v1.empty());

    stable_vector<int> v2(20,6);
    BOOST_TEST(v2.size()==20);

    stable_vector<int> v3(v2.begin(),v2.end());
    BOOST_TEST(v3==v2);

    stable_vector<int> v4(v2);
    BOOST_TEST(v4==v2);

    v2.push_back(10);
    BOOST_TEST(v4!=v2);

    v4=v2;
    BOOST_TEST(v4==v2);

    stable_vector<int> v5((std::size_t)17,18);
    BOOST_TEST(v5.size()==17&&v5[0]==18);

    stable_vector<int> v6(
        input_counting_iterator(0),
        input_counting_iterator(100));
    BOOST_TEST(v6.size()==100&&v6[0]==0&&v6[99]==99);

    v3.assign(v2.begin(),v2.end());
    BOOST_TEST(v3==v2);

    v3.assign(25,20);
    BOOST_TEST(v3.size()==25&&v3[0]==20);

    v3.assign((std::size_t)12,40);
    BOOST_TEST(v3.size()==12&&v3[0]==40);

    BOOST_TEST(v3.get_allocator()==v2.get_allocator());

    const stable_vector<int>&          cv3=v3;
    stable_vector<int>::iterator       it1=v3.begin(),
        it2=v3.end();
    stable_vector<int>::const_iterator cit1=cv3.begin(),
        cit11=v3.cbegin(),
        cit2=cv3.cend(),
        cit21=v3.cend();
    BOOST_TEST(it1==cit1&&cit1==cit11);
    BOOST_TEST(it2==cit2&&cit2==cit21);
    BOOST_TEST(std::distance(it1,it2)==(std::ptrdiff_t)v3.size());
    BOOST_TEST(std::distance(cit1,cit2)==(std::ptrdiff_t)v3.size());
    BOOST_TEST(it1+v3.size()==it2);
    BOOST_TEST(cit1+v3.size()==cit2);
    BOOST_TEST(it2-v3.size()==it1);
    BOOST_TEST(cit2-v3.size()==cit1);

    stable_vector<int>::reverse_iterator       rit1=v3.rbegin(),
        rit2=v3.rend();
    stable_vector<int>::const_reverse_iterator crit1=cv3.rbegin(),
        crit11=v3.crbegin(),
        crit2=cv3.crend(),
        crit21=v3.crend();
    BOOST_TEST(crit1==crit11);
    BOOST_TEST(crit2==crit21);
    BOOST_TEST(std::distance(rit1,rit2)==(std::ptrdiff_t)v3.size());
    BOOST_TEST(std::distance(crit1,crit2)==(std::ptrdiff_t)v3.size());
    BOOST_TEST(rit1+v3.size()==rit2);
    BOOST_TEST(crit1+v3.size()==crit2);
    BOOST_TEST(rit2-v3.size()==rit1);
    BOOST_TEST(crit2-v3.size()==crit1);
    BOOST_TEST(&*rit1==&v3.back());
    BOOST_TEST(&*(rit2-1)==&v3.front());
    BOOST_TEST(&*crit1==&cv3.back());
    BOOST_TEST(&*(crit2-1)==&cv3.front());

    BOOST_TEST(v3.max_size()==v3.max_size());

    std::size_t s=v3.size();
    v3.resize(s);
    BOOST_TEST(v3.size()==s);
    v3.resize(s/2);
    BOOST_TEST(v3.size()<s);
    v3.resize(2*s,10);
    BOOST_TEST(v3.size()>s);

    BOOST_TEST(v3.capacity()>=v3.size());
    std::size_t                  c=v3.capacity();
    stable_vector<int>::iterator it=v3.begin()+10;
    v3.reserve(2*c);
    BOOST_TEST(v3.capacity()>=2*c);
    BOOST_TEST(std::distance(v3.begin(),it)==10);

    BOOST_TEST(&v3[5]==&*(v3.begin()+5));
    BOOST_TEST(&cv3[5]==&*(v3.begin()+5));
    BOOST_TEST(&v3.at(5)==&*(v3.begin()+5));
    BOOST_TEST(&cv3.at(5)==&*(v3.begin()+5));

    int exc_count=0;
    try{v3.at(v3.size());}catch(const std::out_of_range&){++exc_count;}
    try{cv3.at(cv3.size());}catch(const std::out_of_range&){++exc_count;}
    BOOST_TEST(exc_count==2);

    v4=v3;
    v3.push_back(101);
    BOOST_TEST(v3.back()==101);
    v3.pop_back();
    BOOST_TEST(v3==v4);

    it=v3.insert(v3.begin()+10,99);
    BOOST_TEST(std::distance(v3.begin(),it)==10);
    BOOST_TEST(*it==99);

    s=v3.size();
    v3.insert(v3.begin()+10,100,71);
    BOOST_TEST(v3.size()==s+100);
    BOOST_TEST(v3[10]==71);

    s=v3.size();
    v3.insert(v3.begin()+10,(std::size_t)10,72);
    BOOST_TEST(v3.size()==s+10);
    BOOST_TEST(v3[10]==72);

    s=v3.size();
    v3.insert(v3.begin()+10,v2.begin(),v2.end());
    BOOST_TEST(v3.size()==s+v2.size());
    BOOST_TEST(v3[10]==v2[0]);

    s=v3.size();
    v3.reserve(s+100);
    v3.insert(
        v3.begin()+10,
        input_counting_iterator(68),input_counting_iterator(168));
    BOOST_TEST(v3.size()==s+100);
    BOOST_TEST(v3[10]==68);

    it=v3.begin()+11;
    v3.erase(v3.begin()+10);
    BOOST_TEST(it==v3.begin()+10);

    it=v3.begin()+11;
    v3.erase(v3.begin(),it);
    BOOST_TEST(it==v3.begin());

    v1=v2;
    BOOST_TEST(v3!=v1&&v2==v1);
    v3.swap(v2);
    BOOST_TEST(v3==v1&&v2!=v1);
    swap(v3,v2);
    BOOST_TEST(v3!=v1&&v2==v1);

    v3.clear();
    BOOST_TEST(v3.size()==0);

    v3=v2;
    ++v3.back();
    BOOST_TEST(
        !(v3==v2)&&!(v3< v2)&& (v3!=v2)&&
        (v3> v2)&& (v3>=v2)&&!(v3<=v2));

    return boost::report_errors();
}
