#include <fstream>
#include <iostream>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

using namespace std;

class Test {
private:
    friend class boost::serialization::access;
    template<class Archive> void serialize(Archive & ar,
                                           const unsigned int version) {
        ar & BOOST_SERIALIZATION_NVP(a);
        ar & BOOST_SERIALIZATION_NVP(b);
        ar & BOOST_SERIALIZATION_NVP(c);
    }

    int b;
    float c;
public:
    inline Test(int a, int b, float c) {
        this->a = a;
        this->b = b;
        this->c = c;
    }
};

int main(int argc, char *argv[])
{
    std::ofstream ofs("/tmp/filename.xml");
    Test* t = new Test(1, 2, 3.3);
    boost::archive::xml_oarchive oa(ofs);
    oa << BOOST_SERIALIZATION_NVP(t);
    return 0;
}
