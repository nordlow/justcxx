#include <complex>
#include <iostream>
#include "cc_features.h"

#include <boost/typeof/std/complex.hpp>

// Many Separate Includes is Cumbersome to use but motivated by C++11 bad Compilation Performance
#include <boost/units/pow.hpp>
#include <boost/units/systems/si/energy.hpp>
#include <boost/units/systems/si/force.hpp>
#include <boost/units/systems/si/length.hpp>
#include <boost/units/systems/si/electric_potential.hpp>
#include <boost/units/systems/si/current.hpp>
#include <boost/units/systems/si/resistance.hpp>
#include <boost/units/systems/si/io.hpp>

// #include "units.hpp"
// #include "show_units.hpp"

using namespace boost::units;
using namespace boost::units::si;

/*! Get Work (Energy) \p F * \p dx.
 * \tparam T is Numeric Type
 */
template<class T>
inline quantity<energy,T> work(const quantity<force,T>& F,
                               const quantity<length,T>& dx)
{
    return F*dx;
}

int main(int argc, char *argv[])
{
    using std::cout;
    using std::endl;

    auto F = 2.0*newton;        // force
    auto dx = 2.0*meter;        // distance
    auto E = work(F,dx);        // energy
    auto E_ = F*dx;             // energy

    cout << "F  = " << F << " (size:" << sizeof(F) << ")" << std::endl
         << "dx = " << dx << " (size:" << sizeof(dx) << ")" << std::endl
         << "E  = " << E << " (size:" << sizeof(E) << ")" << std::endl
         << "E_  = " << E << " (size:" << sizeof(E_) << ")" << std::endl
         << endl;

    typedef std::complex<double> C;

#if HAVE_CXX11_AUTO_TYPED_VARIABLES
    auto v = C(12.5, 0.0) * volts;  // electric potential
    auto i = C(3.0, 4.0) * amperes; // current
    auto z = C(1.5, -2.0) * ohms;   // resistance
#else
    quantity<electric_potential,C> v = C(12.5,0.0) * volts;
    quantity<current,C>            i = C(3.0,4.0) * amperes;
    quantity<resistance,C>         z = C(1.5,-2.0) * ohms;
#endif

    cout << "V   = " << v << endl
         << "I   = " << i << endl
         << "Z   = " << z << endl
         << "I*Z = " << i*z << endl
         << "I*Z == V? " << std::boolalpha << (i*z == v) << endl
         << endl;

    auto x = (2.0*newton);
    cout << "x = " << x << endl
         << "x*x = " << x*x << endl
         << "x/x = " << x/x
         << endl
//         << "x = " << root(x*x) << endl
        ;

    // cout << "3dB: " << dB(3) << endl;
    // cout << "1e3= "; pnw::siunit_show(1e3); cout << endl;

    return 0;
}
