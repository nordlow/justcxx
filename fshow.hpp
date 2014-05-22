/*! \file fshow.hpp
 * \brief Formatted Pretty Output to HTML or LaTeX/TikZ/PGF.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2012-01-18 13:30
 *
 * \note Uses C++11 Raw String Literals: R"()"
 *
 * \todo Support Boost.Units \c name_string \c symbol_string
 * \todo Replace \c ostringstream with \c to_string or \c boost::lexical_cast
 * \todo Use Boost.Units to auto-generate LaTeX Label Units in \c coordinates()
 *
 * \see http://stackoverflow.com/questions/1135841/c-multiline-string-literal
 */

#pragma once

#include <sstream>
#include <fstream>
#include <ostream>
#include <string>
#include <iomanip>
#include "csc.hpp"
#include <vector>
#include <algorithm>

#include <boost/process.hpp>
#include <boost/units/systems/si/io.hpp>
#include <boost/units/quantity.hpp>

#include "show.hpp"
#include "typenameof.hpp"
#include "bitwise.hpp"

namespace fio { namespace latex { namespace tikz {

/*! TikZ XY-Plot */
class XYPlot {
public:
    XYPlot() : m_xbase(0), m_smooth(false) {}

    /// Plot Y-Values \p a.
    template<class Y> XYPlot(const std::vector<Y>& a) : m_xbase(0), m_smooth(false) { coordinates(a); }
    /// Plot XY coordinates \p a.
    template<class X, class Y> XYPlot(const std::vector<std::pair<X, Y>>& a) : m_xbase(0), m_smooth(false) { coordinates(a); }

    XYPlot& color(const csc& a) { m_color = a; return *this; }
    XYPlot& mark(const csc& a) { m_mark = a; return *this; }
    XYPlot& smooth(bool a) { m_smooth = a; return *this; }
    XYPlot& legend(const csc& a) { m_legend = a; return *this; }

    XYPlot& coordinates(const char * a) { m_coordinates = a; return *this; }

    template<class X, class Y>
    XYPlot& coordinates(const std::vector<std::pair<X, Y>>& a) {
        std::ostringstream oss;
        show_each(a, oss, "{", "}");
        m_coordinates = oss.str().c_str();
        m_xbase = detect_power(a);
        m_coordinates_typename_X = typenameof<X>();
        m_coordinates_typename_Y = typenameof<Y>();

        // Run-time warn about mismatching units.
        m_coordinates_unitname_X = unitnameof<X>(a[0].first).c_str(); //std::cout << m_coordinates_unitname_X << std::endl;
        m_coordinates_unitname_Y = unitnameof<Y>(a[0].second).c_str(); //std::cout << m_coordinates_unitname_Y << std::endl;
        return *this;
    }

    // Auto-Detect log_basis_x in \p a and store.
    template<class X, class Y>
    unsigned int detect_power(const std::vector<std::pair<X, Y>>& a) {
        bool common = true;
        unsigned int base = 0;
        std::for_each(std::begin(a), std::end(a),  // C++11
                      [&common, &base] (const std::pair<X, Y>& p) {
                          if (pnw::is_binpow(p.first)) {
                              base = 2;
                          } else {
                              common = false;
                          }
                      });
        return common ? base : 0;
    }

    template<class T> std::string unitnameof(const T& a) { return ""; }
    template<class T> std::string unitnameof(const boost::units::quantity<T, double> & a) { return boost::units::name_string(a); }

    std::ostream& show(std::ostream& os) const {
        os << "\\addplot";
        if (m_color.size() or m_mark.size() or m_smooth) {
            os << "[";
            if (m_color.size()) { os << "color=" << m_color << ','; }
            if (m_mark.size()) { os << "mark=" << m_mark << ','; }
            if (m_smooth) { os << "smooth,"; }
            os << "]";
        }
        os << " coordinates " << m_coordinates << ";" << std::endl;
        if (m_legend.size()) {
            os << "\\addlegendentry{"
               << m_legend;

            auto type_flag = true; // type (precision)
            if (type_flag) {
                if (not m_coordinates_typename_X.empty()) { os << " X:{\\small " << m_coordinates_typename_X << "}"; }
                if (not m_coordinates_typename_Y.empty()) { os << " Y:{\\small " << m_coordinates_typename_Y << "}"; }
            }

            os << "}" << std::endl; }
        return os;
    }

    friend std::ostream& operator << (std::ostream& os, const XYPlot& a) { return a.show(os); }
private:
    unsigned int m_xbase;          ///< X-Axis Logarithm Base.
    csc m_color, m_mark, m_legend; ///< Attributes.
    csc m_coordinates;             ///< Data.
    csc m_coordinates_typename_X;  ///< X Data Type Name.
    csc m_coordinates_typename_Y;  ///< Y Data Type Name.

    csc m_coordinates_unitname_X;
    csc m_coordinates_unitname_Y;

    bool m_smooth:1;               ///< Curve Smoothing Flag.
};

/*! TikZ Picture */
class Picture {
public:
    Picture() : m_log_basis_x(0), m_log_basis_y(0) {}
    template<class ... Plots>
    Picture(Plots &... ps) : m_log_basis_x(0), m_log_basis_y(0) { add(ps...); }

    Picture& add(XYPlot& p) { m_plots.push_back(p); return *this; }
    template<class ... Plots >
    Picture& add(XYPlot& p, Plots &... q) { add(p); add(q...); return *this; }

    Picture& axis(const csc& a) { m_axis = a; return *this; }
    Picture& loglogaxis() { return axis("loglogaxis"); }
    Picture& semilogxaxis() { return axis("semilogxaxis"); }
    Picture& semilogyaxis() { return axis("semilogyaxis"); }

    Picture& log_basis_x(double a) { m_log_basis_x = a; return *this; }
    Picture& log_basis_y(double a) { m_log_basis_y = a; return *this; }

    Picture& grid(const csc& a) { m_grid = a; return *this; } ///< Set Grid Type \p a: "minor", "major", "both", "none".
    Picture& grid_minor() { return grid("minor"); }
    Picture& grid_major() { return grid("major"); }
    Picture& grid_both() { return grid("both"); }
    Picture& grid_none() { return grid("none"); }

    Picture& xlabel(const csc& a) { m_xlabel = a; return *this; }
    Picture& ylabel(const csc& a) { m_ylabel = a; return *this; }
    Picture& title(const csc& a)  { m_title = a; return *this; }
    Picture& width(const csc& a) { m_width = a; return *this; }
    Picture& height(const csc& a) { m_height = a; return *this; }
    Picture& width_r(double a) {
        std::ostringstream oss;
        oss << a; m_width = oss.str().append("\\textwidth").c_str();
        return *this;
    }
    Picture& height_r(double a) {
        std::ostringstream oss;
        oss << a; m_height = oss.str().append("\\textheight").c_str();
        return *this; }


    std::ostream& show_begin(std::ostream& os) const {
        os << "\\begin{tikzpicture}" << std::endl
           << "\\begin{" << (m_axis.size() ? m_axis : "axis") << "}[";
        if (m_log_basis_x) { std::ostringstream oss; oss << m_log_basis_x; os << ",log basis x={" << oss.str() << "}"; }
        if (m_log_basis_y) { std::ostringstream oss; oss << m_log_basis_y; os << ",log basis y={" << oss.str() << "}"; }
        if (m_grid.size()) { os << ",grid={" << m_grid << "}"; }
        if (m_xlabel.size()) { os << ",xlabel={" << m_xlabel << "}"; }
        if (m_ylabel.size()) { os << ",ylabel={" << m_ylabel << "}"; }
        if (m_title.size()) { os << ",title={" << m_title << "}"; }
        if (m_width.size()) { os << ",width={" << m_width << "}"; }
        if (m_height.size()) { os << ",height={" << m_height << "}"; }
        return os << "]" << std::endl;
    }
    std::ostream& show_end(std::ostream& os) const {
        return os << "\\end{" << m_axis << "}" << std::endl
           << "\\end{tikzpicture}" << std::endl;
    }
    std::ostream& show_plots(std::ostream& os) const {
        std::for_each(std::begin(m_plots), std::end(m_plots),  // C++11
                      [&os] (const XYPlot& p) { // value element
                          os << p;
                      });
        return os;
    }
    friend std::ostream& operator << (std::ostream& os, const Picture& a) {
        a.show_begin(os); a.show_plots(os); a.show_end(os); return os; }

private:
    csc m_axis, m_grid, m_xlabel, m_ylabel, m_title, m_width, m_height;
    double m_log_basis_x, m_log_basis_y;
    std::vector<XYPlot> m_plots;  ///< Axis Plots.
};

}

/*! LaTeX Document */
class Document {
public:
    Document() {}

    Document& add(tikz::Picture& p) { m_pics.push_back(p); return *this; }
    template<class... Pics >
    Document& add(tikz::Picture& p, Pics &... q) { add(p); add(q...); return *this; }

    std::ostream& begin(std::ostream& os) const {
        return os << R"(\documentclass[a4paper,10pt,twocolumn]{article}
\usepackage[cm]{fullpage}
\usepackage{underscore}
\usepackage[T1]{fontenc}
\usepackage[utf8]{inputenc}
\usepackage[english]{babel}
\usepackage{pgfplots}
\pgfplotsset{compat=newest,small}
\begin{document})" << std::endl;
    }
    std::ostream& end(std::ostream& os) const { return os << R"(\end{document})" << std::endl; }
    std::ostream& show_pics(std::ostream& os) const {
        std::for_each(std::begin(m_pics), std::end(m_pics),  // C++11
                      [&os] (const tikz::Picture& p) { // value element
                          os << p;
                      });
        return os;
    }
    std::ostream& show(std::ostream& os) const { begin(os); show_pics(os); end(os); return os; }
    friend std::ostream& operator << (std::ostream& os, const Document& a) { return a.show(os); }
private:
    std::vector<tikz::Picture> m_pics;  ///< Pictures.
};

}

// HTML
namespace html {
/*! C++ Output Stream Encoding to HTML. */
class ohtmlstream {
public:
    /*! Semantic Type Specializer (Extenders). */
    typedef enum  {
        br,                         ///< New Row. (RowBreak)
        table_begin,                ///< Table Beginning.
        table_end,                  ///< Table End.
        tabular_begin,              ///< Tabular Begin.
        tabular_end,                ///< Tabular End.
        tab,                        ///< Table.
        col,                        ///< Column.
        row,                        ///< Row.
        path,                       ///< File Path.
        dir,                        ///< Directory Path.
        url,                        ///< URL Path.
    } ctrl_t;
    ohtmlstream() : tabular(false) {}
    ohtmlstream& operator << (const uint32_t& a) { m_oss << a; return *this; }
    ohtmlstream& operator << (const uint64_t& a) { m_oss << a; return *this; }
    // ohtmlstream& operator << (const ctrl_t& a) {
    //     switch (a) {
    //     case table_begin: m_oss << "<table>"; break;
    //     case table_end: m_oss << "</table>"; break;
    //     case tabular_begin: m_oss << "<table>"; tabular = true; break;
    //     case tabular_end: m_oss << "</table>"; tabular = true; break;
    //     case br: m_oss << "<br>"; break;
    //     case col: m_oss << "<td>"; break;
    //     case row: m_oss << "<tr>"; break;
    //     }
    //     return *this;
    // }
protected:
    bool tabular:1;             ///< Auto-table flag.
    std::ostringstream m_oss;   ///< Serializes elements.
};

}
}
