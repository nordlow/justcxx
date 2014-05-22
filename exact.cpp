/*!
 * \file exact.cpp
 * \brief Classes for exact arithmetic.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cfloat>
#include <cstring>

#include <algorithm>            // swap
#include <iostream>
#include <vector>
#include <stack>
#include <complex>

using std::cout;
using std::cin;
using std::cerr;
using std::endl;

#include "Timer.hpp"

#include "utils.h"

namespace X {
extern "C" {
#include "xwin.h"
}
}

// ============================================================================

class Basic;                    // basic

class Sym;                      // symbol
class Const;                    // constant
class Num;                      // numeric

class Add;                      // sums (like x+y)
class Mul;                      // products (like x*y)
class NcMul;                    // products of non-commutative objects

class Pow;                      // exponentials such as x^2, a^b
class PSeries;         // power series (x - 1/3!*x^3 + 1/5!*x^5 + ...)

class Fun;                      // function
class UFun;                     // unary function (taking 1 argument)
class BFun;                    // binary function (taking 2 arguments)
class NFun;                     // n-ary function (taking n arguments)

class Lst;                      // list (of expressions)
class Mat;                   // matrix (m x n matrices of expressions)

class Rel;                      // relation (x == y)

class Indexed;                  // indexed object (like A_ij)
class Wildcard;                 // wildcard for pattern matching

class Ex;                       // expression (wrapper)

void test ();

// ============================================================================

class Basic
{
public:
    typedef enum
    {
        SYM, CONST, NUM, UFUN, BFUN
    } Class;

    Basic() : refcnt(0) {};

    virtual Basic *clone() const = 0;
    virtual ~Basic() {};

    virtual Class getClass() const = 0;
    virtual void simplify() = 0;

    // io
    virtual void print(std::ostream & os) const = 0;
private:
    size_t refcnt;                // number of reference counts
};

// ============================================================================

class Sym : public Basic
{
public:
    // construct
    Sym(const char *s)
    {
	size_t len = std::strlen(s);
	str = new char [len+1];
	std::strcpy(str, s);
    }
    Sym(const char ch)
    {
	str = new char [2];
	str[0] = ch;
	str[1] = '\0';
    }

    // copy construct
    Sym(const Sym & sym)
    {
	size_t len = std::strlen(sym.str);
	this->str = new char [len+1];
	std::strcpy(this->str, sym.str);
    }

    Basic *clone() const { return new Sym(str); }

    ~Sym() { delete str; }     // destruct

    operator Ex() const;     // type cast

    Class getClass() const { return SYM; }

    void simplify() {}

    // io
    virtual void print(std::ostream & os) const { os << str; }
    friend std::ostream & operator << (std::ostream & os, const Sym & a)
    {
	a.print(os); return os;
    }
private:
    char *str;
};

// ============================================================================

class Const : public Basic
{
public:
    // enum
    typedef enum
    {
        PI, E
    }
	Type;

    Const(Type type) : type(type) {}
    Basic *clone() const { return new Const(type); }
    ~Const() {}                // destruct

    operator Ex() const;     // type cast

    Class getClass() const { return CONST; }

    void simplify() {}

    // io
    virtual void print(std::ostream & os) const;
    friend std::ostream & operator << (std::ostream & os, const Const & a)
    {
	a.print(os); return os;
    }

    // operators
    friend Ex operator + (const Const & a, const Const & b);

private:
    const Type type;
};

void Const::print(std::ostream & os) const
{
    static const char * const type_names[] =
        {
            "PI", "E",
        };
    os << type_names[type];
}

// ============================================================================

class Num : public Basic
{
public:
    Num(uint num) : num(num) {} // construct
    Basic *clone() const { return new Num(num); }
    ~Num() {}                  // destruct

    operator Ex() const;     // type cast

    bool isInt() const { return true; }
    uint get_uint() const { return num; }
    Class getClass() const { return NUM; }

    void simplify() {}

    // io
    virtual void print(std::ostream & os) const { os << num; }
    friend std::ostream & operator << (std::ostream & os, const Num & a)
    {
	a.print(os); return os;
    }
private:
    const uint num;
};

// ============================================================================

class UFun : public Basic
{
public:
    // NOTE: Do not forget to sync changes with strings in print.
    typedef enum {
        NEG, INV, SQRT, COS, SIN, TAN, ACOS, ASIN, ATAN,
    } Type;

    Type get_type() const { return type; }

    // construct
    // \todo CONst Basic * a, use arg (new Basic (a)) instead?
    UFun(Type type, const Basic *a) : type(type), arg(a->clone()) {}
    UFun(Type type, const Sym & s) : type(type), arg(new Sym(s)) {}
    UFun(Type type, const Const & c) : type(type), arg(new Const(c)) {}
    UFun(Type type, const Num & n) : type(type), arg(new Num(n)) {}

    // copy construct
    UFun(const UFun & ufun) : type(ufun.type), arg(ufun.arg->clone()) {}

    Basic *clone() const { return new UFun(type, arg); }

    ~UFun() { delete arg; }    // destruct

    operator Ex() const;     // type case

    Class getClass() const { return UFUN; }

    void simplify();

    // io
    void print(std::ostream & os) const;
    friend std::ostream & operator << (std::ostream & os, const UFun & a)
    {
	a.print(os); return os;
    }
private:
    const Type type;
    Basic *arg;
};

void UFun::print(std::ostream & os) const
{
    static const char * const type_ops[] =
        {
            "-", "inv", "sqrt", "cos", "sin", "tan", "acos", "asin", "atan",
        };
    os << type_ops[type];
    os << '(';
    arg->print(os);
    os << ')';
}

void UFun::simplify()
{
    while (true)
    {
        arg->simplify();   // recurse
        if (arg->getClass() == UFUN)
        {
            const UFun & ufun = *(UFun*)arg;
            if (type == ufun.get_type())
            {
                switch (type)
                {
                case NEG: cout << "-(-(x)) == x" << endl; break;
                case INV: cout << "inv(inv(x)) == x" << endl; break;
                default: break;
                };
            }
            if (type == COS and ufun.get_type() == ACOS)
            {
                cout << "cos(acos(x) == x" << endl;
                continue;
            }
        }
        break;
    }
}

// ============================================================================

class BFun : public Basic
{
    friend class Ex;
public:
    // NOTE: Do not forget to sync changes with strings in print.
    typedef enum {
        ADD, SUB, MUL, DIV, POW, GCD, LCM
    } Type;

    Type get_type() const { return type; }

    BFun(Type type) : type(type) {}

    BFun(Type type,
	 const Basic * arg0,
	 const Basic * arg1) : type(type)
    {
	arg[0] = arg0->clone();
	arg[1] = arg1->clone();
    }

    // copy construct
    BFun(const BFun & bfun) : type(bfun.type)
    {
	arg[0] = bfun.arg[0]->clone();
	arg[1] = bfun.arg[1]->clone();
    }

    Basic *clone() const { return new BFun(type, arg[0], arg[1]); }

    ~BFun() { delete arg[0]; delete arg[1]; }

    operator Ex() const;     // type cast

    Class getClass() const { return BFUN; }

    void simplify() {}

    // io
    void print(std::ostream & os) const;
    friend std::ostream & operator << (std::ostream & os, const BFun & a)
    {
	a.print(os); return os;
    }
private:
    const Type type;
    Basic *arg[2];
};

void BFun::print(std::ostream & os) const
{
    static const char * const type_ops[] =
        {
            "+", "-", "*", "/", "pow", "gcd", "lcm"
        };
    if (type == ADD or
        type == SUB or
        type == MUL or
        type == DIV)
    {
        os << '(';
        arg[0]->print(os);
        os << ' ' << type_ops[type] << ' ';
        arg[1]->print(os);
        os << ')';
    }
    else
    {
        os << type_ops[type];
        os << '(';
        arg[0]->print(os);
        os << ", ";
        arg[1]->print(os);
        os << ')';
    }
}

// ============================================================================

class NFun : public Basic
{
public:
};

// ============================================================================

class Ex {
public:
    // construct
    Ex(const Sym & s)   { root = new Sym(s); }
    Ex(const Const & c) { root = new Const(c); }
    Ex(const Num & n)   { root = new Num(n); }
    Ex(const UFun & u)  { root = new UFun(u); }
    Ex(const BFun & b)  { root = new BFun(b); }
    Ex(const UFun::Type & type, const Ex & e)
    {
	root = new UFun(type, e.root);
    }
    Ex(const BFun::Type & type, const Ex & e0, const Ex & e1)
    {
	root = new BFun(type, e0.root, e1.root);
    }

    // copy construct
    Ex(const Ex & e) { root = e.root->clone(); }

    ~Ex() { delete root; }   // destruct

    // Ex - Ex - binary function
    const Ex & operator += (const Ex & ex)
    {
	BFun * bfun = new BFun(BFun::ADD);
	bfun->arg[0] = root;
	bfun->arg[1] = ex.root->clone();
	root = bfun;
	return *this;
    }
    const Ex & operator -= (const Ex & ex)
    {
	BFun * bfun = new BFun(BFun::SUB);
	bfun->arg[0] = root;
	bfun->arg[1] = ex.root->clone();
	root = bfun;
	return *this;
    }
    const Ex & operator *= (const Ex & ex)
    {
	BFun * bfun = new BFun(BFun::MUL);
	bfun->arg[0] = root;
	bfun->arg[1] = ex.root->clone();
	root = bfun;
	return *this;
    }
    const Ex & operator /= (const Ex & ex)
    {
	BFun * bfun = new BFun(BFun::DIV);
	bfun->arg[0] = root;
	bfun->arg[1] = ex.root->clone();
	root = bfun;
	return *this;
    }

    // Ex - Num - binary function
    const Ex & operator += (const Num & num)
    {
	BFun * bfun = new BFun(BFun::ADD);
	bfun->arg[0] = root;
	bfun->arg[1] = num.clone();
	root = bfun;
	return *this;
    }
    const Ex & operator -= (const Num & num)
    {
	BFun * bfun = new BFun(BFun::SUB);
	bfun->arg[0] = root;
	bfun->arg[1] = num.clone();
	root = bfun;
	return *this;
    }
    const Ex & operator *= (const Num & num)
    {
	BFun * bfun = new BFun(BFun::MUL);
	bfun->arg[0] = root;
	bfun->arg[1] = num.clone();
	root = bfun;
	return *this;
    }
    const Ex & operator /= (const Num & num)
    {
	BFun * bfun = new BFun(BFun::DIV);
	bfun->arg[0] = root;
	bfun->arg[1] = num.clone();
	root = bfun;
	return *this;
    }

    void simplify() { root->simplify(); }

    // substitute
    void subs(const Ex & ex) {}

    // io
    void print(std::ostream & os) const { root->print(os); }
    friend std::ostream & operator << (std::ostream & os, const Ex & a)
    {
	a.print(os); return os;
    }
private:
    Basic *root;
};

// ============================================================================

class Lst : public std::vector<Ex>
{
public:
    // construct
    Lst(const Ex & ex) { push_back(ex); }

    // io
    void print(std::ostream & os) const {
        os << "[ ";
        for (auto i=begin(); i != end(); i++)
        {
            os << *i << ' ';
        }
        os << ']';
    }
    friend std::ostream & operator << (std::ostream & os, const Lst & a)
    {
	a.print(os); return os;
    }
};

// ============================================================================

// type cast:
// provides automatic type conversion used in expr - expr - operators
Sym  ::operator Ex() const { return Ex(*this); }
Const::operator Ex() const { return Ex(*this); }
Num  ::operator Ex() const { return Ex(*this); }
UFun ::operator Ex() const { return Ex(*this); }
BFun ::operator Ex() const { return Ex(*this); }

// Ex - Ex - binary functions

Ex operator + (const Ex & e0, const Ex & e1)
{
    return Ex(BFun::ADD, e0, e1);
}
Ex operator - (const Ex & e0, const Ex & e1)
{
    return Ex(BFun::SUB, e0, e1);
}
Ex operator * (const Ex & e0, const Ex & e1)
{
    return Ex(BFun::MUL, e0, e1);
}
Ex operator / (const Ex & e0, const Ex & e1)
{
    return Ex(BFun::DIV, e0, e1);
}

Ex pow(const Ex & e0, const Ex & e1)
{
    return Ex(BFun::POW, e0, e1);
}
Ex gcd(const Ex & e0, const Ex & e1)
{
    return Ex(BFun::GCD, e0, e1);
}
Ex lcm(const Ex & e0, const Ex & e1)
{
    return Ex(BFun::LCM, e0, e1);
}

// Ex - uint - binary functions

Ex pow(const Ex & e, size_t n)
{
    Ex ans = e;
    for (size_t i=1; i<n; i++)
    {
        ans = ans * e;
    }
    return ans;
}

// Ex - Num - binary functions

Ex pow(const Ex & e, const Num & n)
{
    if (n.isInt())
    {
        return pow(e, n.get_uint());
    } else {
        return pow(e, n);
    }
}

// Ex - unary functions

Ex neg  (const Ex & e) { return Ex(UFun::NEG , e); }
Ex sqrt(const Ex & e) { return Ex(UFun::SQRT, e); }

Ex operator - (const Ex & e) { return neg(e); }
const Ex & operator + (const Ex & e) { return e; }

// ============================================================================

void test()
{
//      Ex e = neg(neg(Num(11) * Sym("x"))) + pow(Sym("a"), Sym("b"));

    Sym x("x");
    Ex e = -(x);
    e.simplify();
    cout << e << endl;

    UFun ufun = UFun(UFun::NEG, x);
    Ex ee = Ex(ufun);
    cout << ee << endl;

    Lst lst(ee);
    cout << lst << endl;

    cout << endl;
}

// ============================================================================

class Unit
{
public:
    typedef enum {
        PREFIX_ATTO, PREFIX_FEMTO, PREFIX_NANO, PREFIX_MICRO, PREFIX_MILLI, PREFIX_CENTI,
        PREFIX_ONE, PREFIX_KILO, PREFIX_MEGA, PREFIX_GIGA, PREFIX_TERA, PREFIX_PETA, PREFIX_EXA,
        PREFIX_CNT
    } Prefix;

    // static const char * const prefix_strs[PREFIX_CNT] = {
    //     "atto", "femto", "nano", "micro", "milli", "centi",
    //     "one", "kilo", "mega", "giga", "tera", "peta", "exa"
    // };

    // base / primitive unit
    typedef enum {
        BASE_METER, BASE_SECOND, BASE_GRAM, BASE_JOULE, BASE_WATT,
        BASE_CNT
    } Base;

    // static const char * const base_strs[BASE_CNT] =
    // {
    //     "meter", "second", "gram", "joule", "watt"
    // };

    Unit(const Prefix & prefix, const Base & base)
        : prefix(prefix), base(base) {}
    Unit(const char *c_str);

    friend Unit operator + (const Unit & a, const Unit & b);
    friend Unit operator - (const Unit & a, const Unit & b);
    friend Unit operator * (const Unit & a, const Unit & b);
    friend Unit operator / (const Unit & a, const Unit & b);
    friend Unit pow(const Unit & a, size_t b);

    // io

    void print(std::ostream & os) const {
        os << prefix << '_' << base;
    }

    friend std::ostream & operator << (std::ostream & os, const Unit & a)
    {
	a.print(os); return os;
    }

private:
    Prefix prefix;
    Base base;
};

Unit::Unit(const char *c_str)
{
    prefix = PREFIX_ONE;
    for(int i = 0; i < BASE_CNT; i++)
    {
//          if(strcasecmp(c_str, base_strs[i]) == 0)
        {
//              base = (Base)i;
//          }
        }
    }

}

// ============================================================================

int main()
{
    for (size_t i=0; ; i++)
    {
        cout << "i:" << i << endl;
        test();
    }
    return 0;
}
