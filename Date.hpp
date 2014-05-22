#pragma once

#include <iostream>

class Date
{
public:
    Date(unsigned int y, unsigned int m, unsigned int d) { set_to(y, m, d); }

    void set_to(unsigned int y, unsigned int m, unsigned int d);

    unsigned int get_year() const { return year; }
    unsigned int get_month() const { return month; }
    unsigned int get_day() const { return day; }

    void print(std::ostream & os) const;

    friend std::ostream & operator << (std::ostream & os, const Date & date)
        {
            date.print(os);
            return os;
        }

protected:
private:
    unsigned int year, month:4, day:5;
};
