/*! \file t_boost_serialization.cpp
 * \brief Test Boost::Serialization.
 * \see http://www.fnord.ca/articles/xml.html
 */

#include <fstream>
#include <vector>
#include <tuple>
#include <boost/tuple/tuple.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/bitset.hpp>
//#include <boost/serialization/hash_map.hpp>
//#include <boost/serialization/hash_set.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/basic_text_iarchive.hpp>
#include <boost/archive/basic_text_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/preprocessor/repetition.hpp>
#include "geometry/vec.hpp"
#include "geometry/box.hpp"
#include "geometry/rbox.hpp"
#include "geometry/sphere.hpp"
#include "geometry/hplane.hpp"
#include "iostream_x.hpp"
#include "enforce.hpp"
#include "show.hpp"
#include "cc_features.h"

namespace boost { namespace serialization {
#if HAVE_CXX11_VARIADIC_TEMPLATES
#else
#endif
template<class Ar, class T>                                     void serialize(Ar& ar, std::tuple<T>& a,         const uint version) { ar & get<0>(a); }
template<class Ar, class T, class U>                            void serialize(Ar& ar, std::tuple<T,U>& a,       const uint version) { ar & get<0>(a) & get<1>(a); }
template<class Ar, class T, class U, class V>                   void serialize(Ar& ar, std::tuple<T,U,V>& a,     const uint version) { ar & get<0>(a) & get<1>(a) & get<2>(a); }
template<class Ar, class T, class U, class V, class W>          void serialize(Ar& ar, std::tuple<T,U,V,W>& a,   const uint version) { ar & get<0>(a) & get<1>(a) & get<2>(a) & get<3>(a); }
template<class Ar, class T, class U, class V, class W, class X> void serialize(Ar& ar, std::tuple<T,U,V,W,X>& a, const uint version) { ar & get<0>(a) & get<1>(a) & get<2>(a) & get<3>(a) & get<4>(a); }
template<class Ar, class T>                                     void serialize(Ar& ar, boost::tuple<T>& a,         const uint version) { ar & get<0>(a); }
template<class Ar, class T, class U>                            void serialize(Ar& ar, boost::tuple<T,U>& a,       const uint version) { ar & get<0>(a) & get<1>(a); }
template<class Ar, class T, class U, class V>                   void serialize(Ar& ar, boost::tuple<T,U,V>& a,     const uint version) { ar & get<0>(a) & get<1>(a) & get<2>(a); }
template<class Ar, class T, class U, class V, class W>          void serialize(Ar& ar, boost::tuple<T,U,V,W>& a,   const uint version) { ar & get<0>(a) & get<1>(a) & get<2>(a) & get<3>(a); }
template<class Ar, class T, class U, class V, class W, class X> void serialize(Ar& ar, boost::tuple<T,U,V,W,X>& a, const uint version) { ar & get<0>(a) & get<1>(a) & get<2>(a) & get<3>(a) & get<4>(a); }

/*! This C Macro Magic fails for me.
 * \see http://uint32t.blogspot.se/2008/03/update-serializing-boosttuple-using.html
 */
#if 0
#define GENERATE_ELEMENT_SERIALIZE(z,which,unused)                      \
    ar & boost::serialization::make_nvp("element",t.get< which >());
#define GENERATE_STD_TUPLE_SERIALIZE(z,nargs,unused)                        \
    template< typename Archive, BOOST_PP_ENUM_PARAMS(nargs,typename T) > \
    void serialize(Archive & ar,                                        \
                   std::tuple< BOOST_PP_ENUM_PARAMS(nargs,T) > & t,   \
                   const unsigned int version)                          \
    {                                                                   \
        BOOST_PP_REPEAT_FROM_TO(0,nargs,GENERATE_ELEMENT_SERIALIZE,~);  \
    }
#define GENERATE_BOOST_TUPLE_SERIALIZE(z,nargs,unused)                  \
    template< typename Archive, BOOST_PP_ENUM_PARAMS(nargs,typename T) > \
    void serialize(Archive & ar,                                        \
                   boost::tuple< BOOST_PP_ENUM_PARAMS(nargs,T) > & t,   \
                   const unsigned int version)                          \
    {                                                                   \
        BOOST_PP_REPEAT_FROM_TO(0,nargs,GENERATE_ELEMENT_SERIALIZE,~);  \
    }
BOOST_PP_REPEAT_FROM_TO(1,3,GENERATE_STD_TUPLE_SERIALIZE,~);
BOOST_PP_REPEAT_FROM_TO(1,3,GENERATE_BOOST_TUPLE_SERIALIZE,~);
#endif

}}

/*! GPS Position. */
class GPS_Pos {
public:
    GPS_Pos() {};
    GPS_Pos(int d, size_t m, float s) : m_deg(d), m_min(m), m_sec(s), m_pair(1.1,2.2), m_tuple(1.1,2.2,3.3), m_btuple(1.1,2.2,3.3) {}
private:
    friend class boost::serialization::access;
    template<class Ar> void serialize(Ar& ar, const unsigned int version) { ar & m_deg & m_min & m_sec & m_pair & m_tuple & m_btuple; } // only one access!
    int m_deg;                  ///< Degrees
    size_t m_min;               ///< Minutes
    float m_sec;                ///< Seconds.
    std::pair<float,double> m_pair;
    std::tuple<float,double,long double> m_tuple;
    boost::tuple<float,double,long double> m_btuple;
};

/*! Test Boost::Serialization
 * \tparam OAr OutputArchive
 * \tparam IAr InputArchive
 */
template<class OAr, class IAr, class T>
inline void test_boost_serialization(const GPS_Pos& pos, const char * filename, bool assert_flag = true) {
    std::ofstream ofs; // output archive
    auto temp_filename = open_temporary(ofs, filename);
    OAr oa(ofs, boost::archive::no_header); // \note: Skip leading "magic" bits!

    oa << pos;
    vec2<T> vec2_({11,22}); oa << vec2_;
    box2<T> box2_({{11,22},{33,44}}); oa << box2_;
    rbox2<T> rbox2_({{11,22},{33,44}},{55,66}); oa << rbox2_;
    circle<T> circle_(55,66); oa << circle_;
    sphere3<T> sphere3_(77,88); oa << sphere3_;
    hplane3<T> hplane3_({1,2,3},1000); oa << hplane3_;
    ofs.close();

    std::ifstream ifs(temp_filename); // input archive
    IAr ia(ifs, boost::archive::no_header); // \note: Skip leading "magic" bits!

    GPS_Pos pos_; ia >> pos_;
    vec2<T> vec2__; ia >> vec2__; if (assert_flag) { enforce_eq(vec2__, vec2_); }
    box2<T> box2__; ia >> box2__; if (assert_flag) { enforce_eq(box2__, box2_); }
    rbox2<T> rbox2__; ia >> rbox2__; if (assert_flag) { enforce_eq(rbox2__, rbox2_); }
    circle<T> circle__; ia >> circle__; if (assert_flag) { enforce_eq(circle__, circle_); }
    sphere3<T> sphere3__; ia >> sphere3__; if (assert_flag) { enforce_eq(sphere3__, sphere3_); }
    hplane3<T> hplane3__; ia >> hplane3__; if (assert_flag) { enforce_eq(hplane3__, hplane3_); }
}

int main(int argc, char *argv[]) {
//    test_open_temp("sample_tag");
    const GPS_Pos p(35, 59, 24.567f);
    namespace ba = boost::archive;
    // test_boost_serialization<ba::xml_oarchive, ba::xml_iarchive>(p, "boost_serialization_xml_archive");
    // test_boost_serialization<ba::basic_text_oarchive, ba::basic_text_iarchive>(p, "boost_serialization_basic_text_archive");

    test_boost_serialization<ba::text_oarchive, ba::text_iarchive, double>(p, "boost_serialization_text_archive_double", false);
    test_boost_serialization<ba::binary_oarchive, ba::binary_iarchive, double>(p, "boost_serialization_binary_archive_double");

    test_boost_serialization<ba::text_oarchive, ba::text_iarchive, long double>(p, "boost_serialization_text_archive_long_double", false);
    test_boost_serialization<ba::binary_oarchive, ba::binary_iarchive, long double>(p, "boost_serialization_binary_archive_long_double");
    return 0;
}
