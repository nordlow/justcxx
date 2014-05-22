#include "Date.hpp"

#include <iomanip>

void Date::set_to(unsigned int y, unsigned int m, unsigned int d)
{
    year = y; month = m; day = d;
}

void Date::print(std::ostream & os) const
{
    using std::setw;
    using std::setfill;
    os << year << '-'
       << setw(2) << setfill('0') << month << '-'
       << setw(2) << setfill('0') << day;
}
