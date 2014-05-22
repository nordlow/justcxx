/*! \file t_boost_units.cpp
 * \brief Test Adding new units to Boost.Units.
 */

#ifdef __clang__
typedef struct { double x, y; } __float128;
extern "C" { extern char *gets(char *__s) { return 0; }; }
#endif

#include <boost/units/systems/si.hpp>
#include <boost/units/systems/angle/revolutions.hpp>
#include <boost/units/systems/angle/degrees.hpp>
#include <boost/units/conversion.hpp>
#include <boost/units/pow.hpp>
#include <boost/units/io.hpp>
#include <iostream>
#include <iterator>
#include <algorithm>

typedef boost::units::divide_typeof_helper<
    boost::units::degree::plane_angle,
    boost::units::si::time
    >::type degrees_per_second;

int main()
{
    boost::units::quantity< boost::units::si::angular_velocity> m_speed(
        (30.0*boost::units::si::radians_per_second));
    std::cout << "m_speed rad/sec: " << m_speed << std::endl;
    std::cout << "m_speed deg/sec: " << static_cast<boost::units::quantity<degrees_per_second>>(m_speed) << std::endl;
    return(0);
}
