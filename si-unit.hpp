#ifndef _SI_H_INCLUDED
#define _SI_H_INCLUDED

// SI unit library for C++
// Copyright (C) 2007  Florian Negele
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// SEE: http://blogs.ethz.ch/negelef/2007/06/21/c-si-library/

#include <iosfwd>

namespace si
{
	template<int m, int kg, int s, int A, int K, int mol, int cd, unsigned den = 1, unsigned div = 1>
	class unit {};

	template<typename S, typename U>
	class quantity {};

	template<typename S, unsigned den, unsigned div>
	class quantity<S, unit<0, 0, 0, 0, 0, 0, 0, den, div> >
	{
	public:
		quantity () {}
		quantity (const S& v) : value (v) {}

		template<typename S0, unsigned den0, unsigned div0>
		quantity (const quantity<S0, unit<0, 0, 0, 0, 0, 0, 0, den0, div0> >& q) :
			value (q * den0 * div / den / div0) {}

		operator S& () {return value;}
		operator const S& () const {return value;}

		friend quantity<S, unit<0, 0, 0, 0, 0, 0, 0> > Base (const quantity& q)
			{return quantity<S, unit<0, 0, 0, 0, 0, 0, 0> > (q.value * den / div);}

	private:
		S value;
	};

	template<typename S, int m, int kg, int s, int A, int K, int mol, int cd, unsigned den, unsigned div>
	class quantity<S, unit<m, kg, s, A, K, mol, cd, den, div> >
	{
	public:
		quantity () {}
		explicit quantity (const S& v) : value (v) {}

		template<typename S0, unsigned den0, unsigned div0>
		quantity (const quantity<S0, unit<m, kg, s, A, K, mol, cd, den0, div0> >& q) :
			value (scalar (q) * den0 * div / den / div0) {}

		quantity& operator += (const quantity& q) {value += q.value; return *this;}
		quantity& operator -= (const quantity& q) {value -= q.value; return *this;}
		quantity& operator *= (const S& v) {value *= v; return *this;}
		quantity& operator /= (const S& v) {value /= v; return *this;}
		quantity& operator %= (const S& v) {value %= v; return *this;}

		bool operator < (const quantity& q) const {return value < q.value;}
		bool operator > (const quantity& q) const {return value > q.value;}
		bool operator <= (const quantity& q) const {return value <= q.value;}
		bool operator >= (const quantity& q) const {return value >= q.value;}
		bool operator == (const quantity& q) const {return value == q.value;}
		bool operator != (const quantity& q) const {return value != q.value;}

		quantity operator + () const {return quantity (+value);}
		quantity operator - () const {return quantity (-value);}

		quantity operator + (const quantity& q) const {return quantity (*this) += q;}
		quantity operator - (const quantity& q) const {return quantity (*this) -= q;}
		quantity operator * (const S& v) const {return quantity (value * v);}
		quantity operator / (const S& v) const {return quantity (value / v);}

		friend quantity<S, unit<m, kg, s, A, K, mol, cd, den, div> > operator * (const S& v, const quantity& q)
			{return quantity<S, unit<m, kg, s, A, K, mol, cd, den, div> > (v * q.value);}

		friend quantity<S, unit<-m, -kg, -s, -A, -K, -mol, -cd, div, den> > operator / (const S& v, const quantity& q)
			{return quantity<S, unit<-m, -kg, -s, -A, -K, -mol, -cd, div, den> > (v / q.value);}

		template<typename S0, int m0, int kg0, int s0, int A0, int K0, int mol0, int cd0, unsigned den0, unsigned div0>
		quantity<S, unit<m+m0, kg+kg0, s+s0, A+A0, K+K0, mol+mol0, cd+cd0, den*den0, div*div0> > operator * (const quantity<S0, unit<m0, kg0, s0, A0, K0, mol0, cd0, den0, div0> >& q) const
			{return quantity<S, unit<m+m0, kg+kg0, s+s0, A+A0, K+K0, mol+mol0, cd+cd0, den*den0, div*div0> > (value * scalar (q));}

		template<typename S0, int m0, int kg0, int s0, int A0, int K0, int mol0, int cd0, unsigned den0, unsigned div0>
		quantity<S, unit<m-m0, kg-kg0, s-s0, A-A0, K-K0, mol-mol0, cd-cd0, den*div0, div*den0> > operator / (const quantity<S0, unit<m0, kg0, s0, A0, K0, mol0, cd0, den0, div0> >& q) const
			{return quantity<S, unit<m-m0, kg-kg0, s-s0, A-A0, K-K0, mol-mol0, cd-cd0, den*div0, div*den0> > (value / scalar (q));}

		friend S scalar (const quantity& q) {return q.value;}

		friend quantity<S, unit<m, kg, s, A, K, mol, cd> > base (const quantity& q)
			{return quantity<S, unit<m, kg, s, A, K, mol, cd> > (q.value * den / div);}

		template<typename C, typename T>
		friend std::basic_ostream<C, T>& operator << (std::basic_ostream<C, T>& os, const quantity& q)
			{return os << q.value;}

		template<typename C, typename T>
		friend std::basic_istream<C, T>& operator >> (std::basic_istream<C, T>& is, quantity& q)
			{return is >> q.value;}

	private:
		S value;
	};

	// base units
	typedef unit< 1, 0, 0, 0, 0, 0, 0> metre, meter;
	typedef unit< 0, 1, 0, 0, 0, 0, 0> kilogram;
	typedef unit< 0, 0, 1, 0, 0, 0, 0> second;
	typedef unit< 0, 0, 0, 1, 0, 0, 0> ampere;
	typedef unit< 0, 0, 0, 0, 1, 0, 0> kelvin;
	typedef unit< 0, 0, 0, 0, 0, 1, 0> mole;
	typedef unit< 0, 0, 0, 0, 0, 0, 1> candela;

	// derived units
	typedef unit< 0, 0,-1, 0, 0, 0, 0> hertz, becquerel;
	typedef unit< 1, 1,-2, 0, 0, 0, 0> newton;
	typedef unit< 2, 1,-2, 0, 0, 0, 0> joule;
	typedef unit< 2, 1,-3, 0, 0, 0, 0> watt;
	typedef unit<-1, 1,-2, 0, 0, 0, 0> pascal;
	typedef unit< 0, 0, 0, 0, 0, 0, 1> lumen;
	typedef unit<-2, 0, 0, 0, 0, 0, 1> lux;
	typedef unit< 0, 0, 1, 1, 0, 0, 0> coloumb;
	typedef unit< 2, 1,-3,-1, 0, 0, 0> volt;
	typedef unit< 2, 1,-3,-2, 0, 0, 0> ohm;
	typedef unit<-2,-1, 4,-2, 0, 0, 0> farad;
	typedef unit< 2, 1,-2,-1, 0, 0, 0> weber;
	typedef unit< 0, 1,-2,-1, 0, 0, 0> tesla;
	typedef unit< 2, 1,-2,-2, 0, 0, 0> henry;
	typedef unit<-2,-1, 3, 2, 0, 0, 0> siemens;
	typedef unit< 2, 0,-2, 0, 0, 0, 0> gray, sievert;
	typedef unit< 0, 0,-1, 0, 0, 1, 0> katal;

	// converted units
	typedef unit< 1, 0, 0, 0, 0, 0, 0, 1143, 1000>	ell;
	typedef unit< 1, 0, 0, 0, 0, 0, 0, 1143, 1250>	yard;
	typedef unit< 1, 0, 0, 0, 0, 0, 0, 381, 1250>	foot;
	typedef unit< 1, 0, 0, 0, 0, 0, 0, 127, 5>	inch;
	typedef unit< 1, 0, 0, 0, 0, 0, 0, 1, 1000>	millimetre, millimeter;
	typedef unit< 1, 0, 0, 0, 0, 0, 0, 1, 100>	centimetre, centimeter;
	typedef unit< 1, 0, 0, 0, 0, 0, 0, 1, 10>	decimetre, decimeter;
	typedef unit< 1, 0, 0, 0, 0, 0, 0, 1000, 1>	kilometer, kilometre;
	typedef unit< 2, 0, 0, 0, 0, 0, 0, 1, 10000>	are;
	typedef unit< 2, 0, 0, 0, 0, 0, 0, 1, 100>	hectare;
	typedef unit< 3, 0, 0, 0, 0, 0, 0, 1, 1000>	liter;
	typedef unit< 0, 1, 0, 0, 0, 0, 0, 1, 1000>	gram;
	typedef unit< 0, 1, 0, 0, 0, 0, 0, 1, 5000>	carat;
	typedef unit< 0, 1, 0, 0, 0, 0, 0, 1, 2>	pound;
	typedef unit< 0, 0, 1, 0, 0, 0, 0, 1, 1000>	millisecond;
	typedef unit< 0, 0, 1, 0, 0, 0, 0, 90, 1>	moment;
	typedef unit< 0, 0, 1, 0, 0, 0, 0, 60, 1>	minute;
	typedef unit< 0, 0, 1, 0, 0, 0, 0, 432, 5>	milliday;
	typedef unit< 0, 0, 1, 0, 0, 0, 0, 864, 1>	ke;
	typedef unit< 0, 0, 1, 0, 0, 0, 0, 900, 1>	quarter;
	typedef unit< 0, 0, 1, 0, 0, 0, 0, 3600, 1>	hour;
	typedef unit< 0, 0, 1, 0, 0, 0, 0, 86400, 1>	day;
	typedef unit< 0, 0, 1, 0, 0, 0, 0, 604800, 1>	week;
	typedef unit< 1, 0,-1, 0, 0, 0, 0, 514444, 1000000>	knot;
	typedef unit< 1, 0,-2, 0, 0, 0, 0, 1, 100>	galileo;
	typedef unit< 1, 0,-2, 0, 0, 0, 0, 9807, 1000>	gravity;
	typedef unit<-1, 1,-2, 0, 0, 0, 0, 980665, 10000>	atmosphere;
	typedef unit<-1, 1,-2, 0, 0, 0, 0, 1000000, 1>	bar;
	typedef unit<-1, 1,-2, 0, 0, 0, 0, 1, 10>	barye;
	typedef unit< 2, 1,-3, 0, 0, 0, 0, 73549875, 100000> horsepower;
	typedef unit< 2, 1,-3,-1, 0, 0, 0, 37474, 125> statvolt;

	// quantities
	typedef unit< 1, 0, 0, 0, 0, 0, 0> Length, Distance;
	typedef unit< 0, 1, 0, 0, 0, 0, 0> Mass;
	typedef unit< 0, 0, 1, 0, 0, 0, 0> Time, Duration;
	typedef unit< 0, 0, 0, 1, 0, 0, 0> Current;
	typedef unit< 0, 0, 0, 0, 1, 0, 0> Temperature;
	typedef unit< 0, 0, 0, 0, 0, 1, 0> Matter;
	typedef unit< 0, 0, 0, 0, 0, 0, 1> Intensity, LuminousFlux;
	typedef unit< 0, 0,-1, 0, 0, 0, 0> Frequency, Radioactivity;
	typedef unit< 1, 1,-2, 0, 0, 0, 0> Force, Weight;
	typedef unit< 2, 1,-2, 0, 0, 0, 0> Energy, Work, Heat;
	typedef unit< 2, 1,-3, 0, 0, 0, 0> Power, RadiandFlux;
	typedef unit<-1, 1,-2, 0, 0, 0, 0> Pressure, Stress;
	typedef unit< 0, 0, 1, 1, 0, 0, 0> ElectricCharge, ElectricFlux;
	typedef unit< 2, 1,-3,-1, 0, 0, 0> Voltage, ElectricalPotentialDifference, ElectromotiveForce;
	typedef unit< 2, 1,-3,-2, 0, 0, 0> ElectricResistance, Impedance, Reactance;
	typedef unit<-2,-1, 4,-2, 0, 0, 0> ElectricCapacitance;
	typedef unit< 2, 1,-2,-1, 0, 0, 0> MagneticFlux;
	typedef unit< 0, 1,-2,-1, 0, 0, 0> MagneticFluxDensity, MagneticInduction;
	typedef unit< 2, 1,-2,-2, 0, 0, 0> Inductance;
	typedef unit<-2,-1, 3, 2, 0, 0, 0> ElectricConductance;
	typedef unit< 2, 0,-2, 0, 0, 0, 0> AbsordedDose, EquivalentDose;
	typedef unit< 0, 0,-1, 0, 0, 1, 0> CatalyticActivity;
	typedef unit< 2, 0, 0, 0, 0, 0, 0> Area;
	typedef unit< 3, 0, 0, 0, 0, 0, 0> Volume;
	typedef unit< 1, 0,-1, 0, 0, 0, 0> Speed, Velocity;
	typedef unit< 1, 0,-2, 0, 0, 0, 0> Acceleration;
	typedef unit< 1, 0,-3, 0, 0, 0, 0> Jerk;
	typedef unit< 0, 0,-1, 0, 0, 0, 0> AngularVelocity;
	typedef unit< 1, 1,-1, 0, 0, 0, 0> Momentum, Impulse;
	typedef unit< 2, 1,-1, 0, 0, 0, 0> AngularMomentum;
	typedef unit< 2, 1,-2, 0, 0, 0, 0> Torque, ForceMomentum;
	typedef unit<-1, 0, 0, 0, 0, 0, 0> WaveNumber;
	typedef unit<-3, 1, 0, 0, 0, 0, 0> Density;
	typedef unit< 3,-1, 0, 0, 0, 0, 0> SpecificDensity;
	typedef unit<-3, 0, 0, 0, 0, 1, 0> Concentration;
	typedef unit< 3, 0,-1, 0, 0, 0, 0> MolarVolume;
}

#endif // _SI_H_INCLUDED
