/*! \file units.hpp
 * \brief SI Units.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2012-02-09 10:11
 * \see http://stackoverflow.com/questions/9257826/physical-boost-units-user-defined-literals
 */

#pragma once
#include <type_traits>
#include <cmath>
#include <ostream>
#include "cc_features.h"
#include "math_x.hpp"
#include <boost/units/pow.hpp>
#include <boost/units/unit.hpp>
#include <boost/units/quantity.hpp>
#include <boost/units/systems/si.hpp>
// #include <boost/units/systems/si/time.hpp>
// #include <boost/units/systems/si/energy.hpp>
// #include <boost/units/systems/si/force.hpp>
// #include <boost/units/systems/si/length.hpp>
// #include <boost/units/systems/si/electric_potential.hpp>
// #include <boost/units/systems/si/current.hpp>
// #include <boost/units/systems/si/resistance.hpp>
#include <boost/units/systems/si/io.hpp>
#include <boost/units/base_units/imperial/mile.hpp>

/* ========================================================================= */
/*! Dimensionless Units */

/*! @name Decibel.
 * \see https://en.wikipedia.org/wiki/Decibel
 */
/// @{
namespace boost { namespace units {
template<class N> N bel(const typename std::enable_if<std::is_floating_point<N>::value, N>::type & x) { return std::pow<N>(10, x); }
template<class N> N decibel(const typename std::enable_if<std::is_floating_point<N>::value, N>::type & x) { return bel(x/10); }
template<class N> N bel(const typename std::enable_if<std::is_integral<N>::value, N>::type & x) { return std::exp10(static_cast<double>(x)); }
template<class N> N decibel(const typename std::enable_if<std::is_integral<N>::value, N>::type & x) { return bel<double>(static_cast<double>(x)/10); }
template<class N> N dB(const N & x) { return decibel<N>(x); }
}}
// template<class N> in_decibel(const N & x) { return x*...; }
// template<class N> in_dB(const N & x) { return decibel(x); }
/// @}

/* ========================================================================= */
/*! Power Laws Extension. */

namespace boost { namespace units {
template<class U, class N> inline quantity<U, N> sqrt (const quantity<U, N> & x) { pow<static_rational<1,2> >(x); };
template<class U, class N> inline quantity<U, N> cbrt (const quantity<U, N> & x) { pow<static_rational<1,3> >(x); };
template<class U, class N> inline quantity<U, N> sqr  (const quantity<U, N> & x) { pow<static_rational<2,1> >(x); };
template<class U, class N> inline quantity<U, N> cube (const quantity<U, N> & x) { pow<static_rational<3,1> >(x); };
template<class U, class N> inline quantity<U, N> quart(const quantity<U, N> & x) { pow<static_rational<4,1> >(x); };
}}

/* ========================================================================= */
// Checking for User-Defined Literals (UDL) is GCC 4.7 and Clang
#if HAVE_CXX11_USER_DEFINED_LITERALSf

namespace boost { namespace units {

// Dimensionless Units

// TODO: Add these
// #define M_KILOBYTE (1   << 10)	/**< KiloByte */
// #define M_MEGABYTE (1   << 20)	/**< MegaByte */
// #define M_GIGABYTE (1   << 30)	/**< GigaByte */
// #define M_TERABYTE (1LL << 40)	/**< TeraByte */
// #define M_PETABYTE (1LL << 50)	/**< PetaByte, BubbaByte */
// #define M_EXABYTE  (1LL << 60)	/**< ExaByte */
// #define M_KB (1   << 10)	/**< KiloByte */
// #define M_MB (1   << 20)	/**< MegaByte */
// #define M_GB (1   << 30)	/**< GigaByte */
// #define M_TB (1LL << 40)	/**< TeraByte */
// #define M_PB (1LL << 50)	/**< PetaByte, BubbaByte */
// #define M_EB (1LL << 60)	/**< ExaByte */

// SI Numerical Suffixes
constexpr double operator"" _decibel (long double x) { return decibel(x); }
constexpr double operator"" _dB (double x) { return decibel(x); }

constexpr double operator"" _deka (double x) { return x * 1e1; }
constexpr double operator"" _hecto (double x) { return x * 1e2; }
constexpr double operator"" _kilo (double x) { return x * 1e3; }
constexpr double operator"" _mega (double x) { return x * 1e6; }
constexpr double operator"" _giga (double x) { return x * 1e9; }
constexpr double operator"" _tera (double x) { return x * 1e12; }
constexpr double operator"" _peta (double x) { return x * 1e15; }
constexpr double operator"" _exa (double x) { return x * 1e18; }
constexpr double operator"" _zetta (double x) { return x * 1e21; }
constexpr double operator"" _yotta (double x) { return x * 1e24; }

constexpr double operator"" _deci (double x) { return x * 1e-1; }
constexpr double operator"" _centi (double x) { return x * 1e-2; }
constexpr double operator"" _milli (double x) { return x * 1e-3; }
constexpr double operator"" _micro (double x) { return x * 1e-6; }
constexpr double operator"" _nano (double x) { return x * 1e-9; }
constexpr double operator"" _pico (double x) { return x * 1e-12; }
constexpr double operator"" _femto (double x) { return x * 1e-15; }
constexpr double operator"" _atto (double x) { return x * 1e-18; }
constexpr double operator"" _zepto (double x) { return x * 1e-21; }
constexpr double operator"" _yocto (double x) { return x * 1e-24; }

// Get \p x \em Degrees.
constexpr double operator"" _degrees (double x) { return x * M_DEG2RAD; }
constexpr double operator"" _deg (double x) { return x * M_DEG2RAD; }

// Get \p x \em Bearings.
constexpr double operator"" _bearing (double x) { return x / (2*M_PI) * 400; }
constexpr double operator"" _bear (double x) { return x / (2*M_PI) * 400; }

// Distances

constexpr quantity<si::length, double> operator"" _meter (double x) { return x*si::meters; } ///< Meter
constexpr quantity<si::length, double> operator"" _m (double x) { return x*si::meters; } ///< Meter

constexpr quantity<si::length, double> operator"" _km (double x) { return x*1e3*si::meters; } ///< Kilometer.
constexpr quantity<si::length, double> operator"" _cm (double x) { return x*1e-2*si::meters; } ///< Centimeter.
constexpr quantity<si::length, double> operator"" _dm (double x) { return x*1e-1*si::meters; } ///< Decimeter.
constexpr quantity<si::length, double> operator"" _mm (double x) { return x*1e-3*si::meters; } ///< Millimeter.
constexpr quantity<si::length, double> operator"" _um (double x) { return x*1e-6*si::meters; } ///< Micrometer.
constexpr quantity<si::length, double> operator"" _nm (double x) { return x*1e-9*si::meters; } ///< Nanometer.
constexpr quantity<si::length, double> operator"" _pm (double x) { return x*1e-12*si::meters; } ///< Picometer.
constexpr quantity<si::length, double> operator"" _fm (double x) { return x*1e-15*si::meters; } ///< Femtometer.
constexpr quantity<si::length, double> operator"" _am (double x) { return x*1e-18*si::meters; } ///< Attometer.

constexpr quantity<si::length, double> operator"" _angstrom (double x) { return x*1e-10*si::meters; } ///< Angström.
constexpr quantity<si::length, double> operator"" _ang (double x) { return x*1e-10*si::meters; } ///< Angström.

constexpr quantity<si::length, double> operator"" _inch (double x) { return x*2.54*cm; }  ///< Inch. REUSE!
constexpr quantity<si::length, double> operator"" _in (double x) { return x*2.54*cm; }  ///< Inch. REUSE!
constexpr quantity<si::length, double> operator"" _mil (double x) { return x*1e-3*in; } ///< Mil
constexpr quantity<si::length, double> operator"" _feet (double x) { return x*12*in; } ///< Feet. (Imperial)
constexpr quantity<si::length, double> operator"" _ft (double x) { return x*12*in; } ///< Feet. (Imperial)
constexpr quantity<si::length, double> operator"" _mile (double x) { return x*5280*ft; } ///< Mile. (Imperial)
constexpr quantity<si::length, double> operator"" _mi (double x) { return x*5280*ft; } ///< Mile. (Imperial)

constexpr quantity<si::length, double> operator"" _ua (double x) { return x*1.49598e11*si::meters; } ///< Astronomical Unit
constexpr quantity<si::length, double> operator"" _ly (double x) { return x*9460730472580.8*km; } ///< Light-year

// Time
constexpr quantity<si::time, double> operator"" _second (double x) { return x*si::second; } ///< Second.
constexpr quantity<si::time, double> operator"" _s (double x) { return x*si::second; } ///< Second
constexpr quantity<si::time, double> operator"" _minute (double x) { return x * 60*s; } ///< Minute.
constexpr quantity<si::time, double> operator"" _min (double x) { return x * 60*s; } ///< Minute.
constexpr quantity<si::time, double> operator"" _hour (double x) { return x * 60*min; } ///< Hour.
constexpr quantity<si::time, double> operator"" _h (double x) { return x * 60*min; } ///< Hour.
constexpr quantity<si::time, double> operator"" _day (double x) { return x * 24*hour; } ///< Day.
constexpr quantity<si::time, double> operator"" _d (double x) { return x * 24*hour; } ///< Day
constexpr quantity<si::time, double> operator"" _year (double x) { return x*365*day; } // Expanding Earth!
// Frequency
constexpr quantity<si::time, double> operator""  _Hz (double x) { return x      /si::second; }
constexpr quantity<si::time, double> operator"" _kHz (double x) { return x*1e-3 /si::second; }
constexpr quantity<si::time, double> operator"" _MHz (double x) { return x*1e-6 /si::second; }
constexpr quantity<si::time, double> operator"" _GHz (double x) { return x*1e-9 /si::second; }
constexpr quantity<si::time, double> operator"" _THz (double x) { return x*1e-12/si::second; }
constexpr quantity<si::time, double> operator"" _PHz (double x) { return x*1e-15/si::second; }
constexpr quantity<si::time, double> operator"" _EHz (double x) { return x*1e-18/si::second; }

// Volume
constexpr quantity<si::volume, double> operator"" _litre (double x) { return x*dm*dm*dm; } ///< Litre
constexpr quantity<si::volume, double> operator"" _L (double x) { return x*dm*dm*dm; } ///< Litre
constexpr quantity<si::volume, double> operator"" _cubicmeter (double x) { return x*m*m*m; } ///< Cubicmeter

// Mass
constexpr quantity<si::mass, double> operator"" _gram (double x) { return x*si::gram; } ///< Gram
constexpr quantity<si::mass, double> operator"" _g (double x) { return x*si::gram; } ///< Gram
constexpr quantity<si::mass, double> operator"" _kg (double x) { return x*1e3*g; } ///< Kilogram
constexpr quantity<si::mass, double> operator"" _ton (double x) { return x*1e3*kg; } ///< Ton
constexpr quantity<si::mass, double> operator"" _t (double x) { return x*1e3*kg; } ///< Ton
constexpr quantity<si::mass, double> operator"" _u (double x) { return x*1.66054e-27kg; } ///< Unified Atomic Mass Unit.

// Speed
constexpr quantity<si::velocity, double> operator"" _mps (double x) { return x*(si::meter / si::second); } ///< Meter per second
constexpr quantity<si::velocity, double> operator"" _kmph (double x) { return x*(si::km / si::hour); } ///< Kilometer per hour

// Energy
constexpr quantity<si::energy, double> operator"" _eV (double x) { return x*1.60218e0-19J; }

// TODO:
// u.mps = u.m/u.s;                        % Metre per second (http://en.wikipedia.org/wiki/Meters_per_second)
// u.mph = u.mi/u.hr;                      % Miles per hour (http://en.wikipedia.org/wiki/Miles_per_hour)
// u.kmph = u.km/u.hr;                     % Kilometers per hour (http://en.wikipedia.org/wiki/Kilometres_per_hour)
// u.kph = u.km/u.hr;                      % Kilometers per hour (http://en.wikipedia.org/wiki/Kilometres_per_hour)
// u.mach1 = 340.29*u.m/u.s;               % Speed of Sound in Air
// u.fts = u.ft/u.s;                       % Feets per second
// u.knots = 1.852*u.kmph;                 % Nautical miles per hour (http://en.wikipedia.org/wiki/Knot_(unit))
// u.kn = 1.852*u.kmph;
// u.kts = 1.852*u.kmph;
// %% Speed (Velocity)
// u.c = 2.99792458e8*u.m/u.s;             % Speed/Velocity of Light in Vacuum

// %% Acceleration ------
// u.g = 9.80665*u.m/u.s^2;                % Earth Gravity (http://en.wikipedia.org/wiki/Earth's_gravity)

// %% Frequency ----
// u.Hz = 1/u.s;                           % Hertz
// u.kHz = 1e3 *u.Hz;
// u.MHz = 1e6 *u.Hz;
// u.GHz = 1e9 *u.Hz;
// u.THz = 1e12 *u.Hz;
// u.PHz = 1e15 *u.Hz;
// u.EHz = 1e18 *u.Hz;

// %% Force -------
// u.N = u.kg*u.m/u.s^2;                   % Newton (http://en.wikipedia.org/wiki/Newton)
// u.dyne = 1e-5*u.N;                      % Dyne (http://en.wikipedia.org/wiki/Dyne)
// u.erg = u.dyne * u.cm;
// %u.lbf = 4.44822*u.N;
// u.lbf = u.lb * u.g;                     % Pound Force (http://en.wikipedia.org/wiki/Pound-force)
// u.poise = u.gm / u.cm / u.s;

// %% Energy -----
// u.J = u.N*u.m;                          % Joule
// u.GJ = 1e9*u.J;
// u.MJ = 1e6*u.J;
// u.kJ = 1e3*u.J;
// u.mJ = 1e-3*u.J;
// u.uJ = 1e-6*u.J;
// u.nJ = 1e-9*u.J;
// u.eV = 1.6022e-19*u.J;                  % Electron Volt
// u.BTU = 1.05505585e3*u.J;                % British thermal unit (http://en.wikipedia.org/wiki/British_thermal_unit)
// u.cal = 4.1868*u.J;                     % Calorie
// u.kCal = 1e3*u.cal;                     % Kilocalorie

// %% Temperature ---
// u.K = 1e3*u.K;                          % Kelvin
// u.mK = 1e-3*u.K;
// u.uK = 1e-6*u.K;
// u.nK = 1e-9*u.K;

// %% Pressure -----
// u.Pa = u.N/u.m^2;                       % Pascal
// u.bar = 1e5*u.Pa;
// u.mbar = 1e-3*u.bar;
// u.atm = 1.013e5*u.Pa;                   % Atmospheric Pressure
// u.torr = u.atm / 760;
// u.mtorr = 1e-3*u.torr;
// u.psi = 6.895e3*u.Pa;

// %% Power --- ---
// u.W = u.J/u.s;                          % Watt
// u.TW = 1e12*u.W;
// u.GW = 1e9*u.W;
// u.MW = 1e6*u.W;
// u.kW = 1e3*u.W;
// u.mW = 1e-3*u.W;
// u.uW = 1e-6*u.W;
// u.nW = 1e-9*u.W;
// u.pW = 1e-12*u.W;
// u.hp = 745.69987*u.W;                   % Horsepower

// %% Energy
// u.kWh = u.kW * u.hr;                    % Kilo Watt Hour

// %% Charge ------
// u.e = 1.602176487e-19 * u.C;            % Electron charge

// %% Voltage -----
// u.V = 1*u.J/u.C;                        % Volt
// u.kV = 1e3*u.V;                         % KiloVolt
// u.MV = 1e3*u.V;                         % MegaVolt
// u.mV = 1e-3*u.V;                        % MilliVolt
// u.uV = 1e-6*u.V;                        % MicroVolt
// u.nV = 1e-9*u.V;                        % NanoVolt

// %% Current ------
// u.A = 1*u.C/u.s;                     % Ampere
// u.mA = 1e-3*u.A;
// u.uA = 1e-6*u.A;
// u.nA = 1e-9*u.A;

// %% Magnetism Field (Magnetic Flux Density) -----
// u.T = 1*u.N/(u.A*u.m);                  % Tesla
// u.gauss = 1e-4*u.T;                     % Gauss (http://en.wikipedia.org/wiki/Gauss_(unit))

// %% Magnetism Flux -----
// u.Wb = u.V * u.s;                       % Magnetic Flux in Weber (http://en.wikipedia.org/wiki/Weber_(unit));
// u.Mx = 1e-8 * u.Wb;                      % Maxwell (http://en.wikipedia.org/wiki/Maxwell_(unit))

// %% Resistance -----
// u.ohm = u.V / u.A;                      % Ohm
// u.siemens = 1 / u.ohm;                  % Siemens

// %% Inductance
// u.H = u.Wb / u.A;                       % Henry (http://en.wikipedia.org/wiki/Henry_(unit))

// %% Capacitance
// u.F = u.C / u.V;                                 % Farad (http://en.wikipedia.org/wiki/Henry)
// u.mF = 1e-3 * u.F;
// u.uF = 1e-6 * u.F;
// u.nF = 1e-9 * u.F;
// u.pF = 1e-12 * u.F;

// %% Fundamental constants ----
// u.kB = 1.38e-23*u.J/u.K;
// u.sigma_SB = 5.670e-8 * u.W/(u.m^2 * u.K^4);
// u.h = 6.62606896e-34 * u.J * u.s;       % Planck's constant
// u.hbar = u.h/(2*pi);
// u.mu_B = 9.274e-24 * u.J/u.T;
// u.mu_N = 5.0507866e-27 * u.J/u.T;
// u.eps0 = 8.8541878176204e-12* u.C/(u.V*u.m);
// %u.mu0 = 1.2566370614359e-6 * u.J/(u.m*u.A^2);
// u.mu0 = 4 * pi * u.erg / u.J * u.H / u.m;% Permeability of free space (http://en.wikipedia.org/wiki/Vacuum_permeability)
// u.k = 1.3806504e-23 * u.J / u.K;        % Boltzman constant
// u.G = 6.67428e-11 * u.m ^3 / (u.s ^2 * u.kg); % Gravitational constant (http://en.wikipedia.org/wiki/Gravitational_constant)

// %%---- Particle masses (u)----
// u.u_e = 5.4857990943e-4;
// u.u_p = 1.00727646677;
// u.u_n = 1.00866491597;
// u.u_D = 2.013553212724;
// u.u_T = 3.0155007134;
// u.u_He3 = 3.0149322473;
// u.u_a = 4.001506179127;

// %%---- Mass Ratios ----
// u.p_e = u.u_p / u.u_e;
// u.D_e = u.u_D / u.u_e;
// u.T_e = u.u_T / u.u_e;
// u.He3_e = u.u_He3 / u.u_e;
// u.a_e = u.u_a / u.u_e;

// %%---- Particle masses (kg)----
// u.m_e = u.u_e * u.u;
// u.m_p = u.u_p * u.u;
// u.m_n = u.u_n * u.u;
// u.m_D = u.u_D * u.u;
// u.m_T = u.u_T * u.u;
// u.m_He3 = u.u_He3 * u.u;
// u.m_a = u.u_a * u.u;

// %%---- Lande g factors ----
// u.g_e = -2.0023193043622;
// u.g_p = 5.585694713;
// u.g_n = -3.82608545;
// u.g_D = 0.8574382308;
// u.g_T = 5.957924896;

}}

// Physical Constants

/*! Imaginary Numbers
 * \example auto val = 3.14i;
 * \example auto val = complex<long double>(0, 3.14)
 */
std::complex<long double> operator "" _i(long double d) // cooked form
{
    return std::complex<long double>(0, d);
}

/*! Binary Values
 * \example auto answer = 101010_B;
 * \example auto answer = 42
 */
std::string operator "" _s(const char* str, size_t /*length*/)
{
    return std::string(str);
}

//int operator "" _B(const char*); // raw form

#endif

/* ========================================================================= */

namespace pnw {

/*! Convert and \p x to use SI Numerical Suffixes. */
template<class T>
inline std::pair<T,char> siunit_convert(const T& x)
{
    const int l = static_cast<int>(std::floor(std::log10(x))) / 3; // logarithm
    const T y = x * std::pow(10, static_cast<T>(-3) * l); // adjusted value. TODO: Enable std::pow10 or std::exp10.
    char u = 0;                 // si unit character default to no suffix
    if      (l >= 6) { u = 'E'; }
    else if (l <= -6) { u = 'a'; }
    else {
        switch (l) {
        case 5: u = 'P'; break;
        case 4: u = 'T'; break;
        case 3: u = 'G'; break;
        case 2: u = 'M'; break;
        case 1: u = 'k'; break;
        case 0: u = 0; break;
        case -1: u = 'm'; break;
        case -2: u = 'u'; break;
        case -3: u = 'n'; break;
        case -4: u = 'p'; break;
        case -5: u = 'f'; break;
        default: u = 'a'; break;
        }
    }
    return std::make_pair(y, u);
}

/* ========================================================================= */

}
